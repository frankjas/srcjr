#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "jr/vector.h"
#include "jr/string.h"
#include "jr/malloc.h"

extern size_t strlen	PROTO((const char *)) ;

void	jr_VectorPrintDeclaration(wfp, string_vector, variable_name)
	FILE *				wfp ;
	const char **		string_vector ;
	const char *		variable_name ;
{
	char		padding[128] ;
	char *		escaped_version ;
	jr_int		vector_length;
	jr_int		max_length;
	jr_int		current_length ;
	jr_int		num_blanks ;
	char **		vp ;

	vector_length	= jr_VectorLength(string_vector) ;
	max_length		= jr_VectorFindMaximum(string_vector, vector_length, (jr_int (*) ()) strlen) ;
	
	/* build padding string */ {
		jr_int		i ;

		for (i=0; i < (jr_int) (sizeof(padding)-1); i++) {
			padding[i] = ' '  ;
		}
		padding[i] = 0 ;
	}

	fprintf(wfp, "char *	%s[] = {\n", variable_name) ;

	for (vp = (char **) string_vector ; *vp ; vp++) {
		current_length	= (jr_int) strlen(*vp) ;
		escaped_version = (char *) jr_malloc(2 * current_length) ;
		strcpy(escaped_version, *vp) ;
		num_blanks		= (max_length - current_length) ;
		if (num_blanks < 0) num_blanks = 0 ;

		/* change newlines and such into the backslash equivalents */
		jr_ControlCharsToEscapedChars(escaped_version) ;


		fprintf(wfp,"\t\t\"%s\",\n", escaped_version) ;

		jr_free(escaped_version) ;
	}
	fprintf(wfp, "\t\t(char *) 0\n") ;

	fprintf(wfp, "} ;\n") ;
}
