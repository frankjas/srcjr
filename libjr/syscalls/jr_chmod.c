#include "ezport.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "jr/syscalls.h"
#include "jr/error.h"


jr_int jr_FileNameSetPerms (
	const char *		path,
	jr_int				perm_flags,
	char *				error_buf)
{
	jr_int				status;


	status = chmod (path, jr_PermsToUnixMode (perm_flags));

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		return jr_ConvertErrno (errno);;
	}

	return 0;
}


jr_int jr_FileNameGetPerms (
	const char *		path,
	char *				error_buf)
{
	struct stat			stat_info[1];
	jr_int				perm_flags;
	jr_int				status;

	status	= stat (path, stat_info);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		return jr_ConvertErrno (errno);;
	}

	perm_flags	= jr_UnixModeToPerms (stat_info->st_mode);


	return perm_flags;
}

