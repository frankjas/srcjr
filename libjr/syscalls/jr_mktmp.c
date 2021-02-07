#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <stdlib.h>

#include "jr/misc.h"
#include "jr/long.h"
#include "jr/error.h"
#include "jr/malloc.h"
#include "jr/syscalls.h"
#include "jr/time.h"


void jr_MkTemp (tmp_name_template)
	char *			tmp_name_template;
{
	static int		SeedWasSet			 = 0;
	jr_int			random_int;
	jr_ULong		random_ulong;
	char			x_buffer[32];
	char *			cptr;

	/*
	 * 3/11/2004: Used to use mktemp(), but that now generates
	 * warning message whenever it is linked in.
	 * 
	 * The following procedure should generate a random number
	 * where the top 16 bits are process id, lower 14 are random
	 * which should fit in 6 base 32 digits (32^6 == 2^30)
	 */

	if (! SeedWasSet) {
		srand (jr_time_seconds ());
		SeedWasSet	= 1;
	}

	random_int		= rand ();

	if (random_int < 0) {
		random_int	= -random_int;
	}

	random_int		= (16.0 * 1024 * random_int / (RAND_MAX + 1.0));
	/*
	 * 3/11/2004: random_int == a number from 0-2^14
	 */

	jr_ULongAsgnUInt	(&random_ulong, jr_getpid ());
	jr_ULongShiftInt	(&random_ulong, random_ulong, -16);
	jr_ULongAddUInt		(&random_ulong, random_ulong, random_int);

	jr_ULongToBase32String (random_ulong, x_buffer, 1 /* use_lower_case */);

	cptr		= strstr (tmp_name_template, "XXXXXX");

	if (cptr) {
		/*
		** 9-6-2012: pad x_buffer by 0 on the left, a straight strncpy()
		** in this case puts a null in position 5, truncating the string.
		*/
		jr_int		pad_length	= 6 - (jr_int) strlen( x_buffer);
		jr_int		z;

		if (pad_length < 0) {
			pad_length	= 0;
		}

		for (z=0; z < pad_length; z++, cptr++) {
			*cptr	= '0';
		}
		strncpy( cptr, x_buffer, 6 - pad_length);
	}
}


const char *jr_MkTempInTempDir (
	const char *	tmp_name_template)
{
	char			tmp_dir[64];
	char *			template_buf;
	char 			error_buf [jr_ERROR_LEN];
	jr_int			status;

	status		= jr_GetTempDir (tmp_dir, sizeof (tmp_dir), error_buf);

	if (status != 0) {
		sprintf (tmp_dir, ".");
	}

	template_buf	= jr_malloc (strlen (tmp_dir) + 1 + strlen (tmp_name_template) + 1);
	sprintf (template_buf, "%s/%s", tmp_dir, tmp_name_template);

	jr_MkTemp (template_buf);

	return template_buf;
}

