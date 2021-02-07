#include "ezport.h"

#include <stdio.h>

#include "jr/string.h"

char *	vector_unsorted[] = {
	"hello\ntesting\nthree line vector entry",
	"two	tab",
	"three\012\040there",
	"four",
	"five",
	0
} ;

void main()
{
	FILE *	wfp ;
	FILE *	rfp ;
	char **	result_vector ;

	rfp = fopen("testvio.out", "r") ;
	if (rfp == NULL) /* create file of vector declaration */ {
		wfp = fopen("testvio.out", "w") ;

		jr_VectorPrintDeclaration(wfp, vector_unsorted, "vector_unsorted") ;

		fclose(wfp) ;
	}

	/* read from file */ {
		char	variable_name[128] ;
		char	func_errbuf[512] ;

		rfp = fopen("testvio.out", "r") ;

		result_vector = jr_VectorReadDeclaration(rfp, variable_name, func_errbuf) ;
		
		if (result_vector == 0) {
			fprintf(stderr, "Error: %s", func_errbuf) ;
			exit(-1) ;
		}

		jr_VectorPrintDeclaration(stdout, result_vector, variable_name) ;
	}

	exit(0) ;
}

