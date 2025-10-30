#ifndef MEMCHECK_H
#define MEMCHECK_H

#include <stdlib.h>

/* Use these macros to capture file/line/func */
#define MALLOC(sz) mm_malloc((sz), __FILE__, __LINE__, __func__)
#define CALLOC(nm, sz) mm_calloc((nm), (sz), __FILE__, __LINE__, __func__)
#define REALLOC(ptr, sz) mm_realloc((ptr), (sz), __FILE__, __LINE__, __func__)
#define FREE(p) mm_free((p), __FILE__, __LINE__, __func__)

/* API */
void *mm_malloc(size_t size, const char *file, int line, const char *func);
void *mm_calloc(size_t nmemb, size_t size, const char *file, int line, const char *func);
void *mm_realloc(void *ptr, size_t size, const char *file, int line, const char *func);
void mm_free(void *ptr, const char *file, int line, const char *func);
void mm_report_leaks(void);

#endif /* MEMCHECK_H */
