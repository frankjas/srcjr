#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>

/* This function is identical to 'index(str,c)' */

#ifdef missing_strchr

char *strchr(str, chr)
	const char *str ;
	int chr ;
{
	for (; *str; str++) {
		if (*str == chr) return((char *) str) ;
	}

	if (chr == 0) return((char *) str) ;

	return(0) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
