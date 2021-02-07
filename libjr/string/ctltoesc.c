#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/string.h"

/*
 * memmove takes care of overlapping copies
 */

void jr_CopyControlCharsToEscapedChars(target, bounds_arg, source)
	char *			target ;
	jr_int			bounds_arg ;
	const char *	source ;
{
	const char *	escape_string ;
	jr_int			escape_length ;
	jr_int			target_index ;
	jr_int			source_index ;
	jr_int			bounds ;

	bounds	= bounds_arg - 1 ;
	for (	target_index = 0, source_index = 0 ;
			target_index < bounds;
			target_index += escape_length, source_index++	) {

		if (source[source_index] == 0) break ;

		escape_length = 2 ;
		switch(source[source_index]) {
			case '\a': escape_string = "\\a" ; break ;
			case '\b': escape_string = "\\b" ; break ;
			case '\f': escape_string = "\\f" ; break ;
			case '\n': escape_string = "\\n" ; break ;
			case '\r': escape_string = "\\r" ; break ;
			case '\t': escape_string = "\\t" ; break ;
			case '\v': escape_string = "\\v" ; break ;

			default  : {
				escape_string = jr_StringFromControlChar(source[source_index]) ;
				if (escape_string[1] != 0) {
					escape_length = (jr_int) strlen(escape_string) ;
				}
				else {
					escape_length = 1 ;
				}
				break ;
			}
		}
		if ((target_index + escape_length) > bounds) break ;
		strcpy(target+target_index, escape_string) ;
	}
	target[target_index] = 0 ;
}

void jr_ControlCharsToEscapedChars(str)
	char *	str ;
{
	char *	f = str ;

	for(; *f; f++) {
		switch(*f) {
			case '\a': memmove(f+2, f+1, strlen(f+1) + 1) ; *f++ = '\\' ; *f = 'a'; break ;
			case '\b': memmove(f+2, f+1, strlen(f+1) + 1) ; *f++ = '\\' ; *f = 'b'; break ;
			case '\f': memmove(f+2, f+1, strlen(f+1) + 1) ; *f++ = '\\' ; *f = 'f'; break ;
			case '\n': memmove(f+2, f+1, strlen(f+1) + 1) ; *f++ = '\\' ; *f = 'n'; break ;
			case '\r': memmove(f+2, f+1, strlen(f+1) + 1) ; *f++ = '\\' ; *f = 'r'; break ;
			case '\t': memmove(f+2, f+1, strlen(f+1) + 1) ; *f++ = '\\' ; *f = 't'; break ;
			case '\v': memmove(f+2, f+1, strlen(f+1) + 1) ; *f++ = '\\' ; *f = 'v'; break ;

			default  : {
				if (*f < ' ') {
					char buf[128] ;
					memmove(f+4, f+1, strlen(f+1) + 1) ;
					sprintf(buf, "\\%03o", *f) ;
					*f++ = buf[0] ; *f++ = buf[1]; *f++ = buf[2] ; *f = buf[3] ;
				}
				break ;
			}
		}
	}
}

/*
void main()
{
	char 	result[128] ;
	char *	original = "\f\fline 1\012hello 12345 there 6" ; 
	
	strcpy(result, original) ;
	jr_ControlCharsToEscapedChars(result) ;

	fprintf(stdout, "jr_ControlCharsToEscapedChars() == '%s'\n",
		result
	) ;

	jr_CopyControlCharsToEscapedChars(result, sizeof(result), original) ;
	fprintf(stdout, "jr_CopyControlCharsToEscapedChars(%#010x, %d, \"%s\") == '%s'\n",
		result, sizeof(result), original,
		result
	) ;
}
*/
