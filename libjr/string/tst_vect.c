#include "ezport.h"

#include <stdio.h>
#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/trace.h"

void VectorPrint (wfp, argv)
	FILE *wfp ;
	char **argv ;
{
	for (; *argv; argv++) {
		fprintf(wfp,"\"%s\"%s", *argv, argv[1]?" ":"") ;
	}
	fprintf(wfp,"\n") ;
}

void main (argc, argv)
	jr_int argc ;
	char **argv ;
{
	char		buf[512] ;
	char *		vbuf[5] ;

	char *		delimiters		= " ,";
	char *		white_space		= " ,\t\n";
	char *		quotes			= "\"";

	char		tmp_delimiters [32];
	char		tmp_white_space [32];
	char		tmp_quotes [32];


	jr_set_trace_level (jr_malloc_stats, 1);

	strcpy (tmp_delimiters, delimiters);
	jr_ControlCharsToEscapedChars (tmp_delimiters);

	strcpy (tmp_white_space, white_space);
	jr_ControlCharsToEscapedChars (tmp_white_space);

	strcpy (tmp_quotes, quotes);
	jr_ControlCharsToEscapedChars (tmp_quotes);

	fprintf (stderr, "Delimiters:     ->%s<-\n", tmp_delimiters);
	fprintf (stderr, "White space:    ->%s<-\n", tmp_white_space);
	fprintf (stderr, "Quotes:         ->%s<-\n", tmp_quotes);

	fprintf (stderr,
		"Enter up to %d arguments on a line: ", 
		sizeof (vbuf) / sizeof (char *) - 1
	) ;


	while (fgets (buf, sizeof (buf), stdin)) {

		argc = jr_VectorizeWithQuotes (
			buf, delimiters, white_space, quotes, vbuf, sizeof (vbuf) / sizeof (char *)
		);

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
			VectorPrint (stdout, vbuf) ;
			jr_VectorUndo (vbuf);
		}
		fprintf (stderr,
			"Enter up to %d arguments on a line: ", 
			sizeof (vbuf) / sizeof (char *) - 1
		) ;
	}
	jr_malloc_stats (stdout, "after jr_VectorUndo");

	exit (0);
}
