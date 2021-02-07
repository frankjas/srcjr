#include "ezport.h"

#include "jr/syscalls.h"
#include "jr/io.h"
#include "jr/error.h"
#include "jr/malloc.h"

jr_int jr_GetCanonicalPath (
	const char *				user_path,
	char *						path_buf,
	jr_int						path_buf_size,
	char *						error_buf)
{
	const char *				curr_path			= 0;
	jr_int						status;

	status	= jr_getcwd (path_buf, path_buf_size, error_buf);

	if (status != 0) {
		goto return_status;
	}

	curr_path	= jr_strdup (path_buf);

	status	= jr_chdir (user_path, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't 'cd' to '%.64s': %s", path_buf, error_buf);
		goto return_status;
	}


	status	= jr_getcwd (path_buf, path_buf_size, error_buf);

	if (status != 0) {
		goto return_status;
	}


	status	= jr_chdir (curr_path, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't 'cd' back to '%.64s': %s", curr_path, error_buf);
		goto return_status;
	}
	jr_free (curr_path);
	curr_path = 0;


	status = 0;

	return_status : {
		if (curr_path) {
			jr_chdir (curr_path, error_buf);

			jr_free (curr_path);
		}
	}
	return status;
}


const char *jr_MallocCanonicalPath (
	const char *				user_path,
	char *						error_buf)
{
	const char *				curr_path			= 0;
	const char *				canon_path			= 0;
	jr_int						status;

	curr_path	= jr_getcwd_malloc (error_buf);

	if (curr_path == 0) {
		status = jr_INTERNAL_ERROR;
		goto return_status;
	}

	status	= jr_chdir (user_path, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't 'cd' to '%.64s': %s", curr_path, error_buf);
		goto return_status;
	}


	canon_path	= jr_getcwd_malloc (error_buf);

	if (canon_path == 0) {
		status = jr_INTERNAL_ERROR;
		goto return_status;
	}


	status	= jr_chdir (curr_path, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't 'cd' back to '%.64s': %s", curr_path, error_buf);
		goto return_status;
	}
	jr_free (curr_path);
	curr_path = 0;


	status			= 0;

	return_status : {
		if (status != 0) {
			if (canon_path) {
				jr_free (canon_path);
				canon_path = 0;
			}
		}
		if (curr_path) {
			jr_chdir (curr_path, error_buf);

			jr_free (curr_path);
		}
	}
	return canon_path;
}

