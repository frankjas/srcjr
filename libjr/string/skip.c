#include "ezport.h"

#include <stdio.h>
#include <ctype.h>

#include "jr/string.h"

char *jr_FirstNonWhiteSpacePtr(s)
	const char *s ;
{
	while ((*s) && (isspace(*s))) s++ ;
	return((char*) s) ;
}

jr_int jr_FirstNonWhiteSpaceIndex(s)
	const char *s ;
{
	jr_int n ;

	if (! *s) return(-1) ;

	for (n=0;*s;n++,s++) {
		if (! isspace(*s)) return(n) ;
	}
	return(n) ;
}

char *	jr_FirstWhiteSpacePtr(s)
	const char *s ;
{
	for(;*s;s++) {
		if (isspace(*s)) return((char *)s) ;
	}
	return(0) ;
}

jr_int jr_FirstWhiteSpaceIndex(s)
	const char *s ;
{
	jr_int n ;

	for(n=0;*s;n++,s++) {
		if (isspace(*s)) return(n) ;
	}
	return(-1) ;
}

char *jr_NextNonWhiteSpacePtr (const char *s)
{
	char *			next_ptr;
	/*
	** 1/24/07: Skip over non-white space followed by white-space
	*/

	next_ptr	= jr_FirstWhiteSpacePtr (s);

	if (next_ptr) {
		next_ptr	= jr_FirstNonWhiteSpacePtr (next_ptr);
	}

	return next_ptr;
}
