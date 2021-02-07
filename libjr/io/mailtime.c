#include "mailproj.h"

#include <setjmp.h>
#include <signal.h>

static sigjmp_buf JumpBuffer ;

static void local_alarm_handler(signum)
	int signum ;
{
	siglongjmp (JumpBuffer , 17) ;
}


jr_int jr_MailServerValidateWithTimeOut (
								smtp_server_name, opt_sender_address, time_out_seconds, error_buf
							)
	const char *			smtp_server_name;
	const char *			opt_sender_address;
	jr_int					time_out_seconds;
	char *					error_buf;
{
	struct				sigaction new_action[1];
	struct				sigaction old_action[1];
	jr_int				prev_alarm_time ;
	jr_int				timed_out;

	jr_int				status;


	if (sigsetjmp (JumpBuffer, 1)  ==  0) {
		/*
		 * If we are inside this condition then
		 * this is the first time that setjmp was called
		 * and we should therefore set up the signal handler
		 * and alarm values before we begin the "timed work".
		 */
		new_action->sa_handler = local_alarm_handler;
		sigemptyset (&new_action->sa_mask);
		new_action->sa_flags   = SA_RESTART;

		prev_alarm_time    = alarm(0);
		sigaction (SIGALRM, new_action, old_action) ;
		alarm (time_out_seconds);

		/* timed work */
		status		= jr_MailServerValidate (
						smtp_server_name, opt_sender_address, error_buf
					);

		timed_out	= 0;
	}
	else {
		/*
		 * If we are inside this condition, then the alarm
		 * went off and we weren't done reading.
		 */
		timed_out = 1;
	}

	/* reset the old alarm and handler */

	alarm (0);
	sigaction(SIGALRM , old_action , 0) ;
	alarm(prev_alarm_time) ;

	
	if (timed_out) {
		jr_esprintf (error_buf, "connect timed out");
		status	= jr_INET_TIMED_OUT;
	}

	return status;
}


jr_int jr_MailAddressValidateWithTimeOut (
								recipient_address, smtp_server_name, sender_address, time_out_seconds,
								error_buf
							)
	const char *			recipient_address;
	const char *			smtp_server_name;
	const char *			sender_address;
	jr_int					time_out_seconds;
	char *					error_buf;
{
	struct				sigaction new_action[1];
	struct				sigaction old_action[1];
	jr_int				prev_alarm_time ;
	jr_int				timed_out;

	jr_int				status;


	if (sigsetjmp (JumpBuffer, 1)  ==  0) {
		/*
		 * If we are inside this condition then
		 * this is the first time that setjmp was called
		 * and we should therefore set up the signal handler
		 * and alarm values before we begin the "timed work".
		 */
		new_action->sa_handler = local_alarm_handler;
		sigemptyset (&new_action->sa_mask);
		new_action->sa_flags   = SA_RESTART;

		prev_alarm_time    = alarm(0);
		sigaction (SIGALRM, new_action, old_action) ;
		alarm (time_out_seconds);

		/* timed work */
		status		= jr_MailAddressValidate (
						recipient_address, smtp_server_name, sender_address,
						error_buf
					);

		timed_out	= 0;
	}
	else {
		/*
		 * If we are inside this condition, then the alarm
		 * went off and we weren't done reading.
		 */
		timed_out = 1;
	}

	/* reset the old alarm and handler */

	alarm (0);
	sigaction(SIGALRM , old_action , 0) ;
	alarm(prev_alarm_time) ;

	
	if (timed_out) {
		jr_esprintf (error_buf, "connect timed out");
		status	= jr_INET_TIMED_OUT;
	}

	return status;
}

