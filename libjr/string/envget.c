#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/string.h"
#include "jr/malloc.h"

extern char **environ ;

jr_int jr_getenv_index(vector, name) 
	char **			vector ;
	const char  *	name ;
{
	char **			vp ;
	char  *			rhs_ptr ;
	jr_int   			diff ;
	jr_int			i ;

	i = 0 ;
	for (vp = vector ; *vp; vp++, i++) {
		rhs_ptr = strchr(*vp, '=') ;
		if (rhs_ptr) rhs_ptr++ ;

		/* special style of strcmp which stops on null or '=' */ {
			const char *	s1 = *vp ;
			const char *	s2 = name ;

			for (; *s1 == *s2; s1++, s2++) {
				if (*s2 == 0) break ;
				if (*s1 == '=') break ;
			}
			if ((*s2 == 0) && (*s1 == '='))		diff = 0 ;
			else								diff = *s1 - *s2 ;
		}

		if (diff == 0) {
			return(i) ;
		}
	}

	return(-1) ;
}
char *jr_getenv(vector, name) 
	char **			vector ;
	const char  *	name ;
{
	char **			vp ;
	char  *			rhs_ptr ;
	jr_int   		diff ;

	for (vp = vector ; *vp; vp++) {
		rhs_ptr = strchr(*vp, '=') ;
		if (rhs_ptr) rhs_ptr++ ;

		/* special style of strcmp which stops on null or '=' */ {
			const char *	s1 = *vp ;
			const char *	s2 = name ;

			for (; *s1 == *s2; s1++, s2++) {
				if (*s2 == 0) break ;
				if (*s1 == '=') break ;
			}
			if ((*s2 == 0) && (*s1 == '='))		diff = 0 ;
			else								diff = *s1 - *s2 ;
		}

		if (diff == 0) {
			return(rhs_ptr) ;
		}
	}

	return(0) ;
}

/* test program is testenv.c */
