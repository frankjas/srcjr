#define _POSIX_SOURCE 1

#include "ezport.h"

/*
 * 3/31/2005: This is untested code. It doesn't work on ppc
 */

#if defined (missing_backtrace)  &&  !defined (cpu_ppc)

/*
 * 3/31/2005: the power pc has a non-standard stack. It grows larger
 * and doesn't contain the return address.
 */

int backtrace (pc_array, pc_array_size)
	void **			pc_array;
	int				pc_array_size;
{
	void *			dummy_array[1];			/* 3/31/2005: must be 1st local var */
	void **			curr_frame_ptr;
	void *			curr_pc;
	jr_int			trace_size		= 0;
	jr_int			q;

	/*
	 * 3/31/2005: A deliberate array overflow,
	 *
	 * dummy_array[1] == stack pointer
	 * dummy_array[2] == return address
	 *
	 * ==>   &dummy_array[1] is the beginning of a 2 element void * array.
	 */

	curr_frame_ptr	= &dummy_array[1];

	for (q=0;  q < pc_array_size;  q++) {

		curr_pc			= curr_frame_ptr[1];

		if (curr_pc == 0) {
			break;
		}
		pc_array[q]		= curr_pc;

		curr_frame_ptr	= curr_frame_ptr[0];
	}
	return trace_size;
}
#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
#endif
