#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <ctype.h>

/*
 * This routine conforms to the ANSI standard
 * description for 'strstr(string,pattern)'
 * used for finding a substring pattern in
 * a larger string. We provide the source because
 * some C environments are not provided with all
 * of the ANSI specified routines.
 */

#ifdef missing_strstr

char *strstr(string,pattern)  /* position of pattern in string */
	const char *string ;
	const char *pattern ;
{
	jr_int patlen = strlen( pattern ) ;
	jr_int match = 0 ;
	char *strptr = (char *) string ;

	if (*pattern == '\0') return((char *) string) ;
	if (*string  == '\0') return(0) ;

	while((strptr = (char *) strchr(strptr,*pattern)) != 0) {
		if(strncmp(strptr,pattern,patlen) == 0) {
			match = 1 ;
			break ;
		}
		strptr++ ;
	}
	return( match ? strptr : 0 ) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif

#ifdef missing_strcasestr
static char *strcasechr(
	const char *		string,
	int					search_char)
{
	for (; *string; string++) {
		if (tolower( *string) == tolower( search_char)) {
			return (char *) string;
		}
	}
	if (search_char == 0) {
		return (char *) string;
	}
	return 0;
}

char *strcasestr(string,pattern)  /* position of pattern in string */
	const char *string ;
	const char *pattern ;
{
	jr_int patlen = strlen( pattern ) ;
	jr_int match = 0 ;
	char *strptr = (char *) string ;

	if (*pattern == '\0') return((char *) string) ;
	if (*string  == '\0') return(0) ;

	while((strptr = (char *) strcasechr(strptr,*pattern)) != 0) {
		if(strncasecmp(strptr,pattern,patlen) == 0) {
			match = 1 ;
			break ;
		}
		strptr++ ;
	}
	return( match ? strptr : 0 ) ;
}

#endif
