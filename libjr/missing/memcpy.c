#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>

/*
 * This routine conforms to the ANSI standard
 * description for 'memcpy(dest,src,nbytes)'
 * used for copying arbitrary byte streams from
 * one location in memory to another even if those
 * locations overlap.  We provide the source because
 * some C environments are not provided with all
 * of the ANSI specified routines.
 */

#ifdef missing_memcpy

void *memcpy(destin_arg, source_arg, nbytes)
	void *destin_arg ;
	void *source_arg ;
	unsigned long nbytes ;
{
	char *destin = (char *) destin_arg ;
	char *source = (char *) source_arg ;
	jr_int   i ;

	if (source_arg == destin_arg) return(destin_arg) ;

	if ((destin < source) && ((destin + nbytes) > source)) {
		/* top of dest dest overlaps bottom of source */

		for (i=0; i < nbytes ; i++) {
			destin[i] = source[i] ;
		}
	}
	else if ((destin > source) && (destin < (source + nbytes))) {
		/* bottom of dest overlaps top of source */

		for (i = nbytes - 1 ; i >= 0 ; i--) {
			destin[i] = source[i] ;
		}
	}
	else /* no overlap */ {

		for (i=0; i < nbytes ; i++) {
			destin[i] = source[i] ;
		}
	}	
		
	return(destin_arg) ;
}
#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
#endif
