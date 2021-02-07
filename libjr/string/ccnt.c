#include "ezport.h"

#include "jr/string.h"

jr_int jr_CountAllOfChar (search_string, char_val_counted)
	const char *		search_string ;
	jr_int				char_val_counted;
{
	jr_int retval=0 ;

	for (;*search_string; search_string++) {
		if (*search_string == char_val_counted) retval++ ;
	}
	return(retval) ;
}

