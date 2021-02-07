#include "ezport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jr/string.h"
#include "jr/vector.h"
#include "jr/malloc.h"

char **jr_extendpath(environ_arg, pathname)
	char **			environ_arg ;
	const char *	pathname ;
{
	const char *	new_dir  = pathname ;
	char *			old_path = (char *) getenv("PATH") ;
	char *			former_entry ;
	
	if (! old_path) {
		environ_arg = jr_setenv(environ_arg, "PATH", pathname, &former_entry) ;
		if (former_entry) jr_free(former_entry) ;
	}
	else {
		char new_path[2048] ;
		char do_colon[4] ;
		jr_int pos ;

		if ((pos = jr_SubStringIndex(old_path, pathname)) >= 0) {
			char *entry = old_path + pos ;
			jr_int  len   = (jr_int) strlen(pathname) ;

			if ((entry[len] == 0) || (entry[len] == ':')) {
				return(environ_arg) ;
			}
		}
		do_colon[0] = (*old_path == ':') ? 0 : ':' ;
		do_colon[1] = 0 ;

		sprintf(new_path, "%s%s%s", new_dir, do_colon, old_path) ;

		environ_arg = jr_setenv(environ_arg, "PATH", new_path, &former_entry) ;
		if (former_entry) free(former_entry) ;
	}
	return(environ_arg) ;
}

/* test program is testenv.c */
