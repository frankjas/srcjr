#include "ezport.h"

#include <stdio.h>

#ifdef ostype_winnt
#	include <windows.h>
#	include <stdlib.h>
#else
#	include <unistd.h>
#	include <string.h>
#	include <errno.h>

#endif

#include "jr/syscalls.h"
#include "jr/error.h"

#ifdef ostype_winnt
jr_int jr_chdir (
	const char *			dir_path,
	char *					error_buf)
{
	wchar_t					wpath_buf[ MAX_PATH+1];
	size_t					num_converted_chars;
	jr_int					status;

	status	= mbstowcs_s(
				&num_converted_chars,
				wpath_buf,
				sizeof( wpath_buf) / sizeof( wchar_t),
				dir_path,
				_TRUNCATE
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror(status));
		goto return_status;
	}

	if (! SetCurrentDirectoryW( wpath_buf)) {
		status	= GetLastError();
		jr_esprintf (error_buf, "%s", strerror(status));
		status	= jr_ConvertLastError( status);
		goto return_status;
	}

	return_status : {
	}

	return status;
}

#else
jr_int jr_chdir (
	const char *				dir_path,
	char *						error_buf)
{
	jr_int						status;

	status = chdir (dir_path);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));

		return jr_ConvertErrno (errno);
	}


	return 0;
}
#endif
