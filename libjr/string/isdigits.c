#include "ezport.h"

#include <stdio.h>
#include <ctype.h>

#include "jr/string.h"

jr_int jr_StringIsAllDigits(str)
	const char *str ;
{
	jr_int retval = 0 ;

	for (; *str; str++) {
		if (! isdigit(*str)) return(0) ;
		else retval++ ;
	}
	return(retval) ;
}

/*
void main()
{
	char 	string[128] ;
	jr_int	value ;
	
	strcpy(string, "1234t") ;
	value = jr_StringIsAllDigits(string) ;

	fprintf(stdout, "jr_StringIsAllDigits('%s') == %d\n",
		string,
		value
	) ;

	strcpy(string, "234") ;
	value = jr_StringIsAllDigits(string) ;

	fprintf(stdout, "jr_StringIsAllDigits('%s') == %d\n",
		string,
		value
	) ;

}
*/
