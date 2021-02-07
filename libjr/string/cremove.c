#include "ezport.h"

#include <string.h>

#include "jr/string.h"

void jr_RemoveAllOfChar (s, c)
	char *	s ;
	jr_int	c ;
{
	char *tp ;

	while (tp=(char *)strchr(s,c)) {
		for (;*tp;tp++) *tp = tp[1] ;
	}
}

/*
void main()
{
	char *	original = "      hello 12345 there 6" ; 
	char *	result ; 
	jr_int	char_val = 'e' ;
	
	s1	= jr_strdup(original) ;
	jr_RemoveAllOfChar(result, char_val) ;

	fprintf(stdout, "jr_RemoveAllOfChar(%s, '%c') == '%s'\n",
		original, char_val,
		result
	) ;
	jr_free(result) ;
}
*/
