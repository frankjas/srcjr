#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>

/* This function is identical to 'rindex(str,c)' */

#ifdef missing_strrchr

char *strrchr(str, chr)
	const char *str ;
	int chr ;
{
	jr_int   i = strlen(str) ;
	char *ptr = (char *) str + i ;

	for (; ptr >= str ; ptr--) {
		if (*ptr == chr) return(ptr) ;
	}
	return(0) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
