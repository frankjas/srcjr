#include "ezport.h"

#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "jr/syscalls.h"
#include "jr/malloc.h"
#include "jr/error.h"

jr_int jr_getcwd (
	char *						path_buf,
	jr_int						path_buf_size,
	char *						error_buf)
{
	jr_int						status;


	if (getcwd (path_buf, path_buf_size) == 0) {

		if (errno == ERANGE) {
			status	= jr_NO_ROOM_ERROR;
			jr_esprintf (error_buf, "buffer too small");
		}
		else {
			status	= jr_ConvertErrno (errno);
			jr_esprintf (error_buf, "%s", strerror (errno));
		}


		return status;
	}

	return 0;
}

char *jr_getcwd_malloc (
	char *						error_buf)
{
	char *						path_buf			= 0;
	jr_int						path_buf_size		= 64;
	jr_int						status;

		
	do {
		path_buf_size	*= 2;
		path_buf		= jr_realloc (path_buf, path_buf_size);

		if (path_buf == 0) {
			jr_esprintf (error_buf, "out of memory");
			return 0;
		}

		status		= jr_getcwd (path_buf, path_buf_size, error_buf);
	}
	while (status == jr_NO_ROOM_ERROR);

	if (status != 0) {
		jr_free (path_buf);
		return 0;
	}
	return path_buf;
}
