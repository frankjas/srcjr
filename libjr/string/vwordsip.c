#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/vector.h"

jr_int jr_VectorizeIntoWordsInPlace(str, sep, vector, vector_size)
	char *			str ;
	const char *	sep ;
	char **			vector ;
	jr_int   			vector_size ;
{
	char *begin;
	jr_int curr_index;


	for (begin = 0, curr_index = 0; ; str++) {
		
		if (strchr(sep, *str) || *str == 0) {
			/*
			 * saw a separator
			 * the null at the end is an implicit separator
			 */
			
			if (begin == 0) {
				if (*str) continue;
				/*
				 * if begin == 0 then we haven't started
				 * a string and are skipping separators
				 */

				if (!*str) break;
				/*
				 * if *str is null then we hit the end
				 * before begining the next string
				 */
			}
			else {

				if (curr_index == vector_size - 1) {
					/*
					 * If we have hit the last vector slot 
					 * (where a null should go)
					 * stick the remaining un-vectorized string 
					 * into this slot and return.
					 */
					vector [curr_index] = begin;
					return (curr_index + 1);
				}
				else {

					vector [curr_index] = begin;
					curr_index++;

					begin = 0;
					/*
					 * set begin to 0 to indicate we haven't 
					 * begun the string
					 */

					if (*str) {
						*str = 0;
					}
					else {
						/*
						 * ran out of input
						 */
						break;
					}
				}
			}
		}
		else if (begin == 0) {
			/*
			 * beginning the string
			 */
			begin = str;
		}
	}

	vector [curr_index] = 0 ;
	return (curr_index) ;
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

		argc = jr_VectorizeIntoWordsInPlace (
			buf, 
			" :\t", 
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
			}
		}
		else {
			PrintVector(stdout,vbuf) ;
		}
		fprintf (stderr,
			"Enter up to %d arguments on a line: ", 
			sizeof (vbuf) / sizeof (char *) - 1
		) ;
	}
}

*/
