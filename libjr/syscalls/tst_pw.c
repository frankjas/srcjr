#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

#include "jr/trace.h"
#include "jr/malloc.h"

#include "jr/login.h"

char *	ProgramName = "testpw" ;

void main()
{
	char				user_name_buf[128] ;
	char				tty_name_buf[128] ;
	char 				error_buf[jr_ERROR_LEN] ;
	struct passwd *		pw_entry ;
	jr_LoginInfoType *	login_info_ptr ;

	jr_set_trace(jr_malloc_stats) ;

	pw_entry = jr_GetPasswdAndUserName(user_name_buf) ;

	fprintf(stdout,"real user name and user id: %.150s %d\n", user_name_buf, pw_entry->pw_uid) ;

	strcpy(tty_name_buf, ttyname(fileno(stdin))) ;

	/* test for user and working tty */ {
		fprintf(stdout, "jr_LoginInfoCreate(%s, %s)\n",
			user_name_buf, tty_name_buf
		) ;

		login_info_ptr = jr_LoginInfoCreate(user_name_buf, tty_name_buf, error_buf) ;
		if (! login_info_ptr) {

			jr_esprintf(error_buf, "%s : %s", ProgramName, error_buf) ;
			perror(error_buf) ;
			exit(-1) ;
		}

		fprintf(stdout, "\tTotal # users on system: %d\n", login_info_ptr->total_number_of_users) ;
		fprintf(stdout, "\tNumber of users logged in as %s on system: %d\n",
			login_info_ptr->user_name, login_info_ptr->number_of_logins
		) ;
		fprintf(stdout, "\tIs user logged in on tty %s - %s (%s)\n",
			tty_name_buf, login_info_ptr->tty_name ? "yes" : "no",
			login_info_ptr->tty_name
		) ;

		jr_LoginInfoDestroy(login_info_ptr) ;
	}

	strcpy(tty_name_buf, "/dev/ttyb") ;

	/* test for user and possibly-working tty */ {
		fprintf(stdout, "jr_LoginInfoCreate(%s, %s)\n",
			user_name_buf, tty_name_buf
		) ;

		login_info_ptr = jr_LoginInfoCreate(user_name_buf, tty_name_buf, error_buf) ;
		if (! login_info_ptr) {

			jr_esprintf(error_buf, "%s : %s", ProgramName, error_buf) ;
			perror(error_buf) ;
			exit(-1) ;
		}

		fprintf(stdout, "\tTotal # users on system: %d\n", login_info_ptr->total_number_of_users) ;
		fprintf(stdout, "\tNumber of users logged in as %s on system: %d\n",
			login_info_ptr->user_name, login_info_ptr->number_of_logins
		) ;
		fprintf(stdout, "\tIs user logged in on tty %s - %s (%s)\n",
			tty_name_buf, login_info_ptr->tty_name ? "yes" : "no",
			login_info_ptr->tty_name
		) ;

		jr_LoginInfoDestroy(login_info_ptr) ;
	}

	if (jr_do_trace(jr_malloc_stats)) {
		jr_malloc_stats(stdout, "After free") ;
	}

	exit(0) ;
}
