#define _POSIX_SOURCE 1

#include "ezport.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jr/syscalls.h"
#include "jr/error.h"
#include "jr/string.h"
#include "jr/malloc.h"

jr_int jr_MoveFileName (old_file_path, new_file_path, error_buf)
	const char *	old_file_path;
	const char *	new_file_path;
	char *			error_buf;
{
	FILE *			rfp					= 0;
	FILE *			wfp					= 0;
	jr_int			c;
	jr_int			status;


	status	= jr_MovePath (old_file_path, new_file_path, error_buf);

	if (status == 0) {
		goto return_status;
	}
	else if (errno == EXDEV) {
		/*
		** 11/26/05: the paths are on different devices, use a copy instead
		*/
	}
	else {
		jr_esprintf (error_buf, "%s", strerror (errno));
		status = jr_ConvertErrno (errno);
		goto return_status;
	}

	rfp		= fopen (old_file_path, "r");

	if (rfp == NULL) {
		jr_esprintf (error_buf, "couldn't open '%s' for reading: %s",
			old_file_path, strerror (errno)
		);
		status = jr_ConvertErrno (errno);
		goto return_status;
	}

	wfp		= fopen (new_file_path, "w");

	if (wfp == NULL) {
		jr_esprintf (error_buf, "couldn't open '%s' for writing: %s",
			new_file_path, strerror (errno)
		);
		status = jr_ConvertErrno (errno);
		goto return_status;
	}


	while ((c = getc (rfp)) != EOF) {
		putc (c, wfp);
	}

	fclose (rfp);
	fclose (wfp);
	rfp	= 0;
	wfp = 0;

	jr_Unlink ((char *) old_file_path, 0);

	status	= 0;

	return_status : {
		if (rfp) {
			fclose (rfp);
		}
		if (wfp) {
			fclose (wfp);
		}
	}
	return status;
}

jr_int jr_MovePath (
	const char *			old_path_name,
	const char *			new_path_name,
	char *					error_buf)
{
	char *					new_dest_path		= 0;
	jr_int					status;

	status	= jr_PathIsDirectory (new_path_name, error_buf);

	if (status < 0) {
		if (status != jr_NOT_FOUND_ERROR && status != jr_PERMISSION_ERROR) {
			jr_esprintf (error_buf, "couldn't check '%.64s': %s", new_path_name, error_buf);
			goto return_status;
		}
	}
	else if (status) {
		const char *	path_tail	= jr_PathTail (old_path_name);
		jr_int			new_length	= strlen (new_path_name);

		new_dest_path	= jr_malloc (new_length + 1 + strlen (path_tail) + 1);

		if (new_path_name[new_length - 1] == '/') {
			sprintf (new_dest_path, "%s%s", new_path_name, path_tail);
		}
		else {
			sprintf (new_dest_path, "%s/%s", new_path_name, path_tail);
		}

		new_path_name	= new_dest_path;
	}

	status	= rename (old_path_name, new_path_name);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		status = jr_ConvertErrno (errno);
		goto return_status;
		/*
		** 4/17/07: Needs to set errno, is used by jr_MoveFileName()
		*/
	}

	status	= 0;

	return_status : {
		if (new_dest_path) {
			jr_free (new_dest_path);
		}
	}
	return status;
}
