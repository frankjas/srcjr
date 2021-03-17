#ifndef __JR_LOGIN_H__
#define __JR_LOGIN_H__

#include "ezport.h"

typedef struct {
	jr_int		total_number_of_users ;	/* total number of user's logged in */
	jr_int		number_of_logins ;		/* number of times this user was logged in on different ttys */
	char *	user_name ;				/* a jr_strduped string copied from the arg to Init */
	char *	tty_name ;				/* a jr_strduped string filled only if user on matching tty */
} jr_LoginInfoType ;

extern jr_LoginInfoType *	jr_LoginInfoCreate		PROTO((
									const char *		user_name_arg,
									const char *		tty_name_arg,
									char *				errbuf
							)) ;
extern void					jr_LoginInfoDestroy		PROTO((jr_LoginInfoType *	login_info_ptr)) ;

extern jr_int					jr_LoginInfoInit		PROTO((
									jr_LoginInfoType *	login_info_ptr,
									const char *		user_name_arg,
									const char *		tty_name_arg,
									char *				errbuf
							)) ;

extern void					jr_LoginInfoUndo		PROTO((jr_LoginInfoType *	login_info_ptr)) ;

extern struct passwd *		jr_GetPasswdAndUserName	PROTO((char *namebuf)) ;

#endif
