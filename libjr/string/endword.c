#include "ezport.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "jr/string.h"

jr_int jr_RemoveTrailingWhiteSpace (word_arg)
	char *		word_arg;
{
	char *		end_ptr ;
	jr_int		count			= 0;
	
	if (*word_arg == 0) return count;

	end_ptr = (char *) strchr(word_arg, 0) ;

	end_ptr-- ;		/* to back off one from null */

	for (; end_ptr >= word_arg; end_ptr--, count++) {

		if (! isspace(*end_ptr)) {
			end_ptr++ ;
			*end_ptr = 0 ;
			return count;
		}
	}
	*word_arg = 0 ;

	return count;
}

/*
void main()
{
	char 	result[128] ;
	char *	original = "    first word last word       " ; 
	
	strcpy(result, original) ;
	jr_RemoveTrailingWhiteSpace(result) ;

	fprintf(stdout, "jr_RemoveTrailingWhiteSpace('%s') == '%s'\n",
		original,
		result
	) ;
}
*/
