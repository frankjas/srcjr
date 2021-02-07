#define _POSIX_SOURCE 1

#include "ezport.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jr/syscalls.h"
#include "jr/error.h"
#include "jr/malloc.h"

jr_int jr_RemovePath (
	const char *			path_name,
	char *					error_buf)
{
	jr_int					is_dir;
	char *					cmd_buf			= 0;
	jr_int					status;


	status	= jr_PathIsDirectory (path_name, error_buf);

	if (status == jr_NOT_FOUND_ERROR) {
		status	= 0;
		goto return_status;
	}
	else if (status < 0) {
		jr_esprintf (error_buf, "couldn't check path '%.32s': %s", path_name, error_buf);
		goto return_status;
	}
	is_dir	= status;

	if (is_dir) {
		cmd_buf		= jr_malloc (strlen (path_name) + 20);

		sprintf (cmd_buf, "rm -rf '%s'", path_name);

		status		= jr_exec_cmd (cmd_buf, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}
	else {
		status		= jr_Unlink (path_name, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	status	= 0;

	return_status : {
		if (cmd_buf) {
			jr_free (cmd_buf);
		}
	}
	return status;
}
