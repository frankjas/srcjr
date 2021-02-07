#include "ezport.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "jr/syscalls.h"
#include "jr/error.h"

jr_int jr_SymLink (
	const char *			link_target_path,
	const char *			new_link_path,
	char *					error_buf)
{
	jr_int					status;


	status	= symlink (link_target_path, new_link_path);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		status	= jr_ConvertErrno (errno);
		goto return_status;
	}

	status = 0;

	return_status : {
	}

	return status;
}

jr_int jr_ReadLink (
	const char *			link_path,
	char *					link_target_buf,
	jr_int					buf_length,
	char *					error_buf)
{
	jr_int					status;


	status	= readlink (link_path, link_target_buf, buf_length);

	if (status < 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		status	= jr_ConvertErrno (errno);
		goto return_status;
	}
	else if (status == buf_length) {
		jr_esprintf (error_buf, "buffer too small");
		status	= jr_NO_ROOM_ERROR;
		goto return_status;
	}
	/*
	** 5/18/08: readlink() doesn't add null byte to buffer.
	*/
	link_target_buf[status]	= 0;

	status = 0;

	return_status : {
	}

	return status;
}
