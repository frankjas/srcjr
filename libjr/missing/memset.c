#define _POSIX_SOURCE 1
#include "ezport.h"
#include <string.h>

/*
 * This routine conforms to the ANSI standard
 * description for 'memset(dest,value,nbytes)'
 * used for setting arbitrary byte streams to
 * a single value (usually zero).
 * We provide the source because
 * some C environments are not provided with all
 * of the ANSI specified routines.
 */

#ifdef missing_memset

void *memset(destin_arg, value, nbytes)
	void *destin_arg ;
	int   value ;
	unsigned long nbytes ;
{
	char *destin = (char *) destin_arg ;
	unsigned jr_int i ;

	for (i=0; i < nbytes; i++) {
		destin[i] = value ;
	}
	return(destin_arg) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
