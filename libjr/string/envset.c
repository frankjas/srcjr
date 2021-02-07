#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/vector.h"
#include "jr/malloc.h"

extern char **environ ;

char **jr_setenv(old_vector, name, new_rhs_value, old_entry_holder)
	char **			old_vector ;
	const char *	name ;
	const char *	new_rhs_value ;
	char **			old_entry_holder ;
{
	jr_int   name_len		= (jr_int) strlen(name) ;
	jr_int   value_len		= (jr_int) strlen(new_rhs_value) ;
	char  *new_entry  		= jr_malloc(value_len + name_len + 2) ;
	char  *old_rhs_value	= jr_getenv(old_vector, name) ;
	jr_int   i ;


	sprintf(new_entry,"%s=%s", name, new_rhs_value) ;

	if (! old_rhs_value) {
		char **new_vector ;

		jr_int old_n     = jr_VectorLength(old_vector) ;
		jr_int old_bytes = (old_n + 1) * sizeof(char *) ;
		jr_int new_n     = old_n + 1 ;
		
		new_vector = (char **) jr_malloc((new_n + 1) * sizeof(char *)) ;
		memcpy(new_vector, old_vector, old_bytes) ;

		new_vector[new_n - 1]	= new_entry ;
		new_vector[new_n]		= 0 ;
		*old_entry_holder		= 0 ;

		return(new_vector) ;
	}
	else {
		char *current_rhs_value ;
		char *current_entry ;

		for (i = 0 ; old_vector[i] ; i++) {
			current_entry		= old_vector[i] ;
			current_rhs_value	= strchr(old_vector[i], '=') ;

			if (current_rhs_value) {
				current_rhs_value++ ;	/* skip over '=' */
			}

			if (old_rhs_value == current_rhs_value) {
				*old_entry_holder = current_entry ;
				old_vector[i] = new_entry ;
			}
		}
		return(old_vector) ;
	}
}


