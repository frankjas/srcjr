#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <sys/file.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

#include "jr/trace.h"

#define __jr_compiling_timelock
#include "jr/syscalls.h"

#define ENDPERM (S_IREAD | S_IWRITE | 0660 )

/*--------------------------------*/

static sigjmp_buf env ;

static void onalarm(signum)
	jr_int signum ;
{
	siglongjmp(env, 1) ;
}

/*--------------------------------*/

jr_int jr_FileDescTimedFlock (fd, how, time)
	jr_int			fd ;
	const char *	how ;
	jr_int			time ;
{
	struct sigaction	new_action [1];
	struct sigaction	old_action [1];
	unsigned jr_int savealarm				= 0 ;				/* to shutup 'uninit.' msg */
	jr_int			kind					= LOCK_SH ;			/* to shutup 'uninit.' msg */
	jr_int			retval ;

	if (*how == 'u') {
		flock (fd, LOCK_UN) ;
		retval = 1 ;

		if (jr_do_trace(jr_locks)) {
			fprintf(stderr,"jr_FileDescTimedFlock(%d,\"%s\", %d) unlocking. return(%d)\n",
				fd, how, time, retval) ;
		}
	}
	else {
		
		switch (*how) {
			case 'r' :
				kind = LOCK_SH ;
				break ;

			default  :
			case 'w' :
				kind = LOCK_EX ;
				break ;
		}
				
		retval = (flock (fd, kind | LOCK_NB) == 0) ;

		if ((time == 0) | (retval)) {
			if (jr_do_trace(jr_locks)) {
				fprintf(stderr,"jr_FileDescTimedFlock(%d,\"%s\", %d) No wait. return(%d)\n",
					fd, how, time, retval) ;
			}
			return (retval) ;
		}

		/*-------------------------------------------------*/
		if (sigsetjmp(env, 1) == 0) {
			new_action->sa_handler	= onalarm;
			sigemptyset (&new_action->sa_mask);
			new_action->sa_flags	= SA_RESTART;

			savealarm	= alarm(0);
			sigaction (SIGALRM, new_action, old_action);
			savealarm = alarm(time);
		}
		else {
			alarm (0);
			sigaction (SIGALRM, old_action, 0);
			alarm (savealarm);

			retval = 0 ;
			if (jr_do_trace(jr_locks)) {
				fprintf(stderr," timed out. return(%d)\n", retval) ;
			}
			return(retval) ;
		}
		/*-------------------------------------------------*/

		if (jr_do_trace(jr_locks)) {
			fprintf(stderr,"jr_FileDescTimedFlock(%d,\"%s\", %d) waiting....",
				fd, how, time
			) ;
		}
		retval = (flock (fd, kind) == 0) ;

		alarm (0);
		sigaction (SIGALRM, old_action, 0);
		alarm (savealarm);

		if (jr_do_trace(jr_locks)) {
			fprintf(stderr," got \"%s\" lock. return(%d)\n", how, retval) ;
		}
	}
	return(retval) ;
}
