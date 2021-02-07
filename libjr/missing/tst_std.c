#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * a test program that tests the linking
 * of commonly "missing" ANSI and/or POSIX functions
 */

void main ()
{
	char buf1[128] ;
	char buf2[128] ;
	char *	source ;
	char *	target ;
	char *	ptr ;

	memset(buf1, 0, sizeof(buf1)) ;

	source = buf1 ;
	target = buf2 ;

	memcpy(target, source, sizeof(buf2)) ;

	source = buf1 ;
	target = buf2 ;

	memmove(target, source, sizeof(buf2)) ;

	strcpy(buf1, "hello") ;
	ptr = strchr(buf1, 'l') ;
	
	ptr = strdup("hello") ;
	ptr = strrchr(ptr, 'l') ;

	ptr = strstr("hello there", "the") ;
}
