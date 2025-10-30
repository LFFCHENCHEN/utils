#include "memcheck.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef USE_MEMORY_TRACE

/* allocation record: replaced linked-list with BST nodes */
typedef struct alloc_info {
	void *ptr;
	size_t size;
	const char *file;
	int line;
	const char *func;
	struct alloc_info *left;
	struct alloc_info *right;
} alloc_info_t;

static alloc_info_t *alloc_root = NULL;
static pthread_mutex_t alloc_lock = PTHREAD_MUTEX_INITIALIZER;

static void lock(void) { pthread_mutex_lock(&alloc_lock); }
static void unlock(void) { pthread_mutex_unlock(&alloc_lock); }

/* helper: create a tracking node */
static alloc_info_t *node_create(void *ptr, size_t size, const char *file, int line, const char *func) {
	alloc_info_t *n = (alloc_info_t *)malloc(sizeof(alloc_info_t));
	if (!n) return NULL;
	n->ptr = ptr;
	n->size = size;
	n->file = file;
	n->line = line;
	n->func = func;
	n->left = n->right = NULL;
	return n;
}

/* BST insert by pointer address; returns new root */
static alloc_info_t *tree_insert(alloc_info_t *root, alloc_info_t *node) {
	if (!root) return node;
	uintptr_t key = (uintptr_t)node->ptr;
	uintptr_t rkey = (uintptr_t)root->ptr;
	if (key < rkey) root->left = tree_insert(root->left, node);
	else if (key > rkey) root->right = tree_insert(root->right, node);
	else {
		/* duplicate address: update existing node's info and free the newly created node */
		root->size = node->size;
		root->file = node->file;
		root->line = node->line;
		root->func = node->func;
		free(node);
	}
	return root;
}

/* find minimum node in subtree */
static alloc_info_t *tree_min(alloc_info_t *root) {
	while (root && root->left) root = root->left;
	return root;
}

/* BST remove by pointer address; if found, removed node pointer is returned via out_removed and new root returned */
static alloc_info_t *tree_remove(alloc_info_t *root, void *ptr, alloc_info_t **out_removed) {
	if (!root) return NULL;
	uintptr_t key = (uintptr_t)ptr;
	uintptr_t rkey = (uintptr_t)root->ptr;
	if (key < rkey) {
		root->left = tree_remove(root->left, ptr, out_removed);
	} else if (key > rkey) {
		root->right = tree_remove(root->right, ptr, out_removed);
	} else {
		/* found */
		if (out_removed) *out_removed = root;
		/* three cases */
		if (!root->left) {
			alloc_info_t *r = root->right;
			return r;
		} else if (!root->right) {
			alloc_info_t *l = root->left;
			return l;
		} else {
			/* two children: replace with inorder successor */
			alloc_info_t *succ = tree_min(root->right);
			/* copy succ data into root (but keep its left subtree), then remove succ from right subtree */
			root->ptr = succ->ptr;
			root->size = succ->size;
			root->file = succ->file;
			root->line = succ->line;
			root->func = succ->func;
			/* remove succ node from right subtree (it will be freed by the recursive remove) */
			root->right = tree_remove(root->right, succ->ptr, NULL);
			/* since root itself remains the same object, out_removed should be NULL (we didn't remove this node) */
			if (out_removed) *out_removed = NULL;
			return root;
		}
	}
	return root;
}

/* find node by ptr (no locking inside) */
static alloc_info_t *tree_find(alloc_info_t *root, void *ptr) {
	while (root) {
		uintptr_t key = (uintptr_t)ptr;
		uintptr_t rkey = (uintptr_t)root->ptr;
		if (key == rkey) return root;
		root = (key < rkey) ? root->left : root->right;
	}
	return NULL;
}

/* inorder traversal to report leaks */
static void tree_report(alloc_info_t *root, size_t *total) {
	if (!root) return;
	tree_report(root->left, total);
	fprintf(stderr, " LEAK: %p size=%zu at %s:%d %s\n", root->ptr, root->size, root->file, root->line, root->func);
	*total += root->size;
	tree_report(root->right, total);
}

