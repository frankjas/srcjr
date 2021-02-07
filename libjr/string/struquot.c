#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "jr/string.h"

void jr_StringUnQuote(str)
	char *str ;
{
	char *lastchr ;

	if (*str == '"') {
		jr_int nbytes = strlen(str+1) + 1 ;
		memmove(str, str+1, nbytes) ;
	}
	lastchr = (char *) strchr(str, 0) ;
	lastchr-- ;

	if (*lastchr == '"') {
		*lastchr = 0 ;
	}
}

/*
void main()
{
	char *	original ;
	char	result[128] ;

	original = "\"This is a test of \"quotes\" \\\"ha\\\" a litte more\"" ;
	strcpy(result, original) ;
	jr_StringUnQuote(result) ;

	fprintf(stdout,"jr_StringUnQuote(%s) == '%s'\n",
		original,
		result
	) ;
}
*/
