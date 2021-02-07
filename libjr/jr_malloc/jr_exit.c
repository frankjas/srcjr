#include "ezport.h"

#include <stdio.h>
#include <stdlib.h>

#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/trace.h"
#include "jr/apr.h"

const char *	jr_ProgramName	= 0;
/*
** 5-17-2011 Note: referencing jr_ProgramName links in the APR
*/

void jr_exit (exit_status)
	jr_int		exit_status;
{
	jr_int		status;
	char		error_buf [jr_ERROR_LEN];


	jr_apr_terminate();

	if (jr_do_trace (jr_malloc_stats)) {
		if (jr_ProgramName) {
			jr_esprintf (error_buf, "\n\n%s: After Undo of Everything", jr_ProgramName);
		}
		else {
			jr_esprintf (error_buf, "\n\nAfter Undo of Everything");
		}
		jr_malloc_stats (stderr, error_buf);
	}

	if (jr_do_trace (jr_malloc_calls)) {
		status = jr_malloc_check_leaks_and_bounds (error_buf);

		if (status != 0) {
			if (jr_ProgramName) {
				fprintf (stderr, "%s: jr_malloc() error: %s\n", jr_ProgramName, error_buf);
			}
			else {
				fprintf (stderr, "jr_malloc() error: %s\n", error_buf);
			}
			if (exit_status == 0) {
				exit_status	= 1;
			}
		}
		jr_malloc_dump ();
	}

	exit (exit_status);
}

void jr_exit_set_program_name (const char *program_name)
{
	jr_ProgramName = program_name;
}

