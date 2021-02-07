#define _POSIX_SOURCE 1

#include "ezport.h"


#ifdef missing_putenv


/* putenv should be declared extern in unistd.h */
#include <string.h>
#include <unistd.h>

int putenv(name_value_pair_str)
	char *	name_value_pair_str;
{
	jr_int				name_length ;
	char *				temp_char_ptr ;
	char **				new_environ_ptr ;
	extern char **		environ ;
	jr_int				old_vector_length;
	jr_int				new_vector_length ;
	jr_int				i ;

	/*  First see if there is an existing 'name=value' with the
	 *  same name as s.
	 */
	for (temp_char_ptr = name_value_pair_str;
			(*temp_char_ptr != '=') && (*temp_char_ptr != '\0'); temp_char_ptr++) {
		/* skip up to equals sign or null */ ;
	}

	if (*temp_char_ptr == '=') {
		name_length = (temp_char_ptr - name_value_pair_str) + 1;

		for (i = 0; environ[i] != 0; i++) {
			if (strncmp(environ[i], name_value_pair_str, name_length) == 0) {
				environ[i] = name_value_pair_str ;
				return(0) ;
			}
		}
		old_vector_length = i;
	}
	else {
		for (i = 0; environ[i] != 0; i++);
		old_vector_length = i;
	}
	
	new_environ_ptr = (char **) malloc((old_vector_length + 2) * sizeof(char *));
	if (new_environ_ptr == NULL) {
		return(-1) ;
	}

	for (i = 0; i < old_vector_length; i++) {
		new_environ_ptr[i] = environ[i] ;
	}

	new_vector_length					= old_vector_length + 1 ;
	new_environ_ptr[old_vector_length]	= name_value_pair_str ;
	new_environ_ptr[new_vector_length]	= 0 ;

	environ = new_environ_ptr ;

	return(0);
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
