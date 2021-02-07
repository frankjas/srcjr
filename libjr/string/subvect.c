#include "ezport.h"

#include <string.h>
#include "jr/malloc.h"
#include "jr/vector.h"
#include "jr/string.h"
#include "jr/trace.h"

char *	jr_NewStringFromDollarVectorSubstitutions(old_string_arg, env_style_vector)
	const char *	old_string_arg ;
	char **			env_style_vector ;
{
	char	dollar_var_name[512] ;
	char 	plain_var_name[512] ;
	char *	rhs_dollar_value_string ;

	char *	new_string ;
	char *	old_string ;
	jr_int	vlen ;
	jr_int	i ;
	
	vlen = jr_VectorLength(env_style_vector) ;

	new_string = old_string = jr_strdup(old_string_arg) ;
	for (i=0; i < vlen; i++) {
		sscanf(env_style_vector[i], "%[^=]", plain_var_name) ;
		sprintf(dollar_var_name, "$%s", plain_var_name) ;

		rhs_dollar_value_string = (char *) strchr(env_style_vector[i], '=') ;
		if (! rhs_dollar_value_string) {
			rhs_dollar_value_string = "" ;
		}
		else {
			rhs_dollar_value_string++ ;		/* skip over the '=' to the text of the rhs */
		}


		if (jr_is_trace_at_level(jr_io,1)) {
			fprintf(stderr, "vector[%d] == '%s'\n", i, env_style_vector[i]) ;
			fprintf(stderr, "\t sub '%s' with '%s'\n", dollar_var_name, rhs_dollar_value_string) ;
		}

		new_string =	jr_ReplaceSubByteMatches(
							old_string, (jr_int) strlen(old_string),
							dollar_var_name, (jr_int) strlen(dollar_var_name),
							rhs_dollar_value_string, (jr_int) strlen(rhs_dollar_value_string)
						) ;

		jr_free(old_string) ;
		old_string = new_string ;
	}

	return(new_string) ;
}
