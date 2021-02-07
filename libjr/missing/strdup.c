#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdlib.h>
#include <string.h>

#ifdef missing_strdup

char *  strdup(str)
	const char *  str ;
{
	char *  result = malloc(strlen(str) + 1) ;

	strcpy(result, str) ;

	return(result) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
