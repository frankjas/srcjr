#include "ezport.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>

#include "jr/syscalls.h"
#include "jr/error.h"


jr_int jr_FileNameSetOwner (
	const char *		path,
	const char *		owner_name,
	const char *		group_name,
	char *				error_buf)
{
	uid_t				owner_uid;
	gid_t				group_gid;
	jr_int				status;

	struct passwd *		passwd_ptr ;
	struct group *		group_ptr;

	errno = 0;

	passwd_ptr = getpwnam(owner_name) ;

	if (passwd_ptr == 0) {
		if (errno == 0) {
			jr_esprintf (error_buf, "user '%.32s' not found", owner_name);
		}
		else {
			jr_esprintf (error_buf, "user '%.32s' not accessible: %s", owner_name, strerror (errno));
		}
		return jr_ConvertErrno (errno);
	}
	else {
		owner_uid	= passwd_ptr->pw_uid;
	}


	errno = 0;

	group_ptr	= getgrnam (group_name);

	if (group_ptr == 0) {
		if (errno == 0) {
			jr_esprintf (error_buf, "group '%.32s' not found", group_name);
		}
		else {
			jr_esprintf (error_buf, "group '%.32s' not accessible: %s", group_name, strerror (errno));
		}
		return jr_ConvertErrno (errno);
	}
	else {
		group_gid	= group_ptr->gr_gid;
	}


	/*
	** 5/31/08: lchown() doesn't follow symlinks, this is more appropriate for restoring
	** backups, since we need to set ownership on the link file, if the link target already
	** exists, we don't want to change its ownership, and if it doesn't we don't want
	** an error and the restore will restore it with proper ownership.
	*/

	status = lchown (path, owner_uid, group_gid);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		return jr_ConvertErrno (errno);;
	}

	return 0;
}

jr_int jr_IsChownAllowed()
{
	return jr_IsRootUser();
}

