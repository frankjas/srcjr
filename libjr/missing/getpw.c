#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_passwd_funcs

#include <pwd.h>
#include <stdio.h>

#	ifdef has_hesiod

struct passwd *getpwuid(uid)
	jr_int uid ;
{
	char uid_string[128] ;
	static struct passwd pw_entry[1] ;
	static char   pw_name[128] ;
	static char   pw_passwd[128] ;
	static char   pw_comment[128] ;
	static char   pw_dir[512] ;
	static char   pw_shell[512] ;

	char  *entry_text ;
	char **vp ;
	jr_int nfields ;

	sprintf(uid_string,"%d", uid) ;
	vp = (char **) hes_resolve(uid_string, "uid") ;

	if (! (vp && *vp)) {
		fprintf(stderr,"no matching password entry for uid %d\n",
			uid) ;
		return(0) ; 
	}
	entry_text = *vp ;

	/*
	 * fprintf(stderr,"%s\n", entry_text) ;
	 */

	pw_entry->pw_name    = pw_name ;
	pw_entry->pw_passwd  = pw_passwd ;
	pw_entry->pw_comment = pw_comment ;
	pw_entry->pw_dir     = pw_dir ;
	pw_entry->pw_shell   = pw_shell ;

	nfields = sscanf(entry_text, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%[^:\n]",
					pw_entry->pw_name,
					pw_entry->pw_passwd,
					&(pw_entry->pw_uid),
					&(pw_entry->pw_gid),
					pw_entry->pw_comment,
					pw_entry->pw_dir,
					pw_entry->pw_shell
				) ;

	if (nfields != 7) {
		fprintf(stderr,"mis-matching number of struct entries %d of %d: '%s'\n",
			nfields, entry_text, 7) ;
		return(0) ;
	}
	return(pw_entry) ;
}

#	else


struct passwd *		getpwuid(uid_arg)
						uid_t	uid_arg ;
{
	char				username_buf[128] ;
	char *				username ;
	struct passwd *		passwd_ptr;
	
	username = getenv("USER") ;

	if (username == 0) {
		sprintf(username_buf, "%d", uid_arg) ;
		username = username_buf ;
	}

	passwd_ptr	= getpwnam(username) ;

	passwd_ptr->pw_uid	= uid_arg;

	return passwd_ptr;
}

struct passwd *		getpwnam(username)
						const char *	username ;
{
	static struct passwd	result[1] ;
	static char				pw_name_buf[128] ;
	static char 			pw_passwd_buf[4] ;
	static char 			pw_comment_buf[4] ;
	static char 			pw_gecos_buf[4] ;
	static char 			pw_dir_buf[4] ;
	static char 			pw_shell_buf[4] ;

	result->pw_name			= pw_name_buf ;
	result->pw_passwd		= pw_passwd_buf ;
	result->pw_uid			= -1;
	result->pw_gid			= -1;
	result->pw_quota		= 0;
	result->pw_comment		= pw_comment_buf ;
	result->pw_gecos		= pw_gecos_buf ;
	result->pw_dir			= pw_dir_buf ;
	result->pw_shell		= pw_shell_buf ;

	*pw_name_buf			= 0 ;
	*pw_passwd_buf			= 0 ;
	*pw_comment_buf			= 0 ;
	*pw_gecos_buf			= 0 ;
	*pw_dir_buf				= 0 ;
	*pw_shell_buf			= 0 ;

	strncpy(pw_name_buf, username, sizeof(pw_name_buf) - 1) ;
	pw_name_buf[sizeof(pw_name_buf) - 1] = 0 ;

	{
		char *	env_value_equivalent ;

		if (env_value_equivalent = getenv("PASSWD")) {
			result->pw_passwd		= env_value_equivalent ;
		}
		if (env_value_equivalent = getenv("GECOS")) {
			result->pw_gecos		= env_value_equivalent ;
		}
		if (env_value_equivalent = getenv("HOME")) {
			result->pw_dir			= env_value_equivalent ;
		}
		if (env_value_equivalent = getenv("SHELL")) {
			result->pw_shell		= env_value_equivalent ;
		}
	}

	return(result) ;
}

#	endif

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
