#include "ezport.h"

#include <string.h>

#include "jr/string.h"

jr_int jr_SubStringIndex(search_string, pattern)
	const char *	search_string ;
	const char *	pattern ;
{
	char *	substring_location ;
	
	substring_location = strstr((char *) search_string, (char *) pattern) ;

	if (substring_location == 0) {
		return(-1) ;
	}
	else {
		return(substring_location - search_string) ;
	}
}
