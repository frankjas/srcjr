#include "exn_proj.h"

jr_int jr_ExceptionBreakPoint (status)
	jr_int		status;
{
	/*
	 * This function servers only as a place to
	 * set a break point in the debugger.
	 * This function is called every time an exception
	 * is raised.
	 * This function should be compiled with the debugger
	 * option on
	 *
	 * If this function returns non-zero the exception
	 * won't be thrown.
	 */
	if (jr_do_trace (jr_exceptions)) {
		jr_coredump ();
	}
	return (status);
}
