#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_getuid

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

uid_t getuid()
{
	return(0) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
