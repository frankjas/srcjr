#define _POSIX_SOURCE 1

#include "ezport.h"

#include <setjmp.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "jr/syscalls.h"

static sigjmp_buf JumpBuffer ;

static void local_alarm_handler(signum)
	jr_int signum ;
{
	siglongjmp (JumpBuffer , 17) ;
}
/* .bp */

jr_int jr_Timed_fgets (buffer, length, rfp, seconds)
	char *	buffer;
	jr_int	length;
	FILE *  rfp ;
	jr_int	seconds ;
{
	struct	sigaction new_action[1];
	struct	sigaction old_action[1];
	jr_int	prev_alarm_time				= 0 ;		/* to shutup 'uninit.' msg */
	jr_int	timed_out;
	jr_int	jmpval;
	char *	status						= NULL;		/* to shutup 'uninit.' msg */

	if ((jmpval = sigsetjmp(JumpBuffer, 1)) == 0) {
		new_action->sa_handler = local_alarm_handler;
		sigemptyset (&new_action->sa_mask);
		new_action->sa_flags   = SA_RESTART;

		prev_alarm_time    = alarm(0);
		sigaction (SIGALRM, new_action, old_action) ;
		alarm (seconds);

		/* timed work */
		status = fgets (buffer , length , rfp);

		timed_out = 0;
	}
	else {
		timed_out = 1;
	}

	/* reset the old alarm and handler */

	alarm (0);
	sigaction(SIGALRM , old_action , 0) ;
	alarm(prev_alarm_time) ;

	if (timed_out) {
		return -2;
	}

	if (status == NULL) {
		return -1;
	}
	return 0;
}

