#include "ezport.h"

#include <string.h>

#include "jr/misc.h"
#include "jr/malloc.h"

char *  jr_strdup(str)
	const char *  str ;
{
	char *  result = jr_malloc (strlen(str) + 1) ;

	strcpy(result, str) ;

	return(result) ;
}

char *  jr_strndup(str, max_length)
	const char *	str ;
	size_t			max_length;
{
	size_t			str_length	= strlen (str);
	char *			result;
	
	if (max_length  <  str_length) {

		result = jr_malloc (max_length + 1) ;
		strncpy (result, str, max_length);
		result[max_length] = 0;
	}
	else {
		result = jr_malloc (str_length + 1) ;
		strcpy(result, str) ;
	}

	return(result) ;
}
