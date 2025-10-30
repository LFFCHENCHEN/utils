#include <stdio.h>
#include "memcheck.h"

int main(void) {
	char *p1 = MALLOC(100);
	char *p2 = CALLOC(10, 20);
	char *p3 = MALLOC(50);

	/* use memory */
	if (p1) snprintf(p1, 100, "hello memcheck");
	if (p2) p2[0] = 'A';

	/* free some, leak p3 deliberately */
	FREE(p1);
	/* double free example (will warn) */
	// FREE(p1);

	/* free p2 properly */
	FREE(p2);

	/* forget to free p3 -> will be reported at program exit */
	// (void)p3;
    FREE(p3);

	return 0;
}
