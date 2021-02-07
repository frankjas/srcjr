#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef ostype_winnt
#	include <windows.h>
#	include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

#include "jr/error.h"

#	ifdef ostype_winnt
jr_int jr_GetTempDir (tmp_dir_buf, tmp_dir_buf_length, error_buf)
	char *			tmp_dir_buf;
	jr_int			tmp_dir_buf_length;
	char *			error_buf;
{
	jr_int			dir_length;

	dir_length		= GetTempPath( tmp_dir_buf_length, tmp_dir_buf);

	if (dir_length + 1  >  tmp_dir_buf_length) {
		jr_esprintf (error_buf, "buffer too small, minimum size is %u", dir_length + 1);
		return -1;
	}

	return 0;
}

#	else

jr_int jr_GetTempDir (tmp_dir_buf, tmp_dir_buf_length, error_buf)
	char *			tmp_dir_buf;
	jr_int			tmp_dir_buf_length;
	char *			error_buf;
{
	const char *	tmp_dir				= "/tmp";

	if ((jr_int) strlen (tmp_dir) + 1  >  tmp_dir_buf_length) {
		jr_esprintf (error_buf, "buffer too small, minimum size is %u", (unsigned) strlen (tmp_dir) + 1);
		return -1;
	}

	strcpy (tmp_dir_buf, tmp_dir);

	return 0;
}

#	endif
