#include "ezport.h"

#include <string.h>
#include <ctype.h>

#include "jr/misc.h"

jr_int jr_sub_strcmp(bigstr, littlestr)
	const char *	bigstr ;
	const char *	littlestr ;
{
	if (strstr(bigstr, littlestr)) {
		return(0) ;
	}
	else {
		return(-1) ;
	}
}

jr_int jr_leading_strcmp(str1, str2)
	const char *	str1 ;
	const char *	str2 ;
{
	jr_int l1 = strlen(str1) ;
	jr_int l2 = strlen(str2) ;
	jr_int n  = (l1 < l2) ? l1 : l2 ;

	return(strncmp(str1, str2, n)) ;
}


jr_int jr_strrwordcasecmp (void_arg_1, void_arg_2)
	const void *	void_arg_1;
	const void *	void_arg_2;
{
	/*
	 * Compares on the last word of the string, ignoring case.
	 *
	 * Written 3/11/2002 for Groomsmen premier list.
	 * Not well tested.
	 */
	const char *	str_1			= void_arg_1;
	const char *	str_2			= void_arg_2;

	char *			last_word_1;
	char *			last_word_2;
	
	last_word_1		= strrchr (str_1, 0);

	for (;  last_word_1 > str_1;  last_word_1--) {
		if (isspace (*last_word_1)) {
			last_word_1 ++;
			break;
		}
	}


	last_word_2		= strrchr (str_2, 0);

	for (;  last_word_2 > str_2;  last_word_2--) {
		if (isspace (*last_word_2)) {
			last_word_2 ++;
			break;
		}
	}

	return strcasecmp (last_word_1, last_word_2);
}

