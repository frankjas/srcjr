#include "ezport.h"

#include <stdio.h>

#include "jr/errsim.h"
#include "jr/misc.h"

jr_int jr_iferr_sim (
	jr_int					is_err,
	jr_int					is_orig_err,
	jr_int					negate_err,
	const char *			file_name,
	jr_int					line_number)
{
	return is_err;
}
	


#if 0

/*
 * 7/26/2005: Sample code with iferr()
 * Check output with:  gcc -I$I -E t.c >& t.out
 * Undef one or both of jr_IFERR_USE to trigger different expansions.
 */

#define jr_IFERR_ID	err_base

#define jr_IFERR_USE_CACHED	1
#define jr_IFERR_USE_CUSTOM	1


#include "ezport.h"
#include "jr/error.h"

void main ()
{
	jr_int		status		= __LINE__;

	status	= myfunc ();

	iferr (status != 0) {
		fprintf ("Failed");
	}
}

#endif


/*
 * 7/28/2005: Not used from here down?
 */

char jr_HasFakeError[jr_NUM_FAKE_ERRORS];


void jr_check_fake_error (error_number, file_name, line_number)
	jr_int					error_number;
	const char *			file_name;
	jr_int					line_number;
{
	if (error_number < 0  ||  error_number >= jr_NUM_FAKE_ERRORS) {
		if (file_name) {
			fprintf (stderr, "\n%s:%d: ", file_name, line_number);
		}
		else {
			fprintf (stderr, "\n");
		}

		fprintf (stderr, "jr_check_fake_error (%d): error number %d too large, max. is %d\n",
			error_number, error_number, jr_NUM_FAKE_ERRORS
		);
		jr_coredump ();
	}

	if (jr_use_fake_error (error_number)) {
		if (file_name) {
			fprintf (stderr, "\n%s:%d: ", file_name, line_number);
		}
		else {
			fprintf (stderr, "\n");
		}

		fprintf (stderr, "jr_check_fake_error (%d): error number %d already set\n",
			error_number, error_number
		);
		jr_coredump ();
	}
}

