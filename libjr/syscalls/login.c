#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "jr/misc.h"
#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/syscalls.h"
#include "jr/error.h"

#include "jr/login.h"

jr_LoginInfoType *	jr_LoginInfoCreate(user_name_arg,tty_name_arg,errbuf)
						const char *	user_name_arg ;
						const char *	tty_name_arg ;
						char *			errbuf ;
{
	jr_LoginInfoType *	new_login_info_result ;

	new_login_info_result = (jr_LoginInfoType *) jr_malloc(sizeof(jr_LoginInfoType)) ;

	if (jr_LoginInfoInit(new_login_info_result, user_name_arg, tty_name_arg, errbuf) == 0) {
		return(new_login_info_result) ;
	}
	else {
		jr_free(new_login_info_result) ;
		return(0) ;
	}
}

void jr_LoginInfoUndo(login_info_ptr)
	jr_LoginInfoType *	login_info_ptr ;
{
	if (login_info_ptr->user_name)	jr_free(login_info_ptr->user_name) ;
	if (login_info_ptr->tty_name)	jr_free(login_info_ptr->tty_name) ;

	memset(login_info_ptr, 0, sizeof(*login_info_ptr)) ;
}

void jr_LoginInfoDestroy(login_info_ptr)
	jr_LoginInfoType *	login_info_ptr ;
{
	jr_LoginInfoUndo(login_info_ptr) ;

	jr_free(login_info_ptr) ;
}

#ifndef missing_utmp

#include <utmp.h>

jr_int jr_LoginInfoInit(login_info_ptr, user_name_arg, tty_name_arg, errbuf)
	jr_LoginInfoType *	login_info_ptr ;
	const char *		user_name_arg ;
	const char *		tty_name_arg ;
	char *				errbuf ;
{
	struct utmp			utmp ;
	char *				rfn = "/etc/utmp" ;
	FILE *				rfp ;

	if ((rfp = fopen(rfn, "r")) == NULL) {
		jr_esprintf (errbuf, "couldn't open '%.32s' for reading", rfn) ;
		return(-1) ;
	}
	
	/* look in utmp file for information */

	login_info_ptr->total_number_of_users	= 0 ;
	login_info_ptr->number_of_logins		= 0 ;
	login_info_ptr->tty_name				= 0 ;
	login_info_ptr->user_name				= jr_strdup(user_name_arg) ;

	while (fread((char *)&utmp, sizeof(utmp), 1, rfp) == 1) {
		if(utmp.ut_name[0] == '\0') {
			continue; /* null entries are people who logged out */
		}

		login_info_ptr->total_number_of_users++ ;
		if (strcmp(login_info_ptr->user_name, utmp.ut_name) == 0) {
			char *	tty_letters_from_arg ;
			char *	tty_letters_from_utmp ;

			login_info_ptr->number_of_logins++ ;

			tty_letters_from_arg	= jr_TTY_LettersFromDeviceName((char *) tty_name_arg) ;
			tty_letters_from_utmp	= jr_TTY_LettersFromDeviceName((char *) utmp.ut_line) ;

			if (strcmp(tty_letters_from_arg, tty_letters_from_utmp) == 0) {
				/* make sure the person logged in is both the user
				   and has the right terminal before saying that
				   he is still logged in */
				login_info_ptr->tty_name		= jr_strdup(tty_name_arg) ;
		   }
		}
	}

	fclose(rfp) ;

	return(0) ;
}

#else

jr_int jr_LoginInfoInit(login_info_ptr, user_name, tty_name)
	jr_LoginInfoType *	login_info_ptr ;
	const char *		user_name_arg ;
	const char *		tty_name_arg ;
{
	login_info_ptr->total_number_of_users	= 1 ;
	login_info_ptr->number_of_logins			= 1 ;
	login_info_ptr->tty_name					= jr_strdup("console") ;
	login_info_ptr->user_name				= jr_strdup(user_name_arg) ;

	return(0) ;
}

#endif
