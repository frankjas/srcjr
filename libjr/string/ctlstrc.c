#include "ezport.h"

#include <string.h>

#include "jr/string.h"

const char *  jr_EscapedStringFromControlChar(c)
	jr_int c ;
{
	static char	buf[16] ;
	char *		escape_string ;

	escape_string = buf ;
	switch(c) {
		case '\a': escape_string = "\\a" ; break ;
		case '\b': escape_string = "\\b" ; break ;
		case '\f': escape_string = "\\f" ; break ;
		case '\n': escape_string = "\\n" ; break ;
		case '\r': escape_string = "\\r" ; break ;
		case '\t': escape_string = "\\t" ; break ;
		case '\v': escape_string = "\\v" ; break ;

		default  : {
			if ((c < 32) || (c >= 127)) {
				sprintf(buf, "\\%#04o", c) ;
			}
			else {
				buf[0] = c ;
				buf[1] = 0 ;
			}
			break ;
		}
	}

	return escape_string ;
}

const char *  jr_StringFromControlChar(c)
	jr_int c ;
{
	static char buf[16] ;

	if (c < 32) {
		sprintf(buf, "^%c", c + 'A' - 1) ;
	}
	else if (c == 127) {
		sprintf(buf, "^?") ;
	}
	else if (c > 127) {
		sprintf(buf, "\\%#04o", c) ;
	}
	else {
		sprintf(buf, "%c", c) ;
	}
	return buf ;
}

/*
void main()
{
	char *	result ; 
	jr_int	char_val = '\f' ;
	
	result = jr_StringFromControlChar(char_val) ;

	fprintf(stdout, "jr_StringFromControlChar('\%#03o') == '%s'\n",
		char_val,
		result
	) ;
}
*/
