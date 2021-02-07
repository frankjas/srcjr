#define _POSIX_SOURCE 1

#include "ezport.h"


#ifdef missing_setvbuf_objects

#include <stdio.h>
#include <errno.h>

#ifndef 	_IOFBF
#	define	_IOFBF	0
#endif

extern void setlinebuf	PROTO ((FILE *stream));
extern void setbuffer	PROTO ((FILE *stream, char *buf, int size));

int setvbuf (stream, buf, mode, size)
	FILE *			stream;
	char *			buf;
	int				mode;
	size_t			size;
{
	if (mode & _IOFBF) {
		/*
		 * full buffering.
		 */
		errno = EINVAL;
		return -1;
	}
	if (mode & _IOLBF) {
		setlinebuf (stream);
	}
	if (mode & _IONBF) {
		/*
		 * 0 buffer turns on unbuffered input/output
		 */
		setbuffer (stream, 0, 0);
	}
	if (buf) {
		setbuffer (stream, buf, size);
	}
	return 0;
}


#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
