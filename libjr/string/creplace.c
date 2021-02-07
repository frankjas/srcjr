#include "ezport.h"

#include "jr/string.h"

void	jr_ReplaceAllOfChar(str,old,new)
	char *str ;
	jr_int old ;
	jr_int new ;
{
	for (;*str;str++) {
		if (*str == old) *str = new ;
	}
}

/*
void main()
{
	char *	original = "      hello 12345 there 6" ; 
	char *	result ; 
	jr_int	old_char_val = 'e' ;
	jr_int	new_char_val = 'E' ;
	
	s1	= jr_strdup(original) ;
	jr_ReplaceAllOfChar(result, old_char_val, new_char_val) ;

	fprintf(stdout, "jr_ReplaceAllOfChar(%s, '%c', '%c') == '%s'\n",
		original, old_char_val, new_char_val
		result
	) ;
	jr_free(result) ;
}
*/
