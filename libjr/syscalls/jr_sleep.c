#include "ezport.h"

#include <apr-1/apr_time.h>

#ifdef ostype_winnt
#else
#	include <sys/select.h>
#endif

#include "jr/syscalls.h"


void jr_sleep (sleep_sec, sleep_usec)
	jr_int				sleep_sec;
	jr_int				sleep_usec;
{
	apr_interval_time_t	apr_interval;

	apr_interval	= sleep_sec * 1000000LL + sleep_usec;

	apr_sleep (apr_interval);
}

#ifdef ostype_winnt

jr_int jr_uninterrupted_sleep (
	jr_int				sleep_sec,
	jr_int				sleep_usec)
{
	Sleep( (sleep_sec * 1000000.0 + sleep_usec) / 1000);
	/*
	** 2/27/07: Doesn't return a value, expects milli-seconds
	*/

	return 0;
}

#else

jr_int jr_uninterrupted_sleep (
	jr_int				sleep_sec,
	jr_int				sleep_usec)
{
	struct timeval		last_tv;
	struct timeval		curr_tv;
	jr_int				status;

	curr_tv.tv_sec		= sleep_sec;
	curr_tv.tv_usec		= sleep_usec;

	last_tv	= curr_tv;

	/*
	** 2/27/07: ToDo: On systems where select() doesn't decrement the timeval,
	** use time() to figure out how much time is left.
	*/
	while (curr_tv.tv_sec > 0 || curr_tv.tv_usec > 0) {

		status	= select (0, 0, 0, 0, &curr_tv);

		if (status != 0 && errno == EINTR) {
			if (curr_tv.tv_sec >= last_tv.tv_sec) {
				last_tv.tv_sec--;
				last_tv.tv_usec = 0;
			}
			else {
				/*
				** 2/27/07: else select is decrementing the time on it's own
				*/
				last_tv	= curr_tv;
			}
			curr_tv	= last_tv;
		}
		else {
			break;
		}
	}

	return 0;

#	ifdef mess_with_alarms_to_sleep
	{
		while (sleep_amount > 0) {
			sleep_amount = sleep (sleep_amount);
		}

		return sleep_amount;
	}
#	endif
}
#endif



#ifdef use_unix_setitimer_sleep

static void on_alarm (sig)
	jr_int				sig;
{
}

void jr_sleep (sleep_sec, sleep_usec)
	jr_int				sleep_sec;
	jr_int				sleep_usec;
{
	struct sigaction	new_action[1];
	struct sigaction	old_action[1];
	struct itimerval	new_timer_value[1];
	struct itimerval	old_timer_value[1];


	/*
	 * Turn off any timers before setting a new alarm handler
	 */
	memset (new_timer_value, 0, sizeof (*new_timer_value));

	setitimer (ITIMER_REAL, new_timer_value, old_timer_value);

	new_action->sa_handler = on_alarm;
	sigemptyset (&new_action->sa_mask);
	new_action->sa_flags   = SA_RESTART;

	sigaction (SIGALRM, new_action, old_action) ;
	/*
	 * We need to set a handler for SIGALRM, since according
	 * to POSIX, only a signal that executes a signal handling
	 * function causes pause() to awake.
	 */


	new_timer_value->it_value.tv_sec	= sleep_sec;
	new_timer_value->it_value.tv_usec	= sleep_usec;

	setitimer (ITIMER_REAL, new_timer_value, 0);

	pause ();

	memset (new_timer_value, 0, sizeof (*new_timer_value));
	setitimer (ITIMER_REAL, new_timer_value, 0);

	sigaction (SIGALRM, old_action, 0);

	setitimer (ITIMER_REAL, old_timer_value, 0);
}
#endif
