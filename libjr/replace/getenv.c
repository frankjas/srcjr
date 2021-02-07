#define _POSIX_SOURCE 1


#include "ezport.h"


#ifdef replace_buggy_getenv

#include <stdlib.h>
#include <string.h>

#undef getenv

extern char **environ;

char *jr_system_getenv (search_str)
	const char *	search_str ;
{
	char **		vp ;
	char *		ptr_to_lhs ;
	char *		ptr_to_equal_sign ;
	int			search_length ;
	int			length_of_current_lhs ;

	search_length = strlen(search_str) ;

	for (vp = environ ; *vp; vp++) {
		ptr_to_lhs			= *vp ;
		ptr_to_equal_sign	= strchr(ptr_to_lhs, '=') ;
		if (ptr_to_equal_sign == 0) continue ;

		length_of_current_lhs = (ptr_to_equal_sign - ptr_to_lhs) ;
		if (length_of_current_lhs != search_length) continue ;

		if (strncmp(search_str, ptr_to_lhs, search_length) == 0) {
			return(ptr_to_equal_sign + 1) ;
		}
	}

	return(0) ;
}


#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}

#endif
