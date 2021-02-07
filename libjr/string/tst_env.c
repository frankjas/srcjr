#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/string.h"

extern char **environ ;

void main(argc, argv)
	jr_int argc ;
	char **argv ;
{
	jr_int		status = 0 ;
	char **		dup_of_environ = jr_malloc(1) ;
	char *		former_value ;
	char *		str ;

	jr_set_trace(jr_malloc_stats) ;
	jr_set_trace(jr_malloc_calls) ;
	jr_set_trace(jr_malloc_trap) ;

	dup_of_environ = jr_VectorDup(environ, (void *)jr_strdup) ;

	if (argc == 1) {
		jr_VectorPrintDeclaration(stdout, dup_of_environ, "duped_environ") ;
	}

	for (argv++ ; *argv; argv++) {

		if (str = jr_getenv(dup_of_environ, *argv)) {
			fprintf(stdout,"\tThe old value of '%s' is: %s\n", *argv, str) ;
			if (argv[1]) {
				dup_of_environ = jr_setenv(dup_of_environ, *argv, argv[1], &former_value) ;
				if (str = jr_getenv(dup_of_environ, *argv)) {
					fprintf(stdout,"\tThe new value of '%s' is: %s\n", *argv, str) ;
					fprintf(stdout,"\t\tthe old value was: %s\n", former_value) ;
					status = 0 ;
				}
				else {
					fprintf(stdout,"--- DANGER NO NEW VALUE ---\n") ;
				}
				argv++ ;
			}
			else {
				status = 0 ;
			}
		}
		else {
			fprintf(stderr,
				"\t'%s' is not in the environment\n", *argv
			) ;
			status = 1 ;
		}
	}

	if (str = jr_getenv(dup_of_environ, "PATH" )) {
		fprintf(stdout,"\tThe old value of '%s' is: %s\n", "PATH", str) ;
	}
	else {
		fprintf(stdout,"no PATH variable\n") ;
	}

	dup_of_environ = jr_extendpath(dup_of_environ, "/a/clas/will") ;

	if (str = jr_getenv(dup_of_environ, "PATH" )) {
		fprintf(stdout,"\tThe new jr_getenv value of '%s' is: %s\n", "PATH", str) ;
		status = 0 ;
	}
	else {
		fprintf(stdout,"--- DANGER NO NEW VALUE ---\n") ;
		status = 1 ;
	}

	environ = dup_of_environ ;
	if (str = getenv("PATH")) {
		fprintf(stdout,"\tThe new getenv value of '%s' is: %s\n", "PATH", str) ;
		status = 0 ;
	}
	else {
		fprintf(stdout,"--- DANGER NO NEW VALUE ---\n") ;
		status = 1 ;
	}

	jr_VectorDestroy(dup_of_environ) ;

	jr_malloc_stats(stdout, "At end") ;
	jr_malloc_dump() ;

	exit(status) ;
}
