#include "ezport.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "jr/string.h"

/*****************************************************

	This module provides routines for manipulating
	escaped characters in strings. Escaped characters
	are those which are preceded with a backslash.

	The recognized jr_EscapedCharsToControlChars are:

		\a = ???
		\b = backspace
		\f = formfeed
		\n = newline
		\t = tab
		\v = vertical tab
		\\ = a single backslash
		\" an embedded double quote
		\' an embedded single quote

	a backslash before any other character is interpreted
	as just being that character.

	jr_EscapedCharsToControlChars(s)
		char *s ;    translates the characters in 's' to be their
					 escaped equivalents
	jr_EndsWithOddBackslashes(s)
		char *s ;    determines if the string argument ends in
                     an odd number of backslashes. This is useful
					 in the context of the scanner, where it is
					 necessary to know whether to continue reading
					 a string constant (in the case where an odd
					 number of back slashes occur before a double
					 quote)

*************************************************************/

void	jr_EscapedCharsToControlChars (s)
	char *s ;
{
	char *t=s, *f=s ;

	for(; *f; f++, t++) {
		if (*f == '\\') {
			f++;
			switch(*f) {
				case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8':
				case '9': {
					jr_int hexadecimal;

					sscanf (f, "%x", &hexadecimal);
					*t = hexadecimal;

					for (; *f; f++) {
						if (! isdigit(*f)) {
							f-- ;	/* accomodate for the outer loop increment */
							break ;
						}
					}

					continue;
				}
				case '0': {
					jr_int octal;

					sscanf (f, "%o", &octal);
					*t = octal;

					for (; *f; f++) {
						if (! isdigit(*f)) {
							f-- ;	/* accomodate for the outer loop increment */
							break ;
						}
					}

					continue;
				}
				case 'a': *t = '\a' ; continue ;
				case 'b': *t = '\b' ; continue ;
				case 'f': *t = '\f' ; continue ;
				case 'n': *t = '\n' ; continue ;
				case 'r': *t = '\r' ; continue ;
				case 't': *t = '\t' ; continue ;
				case 'v': *t = '\v' ; continue ;
				default:  *t = *f   ; continue ;
			}
		}
		else *t = *f ;
	}
	*t = '\0' ;
}

jr_int jr_EndsWithOddBackslashes(s)
	const char *s ;
{
	const char *	tp ;
	jr_int			len = (jr_int) strlen(s) ;
	jr_int			occur = 0 ;

	for (tp=(s+len); len; len--) {
		if (*(--tp) == '\\') occur++ ;
		else break ;
	}
	return(occur % 2) ;
}

/*
void main()
{
	char 	result[128] ;
	char *	original = "\\f\\fline 1\\012\\00002hello 12345 there 6" ; 
	
	strcpy(result, original) ;
	jr_EscapedCharsToControlChars(result) ;

	fprintf(stdout, "jr_EscapedCharsToControlChars('%s') == %s\n",
		original,
		result
	) ;
}
*/
