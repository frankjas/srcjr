#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/vector.h"

jr_int jr_VectorizeWithSkipSeparators(str, significant_sep, skip_sep, vector, vector_size)
	const char *	str ;
	const char *	significant_sep ;
	const char *	skip_sep ;
	char **			vector ;
	jr_int   			vector_size ;
{
	const char *	begin ;
	char *			arg ;
	jr_int  			length ;
	jr_int  			argc = 0 ;

	/*
	 * skip leading separators
	 */
	for (; *str && strchr(skip_sep, *str); str++) ;
	if (! *str) {
		vector[0] = 0;
		return(0) ;
	}

	begin = str ;
	length = 0 ;
	for ( ;; str++) {
		
		if ((! *str) || strchr(skip_sep, *str) || strchr(significant_sep, *str)) {
			
			if (argc == (vector_size - 1)) {
				/*
				 * If we have hit the last valid index (where a null should go)
				 * then stick the remaining un-vectorized string into this slot
				 * and return.
				 */
				vector[argc++] = (char *) begin ;
				return(argc) ;
			}
			else {
				/*
				 * Since we remembered the beginning of the string
				 * and have counted the length up to this position
				 * we are ready to save the bytes from the beginning
				 * of the last argument to the current position.
				 */
				arg = (char *) jr_malloc(length+1) ;
				strncpy(arg, begin, length) ;
				arg[length] = 0 ;

				vector[argc++] = arg ;
			}

			if (! *str) break ;
			for (; *str && (strchr(skip_sep, *str) || strchr(significant_sep, *str)) ; str++) {
				if (strchr(significant_sep, *str)) {
					for (str++; *str && strchr(skip_sep, *str) ; str++) ;
					break ;
				}
			}
			if (! *str) break ;

			begin = str ;
			length = 0 ;

			if (*str && strchr(significant_sep, *str)) {
				str-- ;
				continue ;
			}
		}

		/*
		 * If the char is not a separator than it is
		 * part of an argument, so increment the length
		 * of that arguemnt.
		 */
		length++ ;
	}
	vector[argc] = 0 ;
	return(argc) ;
}

const char ** jr_VectorCreateWithSkipSeparators (str, significant_sep, skip_sep)
	const char *	str ;
	const char *	significant_sep ;
	const char *	skip_sep ;
{
	char **			new_vector;		
	jr_int			max_elem			= 8;
	jr_int			num_elem;

	new_vector					= jr_malloc (max_elem * sizeof (char *));
	new_vector[max_elem - 1]	= (char *) str;
	num_elem					= 0;


	while (1) {
		
		num_elem		+= jr_VectorizeWithSkipSeparators (
							str, significant_sep, skip_sep,
							new_vector + num_elem, max_elem - num_elem
						);

		if (num_elem  ==  max_elem) {
			str			= new_vector[max_elem - 1];
			num_elem	= max_elem - 1;

			max_elem	*= 2;
			new_vector	= jr_realloc (new_vector, max_elem * sizeof (char *));
		}
		else {
			break;
		}
	}

	return (const char **) new_vector;
}

/*
#include <stdio.h>

PrintVector(wfp, argv)
	FILE *wfp ;
	char **argv ;
{
	for (; *argv; argv++) {
		fprintf(wfp,"\"%s\"%s", *argv, argv[1]?" ":"") ;
	}
	fprintf(wfp,"\n") ;
}

main (argc, argv)
	jr_int argc ;
	char **argv ;
{
	char buf[512] ;
	char *vbuf[5] ;

	fprintf (stderr,
		"Enter up to %d arguments on a line: ", 
		sizeof (vbuf) / sizeof (char *) - 1
	) ;


	while (fgets (buf, sizeof (buf), stdin)) {

		argc = jr_VectorizeWithSkipSeparators (
			buf, 
			":", 
			" \t\n", 
			vbuf, 
			sizeof (vbuf) / sizeof (char *)
		) ;

		fprintf (stdout, "%d arguments\n", argc) ;

		if (argc == sizeof (vbuf) / sizeof (char *)) {
			jr_int i ;

			fprintf (stderr,
				"Insufficient buffer space for arguments.\n"
			) ;
			for (i=0; i < argc; i++) {
				fprintf (stderr, "jr_free (argv[%d] == %s)\n", 
					i, vbuf[i]
				) ;
				jr_free (vbuf[i]) ;
			}
		}
		else {
			PrintVector(stdout,vbuf) ;
			jr_VectorUndo (vbuf);
		}
		fprintf (stderr,
			"Enter up to %d arguments on a line: ", 
			sizeof (vbuf) / sizeof (char *) - 1
		) ;
	}
	jr_malloc_stats (stdout, "after jr_VectorUndo");
}
*/
