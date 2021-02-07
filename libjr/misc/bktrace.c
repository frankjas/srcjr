#include "ezport.h"

#include "jr/misc.h"

#ifndef missing_execinfo_h

#include <execinfo.h>

jr_int jr_backtrace (pc_array, pc_array_size)
	void **			pc_array;
	jr_int			pc_array_size;
{
	jr_int			trace_size;
	jr_int			q;

	trace_size		= backtrace (pc_array, pc_array_size);

	for (q=0;  q < trace_size-1;  q++) {
		pc_array[q]	= pc_array[q+1];
	}

	return trace_size - 1;
}

#else

jr_int jr_backtrace (pc_array, pc_array_size)
	void **			pc_array;
	jr_int			pc_array_size;
{
	return 0;
}

#endif
