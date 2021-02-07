#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>

#include "jr/trace.h"
#include "jr/malloc.h"

#include "jr/io.h"

char *	ProgramName = "testpw" ;

void main()
{
	jr_set_trace(jr_malloc_stats) ;

	/* simulated terminal input */ {
		char *	simulated_input_string = "test\n" ;
		char 	input_buffer[128] ;


		fprintf(stderr, "Reading simulated input:\n") ;

		jr_SimulateTerminalInputFromString(simulated_input_string) ;

		fgets(input_buffer, sizeof(input_buffer), stdin) ;
		if (strcmp(input_buffer, simulated_input_string) != 0) {
			fprintf(stderr, "input wasn't simulated correctly!\n") ;
		}
		else {
			fprintf(stdout, "simulated correctly: %s\n", input_buffer) ;
		}
	}

	/* file size */ {
		char *		rfn = "Makefile" ;
		size_t		fsize ;

		fsize = jr_FileNameDiskSize(rfn) ;
		fprintf(stdout, "DiskSize of '%s' == %ld\n", rfn, fsize) ;
	}

	/* file modification date */ {
		char *		rfn = "Makefile" ;
		time_t		fmodtime ;

		fmodtime = jr_FileNameModifiedTime(rfn) ;
		fprintf(stdout, "Modified time of '%s' == %ld %s\n",	
			rfn, fmodtime, ctime(&fmodtime)
		) ;
	}

	if (jr_do_trace(jr_malloc_stats)) {
		jr_malloc_stats(stdout, "After free") ;
	}

	exit(0) ;
}

