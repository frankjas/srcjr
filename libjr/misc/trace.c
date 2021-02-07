#define is_libjr_build

#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/misc.h"

void jr_check_trace(trace_num, file_name, line_number)
	jr_int			trace_num;
	const char *	file_name ;
	jr_int			line_number;
{
	/*
	** 3/2/07: Can't do a malloc(), since called from inside jr_MallocDiag() code
	*/
	if (trace_num >= jr_malloc_calls_TRACE  &&  trace_num <= jr_malloc_stats_TRACE) {
		if (jr_malloc_num_unrecorded_calls () > 0) {
			fprintf(stderr,
				"\n==== %s:%d: jr_set_trace(jr_malloc...): jr_malloc() has already been called ====\n",
				file_name, line_number
			) ;
			fprintf(stderr,
				"==== %s:%d: jr_set_trace(jr_malloc...): expect jr_malloc errors and/or abort() ====\n",
				file_name, line_number
			) ;
			/*
			 * can't coredump, because in some apps the traces are set via a UI
			 * which may not be set before jr_mallocs have been done
			 */
		}
	}
}

jr_int jr_trace_bit (jr_int	trace_value)
{
	jr_int		q;

	for (q=0;  trace_value > 0;  q++) {
		trace_value >>= 1;
	}

	return q;
}
