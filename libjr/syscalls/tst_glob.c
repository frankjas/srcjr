#include <stdio.h>

#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/string.h"

#include "jr/string.h"

char *	ProgramName = "testglob" ;

void main()
{
	char		lbuf[128] ;
	char **		new_vector ;

	jr_set_trace(jr_malloc_stats) ;
	/*
	jr_set_trace(jr_malloc_trap) ;
	jr_set_trace(jr_malloc_calls) ;
	*/

	for (
		fprintf(stderr, "Enter glob names with $VAR and ~user references: ") ;
		fgets(lbuf, sizeof(lbuf), stdin) ;
		fprintf(stderr, "Enter glob names with $VAR and ~user references: ") 
	) {
		jr_RemoveTrailingWhiteSpace(lbuf) ;
		new_vector = jr_NewVectorFromCshLikeNameExpansion(lbuf) ;

		if (! new_vector) {
			char errbuf[128] ;

			sprintf(errbuf, "%s : couldn't csh-like expand '%s' ",
				ProgramName,
				lbuf
			) ;
			perror(errbuf) ;
		}

		if (new_vector != 0) {
			char **		vp ;

			fprintf(stdout, "%s\n", lbuf) ;
			for (vp = new_vector ; *vp ; vp++) {
				fprintf(stdout, "\t%s\n", *vp) ;
			}
			jr_VectorDestroy(new_vector) ;
		}

	}

	if (jr_do_trace(jr_malloc_stats)) {
		jr_malloc_stats(stdout, "After all freed supposedly") ;
	}
	if (jr_do_trace(jr_malloc_calls)) {
		jr_malloc_dump() ;
	}
	exit(0) ;
}
