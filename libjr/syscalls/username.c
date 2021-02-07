#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#include "jr/syscalls.h"
#include "jr/error.h"

/*
** 11/16/05: Could also use the environment USER and HOME to get the current user and dir.
** Under Microsoft, USERNAME and HOMEPATH.
*/

jr_int jr_GetUserName (namebuf, buf_length, error_buf)
	char *				namebuf ;
	jr_int				buf_length;
	char *				error_buf;
{
	jr_int				uid				= getuid() ;
	struct passwd *		passwd_ptr ;

	passwd_ptr = getpwuid(uid) ;

	if (passwd_ptr == 0) {
		jr_esprintf (error_buf, "couldn't get password entry: %s", strerror (errno));
		return jr_ConvertErrno (errno);
	}

	if ((jr_int) strlen (passwd_ptr->pw_name) + 1  >  buf_length) {
		jr_esprintf (error_buf, "buffer too small");
		return jr_NO_ROOM_ERROR;
	}

	sprintf (namebuf, "%.*s", buf_length - 1, passwd_ptr->pw_name);

	return 0;
}

jr_int jr_GetUserHomeDir (path_buf, buf_length, error_buf)
	char *				path_buf ;
	jr_int				buf_length;
	char *				error_buf;
{
	jr_int				uid				= getuid() ;
	struct passwd *		passwd_ptr ;

	passwd_ptr = getpwuid(uid) ;

	if (passwd_ptr == 0) {
		jr_esprintf (error_buf, "couldn't get password entry: %s", strerror (errno));
		return jr_ConvertErrno (errno);
	}

	if ((jr_int) strlen (passwd_ptr->pw_dir) + 1  >  buf_length) {
		jr_esprintf (error_buf, "buffer too small");
		return jr_NO_ROOM_ERROR;
	}
	sprintf (path_buf, "%.*s", buf_length - 1, passwd_ptr->pw_dir);

	return 0;
}

jr_int jr_GetGroupName (namebuf, buf_length, error_buf)
	char *				namebuf ;
	jr_int				buf_length;
	char *				error_buf;
{
	jr_int				gid				= getgid() ;
	struct group *		group_ptr ;

	group_ptr = getgrgid(gid) ;

	if (group_ptr == 0) {
		jr_esprintf (error_buf, "couldn't get password entry: %s", strerror (errno));
		return jr_ConvertErrno (errno);
	}

	if ((jr_int) strlen (group_ptr->gr_name) + 1  >  buf_length) {
		jr_esprintf (error_buf, "buffer too small");
		return jr_NO_ROOM_ERROR;
	}

	sprintf (namebuf, "%.*s", buf_length - 1, group_ptr->gr_name);

	return 0;
}

jr_int jr_IsRootUser ()
{
	jr_int				uid				= getuid() ;

	if (uid == 0) {
		return 1;
	}

	return 0;
}


