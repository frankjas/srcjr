#include "ezport.h"

#include <stdio.h>

#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/string.h"

void main()
{
	char *	original ;
	char	result[128] ;

	original = "\"This is a \n \r\b\f\ttest of \"quotes\" \\\"ha\\\" a litte more\"" ;
	strcpy(result, original) ;	
	jr_ControlCharsToEscapedChars(result) ;
	jr_StringUnQuote(result) ;

	fprintf(stdout,"jr_StringUnQuote(%s) == '%s'\n",
		original,
		result
	) ;
}