void *mm_malloc(size_t size, const char *file, int line, const char *func) {
	void *p = malloc(size);
	if (!p) return NULL;

	alloc_info_t *node = node_create(p, size, file, line, func);
	if (!node) {
		free(p);
		return NULL;
	}

	lock();
	alloc_root = tree_insert(alloc_root, node);
	unlock();

	return p;
}

void *mm_calloc(size_t nmemb, size_t size, const char *file, int line, const char *func) {
	void *p = calloc(nmemb, size);
	if (!p) return NULL;

	alloc_info_t *node = node_create(p, nmemb * size, file, line, func);
	if (!node) {
		free(p);
		return NULL;
	}

	lock();
	alloc_root = tree_insert(alloc_root, node);
	unlock();

	return p;
}

void *mm_realloc(void *ptr, size_t size, const char *file, int line, const char *func) {
	if (!ptr) {
		return mm_malloc(size, file, line, func);
	}

	void *newp = realloc(ptr, size);
	if (!newp) return NULL;

	lock();
	/* try to find tracked node for old ptr */
	alloc_info_t *found = tree_find(alloc_root, ptr);
	if (found) {
		if (newp == ptr) {
			/* same address, just update size/meta */
			found->size = size;
			found->file = file;
			found->line = line;
			found->func = func;
			unlock();
			return newp;
		}
		/* address changed: remove old node object and reinsert updated node with new address */
		alloc_info_t *removed = NULL;
		alloc_root = tree_remove(alloc_root, ptr, &removed);
		if (removed) {
			/* reuse removed node object for new address */
			removed->ptr = newp;
			removed->size = size;
			removed->file = file;
			removed->line = line;
			removed->func = func;
			removed->left = removed->right = NULL;
			alloc_root = tree_insert(alloc_root, removed);
			unlock();
			return newp;
		}
		/* shouldn't reach here, but fallthrough to insert new node below */
	}
	unlock();

	/* not tracked: warn and insert new tracking node for new pointer */
	fprintf(stderr, "mm_realloc: pointer %p not tracked (realloc at %s:%d %s)\n", ptr, file, line, func);
	alloc_info_t *node = node_create(newp, size, file, line, func);
	if (!node) return newp;

	lock();
	alloc_root = tree_insert(alloc_root, node);
	unlock();

	return newp;
}

void mm_free(void *ptr, const char *file, int line, const char *func) {
	if (!ptr) return;

	lock();
	alloc_info_t *removed = NULL;
	alloc_root = tree_remove(alloc_root, ptr, &removed);
	unlock();

	if (removed) {
		/* if removed non-NULL, we must free the removed tracking node and the user memory */
		free(removed);
		free(ptr);
		return;
	}

	/* not found: warn (possible double free or untracked) */
	fprintf(stderr, "mm_free: pointer %p not found (free at %s:%d %s) — possible double free or untracked allocation\n",
	        ptr, file, line, func);
	/* previous behavior avoided freeing to not risk double-free crash; keep same behavior here */
}

void mm_report_leaks(void) {
	lock();
	if (!alloc_root) {
		unlock();
		return;
	}
	fprintf(stderr, "Memory leaks detected:\n");
	size_t total = 0;
	tree_report(alloc_root, &total);
	fprintf(stderr, "Total leaked bytes: %zu\n", total);
	unlock();
}

/* register report at exit */
__attribute__((constructor))
static void mm_init(void) {
	atexit(mm_report_leaks);
}

#else /* !USE_MEMORY_TRACE - lightweight passthrough implementations */

void *mm_malloc(size_t size, const char *file, int line, const char *func) {
	(void)file; (void)line; (void)func;
	return malloc(size);
}

void *mm_calloc(size_t nmemb, size_t size, const char *file, int line, const char *func) {
	(void)file; (void)line; (void)func;
	return calloc(nmemb, size);
}

void *mm_realloc(void *ptr, size_t size, const char *file, int line, const char *func) {
	(void)file; (void)line; (void)func;
	return realloc(ptr, size);
}

void mm_free(void *ptr, const char *file, int line, const char *func) {
	(void)file; (void)line; (void)func;
	if (!ptr) return;
	free(ptr);
}

/* no-op when tracing disabled */
void mm_report_leaks(void) {
	/* intentionally empty */
}

#endif /* USE_MEMORY_TRACE */
