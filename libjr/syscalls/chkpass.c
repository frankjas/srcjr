#define _POSIX_SOURCE 1

#include "ezport.h"

#include <pwd.h>
#include <unistd.h>
#include <string.h>

#include "jr/syscalls.h"

jr_int jr_CheckUnixPassword(username, unencrypted_password_text, pw_entry_arg)
	char *		username ;
	char *		unencrypted_password_text ;
	void *		pw_entry_arg ;
{
	struct passwd *		pw_entry = pw_entry_arg ;
    char *				encrypted_passwd_text ;
	jr_int				difference ;

    encrypted_passwd_text	= (char *) crypt(unencrypted_password_text, pw_entry->pw_passwd) ;

    /* Compare the supplied password with the password file entry */
    difference				= strcmp(encrypted_passwd_text, pw_entry->pw_passwd) ;

	/*
	fprintf(stderr, "strcmp(\"%s\", \"%s\") == %d\n",
		encrypted_passwd_text,
		pw_entry->pw_passwd,
		difference
	) ;
	*/
	
	if (difference != 0) {
		return (-1);
    }
    else {
		return (0);
    }
}

/*
 * Sample use:

	if ((pw = getpwnam(user)) == NULL) {
		WriteToClient ("500 Unknown user, %s.", user);
		exit(1);
	}

	if (jr_CheckUnixPassword (user, oldpass, pw) != 0) {
		WriteToClient ("500 Old password is incorrect.");
		exit(1);
	}
 */
