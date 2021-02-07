#include "ezport.h"

#include "jr/syscalls.h"
#include "jr/apr.h"

jr_int jr_PathIsDirectory(pathname, error_buf)
	const char *	pathname ;
	char *			error_buf;
{
	struct jr_stat	stat_buf[1];
	jr_int			status;
	jr_int			is_dir;

	status	= jr_stat(pathname, stat_buf, error_buf);

	if (status != 0) {
		return status;
	}

	is_dir = jr_stat_is_directory (stat_buf);

	return(is_dir) ;
}


jr_int jr_PathIsFile(pathname, error_buf)
	const char *	pathname ;
	char *			error_buf;
{
	struct jr_stat	stat_buf[1];
	jr_int			status;
	jr_int			is_file;

	status	= jr_stat(pathname, stat_buf, error_buf);

	if (status != 0) {
		return status;
	}

	is_file = jr_stat_is_file (stat_buf);

	return(is_file) ;
}


jr_int jr_PathIsLink(pathname, error_buf)
	const char *	pathname ;
	char *			error_buf;
{
	struct jr_stat	stat_buf[1];
	jr_int			status;
	jr_int			is_link;

	status	= jr_lstat(pathname, stat_buf, error_buf);

	if (status != 0) {
		return status;
	}

	is_link = jr_stat_is_link (stat_buf);

	return(is_link) ;
}
