#include "ezport.h"

/*
** 5-17-2011: implementation that avoid dependency on APR
** to allow export of jr_malloc() w/o requiring APR.
*/
#ifdef ostype_winnt
#	include <windows.h>
#	include <stdlib.h>
#	include <io.h>
#else
#	include <unistd.h>
#	include <string.h>
#	include <errno.h>

#endif

#include "jr/syscalls.h"
#include "jr/error.h"


#ifdef ostype_winnt

jr_int jr_UnlinkNoApr (
	const char *			file_path,
	char *					error_buf)
{
	wchar_t					wpath_buf[ MAX_PATH+1];
	size_t					num_converted_chars;
	jr_int					status;

	status	= mbstowcs_s(
				&num_converted_chars,
				wpath_buf,
				sizeof( wpath_buf) / sizeof( wchar_t),
				file_path,
				_TRUNCATE
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror(status));
		goto return_status;
	}

	if (! DeleteFileW( wpath_buf)) {
		status	= GetLastError();
		jr_esprintf (error_buf, "%s", strerror(status));
		status	= jr_ConvertLastError( status);
		goto return_status;
	}

	return_status : {
	}

	return status;
}

jr_int jr_FileExists(
	const char *			file_path)
{
	if (_access( file_path, 00) == 0) {
		return 1;
	}
	return 0;
}

#else

jr_int jr_UnlinkNoApr (
	const char *			file_path,
	char *					error_buf)
{
	jr_int					status;

	status	= unlink (file_path);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror( errno));
		status	= jr_ConvertErrno (errno);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}

	return status;
}

jr_int jr_FileExists(
	const char *			file_path)
{
	if (access( file_path, F_OK) == 0) {
		return 1;
	}
	return 0;
}

#endif
