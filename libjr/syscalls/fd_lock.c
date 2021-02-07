#define _POSIX_SOURCE 1

#include "ezport.h"

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/file.h>
#include <unistd.h>

#include "jr/syscalls.h"
#include "jr/error.h"

jr_int jr_fcntl_flock (fd, operation, is_blocking)
	jr_int			fd;
	jr_int			operation;
	jr_int			is_blocking;
{
	struct flock	new_lock [1];
	jr_int			status;

	new_lock->l_whence		= SEEK_SET;
	new_lock->l_start		= 0;
	new_lock->l_len			= 0;
	new_lock->l_type		= operation;

	if (is_blocking) {
		status		= fcntl (fd, F_SETLKW, new_lock);
	}
	else {
		status		= fcntl (fd, F_SETLK, new_lock);
	}

	return status;
}

static sigjmp_buf	LockEnv ;

static void onalarm(signum)
	jr_int signum ;
{
	siglongjmp(LockEnv, 1) ;
}


jr_int jr_FileDescReadLock (file_descriptor, time_out, error_buf)
	jr_int			file_descriptor;
	jr_int			time_out;
	char *			error_buf;
{
	jr_int			return_status;

	if (time_out > 0) {
		struct sigaction	new_action [1];
		struct sigaction	old_action [1];
		unsigned jr_int		savealarm				= 0 ;		/* to shutup 'uninit.' msg */

		/*
		 * break out of the lock if it takes too long
		 */
		if (sigsetjmp (LockEnv, 1) == 0) {
			/*
			 * setting the jump, if we can't get the lock
			 */
			new_action->sa_handler	= onalarm;
			sigemptyset (&new_action->sa_mask);
			new_action->sa_flags	= SA_RESTART;

			savealarm	= alarm(0);
			sigaction (SIGALRM, new_action, old_action);
			alarm (time_out);

			/*
			 * wait for the lock to happen
			 */
			if (flock (file_descriptor, LOCK_SH) < 0) {
				jr_esprintf (error_buf, "couldn't lock file: %s", strerror (errno));
				return_status = jr_INTERNAL_ERROR;
			}
			else {
				return_status = 0;
			}
		}
		else {
			/*
			 * we timed out
			 */
			jr_esprintf (error_buf, "timed out");
			return_status = jr_TIME_OUT_ERROR;
		}
		alarm (0);
		sigaction (SIGALRM, old_action, 0);
		alarm (savealarm);
	}
	else {
		/*
		 * try a non-blocking lock
		 */
		if (flock (file_descriptor, LOCK_SH | LOCK_NB) < 0) {
			if (errno == EWOULDBLOCK) {
				jr_esprintf (error_buf, "timed out");
				return_status = jr_TIME_OUT_ERROR;
			}
			else {
				jr_esprintf (error_buf, "couldn't lock file: %s", strerror (errno));
				return_status = jr_INTERNAL_ERROR;
			}
		}
		else {
			return_status = 0;
		}
	}

	return return_status;
}

jr_int jr_FileDescWriteLock (file_descriptor, time_out, error_buf)
	jr_int		file_descriptor;
	jr_int		time_out;
	char *		error_buf;
{
	jr_int		return_status;

	if (time_out > 0) {
		/*
		 * break out of the lock if it takes too long
		 */
		struct sigaction	new_action [1];
		struct sigaction	old_action [1];
		unsigned jr_int		savealarm				= 0 ;		/* to shutup 'uninit.' msg */

		if (sigsetjmp (LockEnv, 1) == 0) {
			/*
			 * setting the jump, if we can't get the lock
			 */
			new_action->sa_handler	= onalarm;
			sigemptyset (&new_action->sa_mask);
			new_action->sa_flags	= SA_RESTART;

			savealarm	= alarm(0);
			sigaction (SIGALRM, new_action, old_action);
			alarm(time_out);

			/*
			 * wait for the lock to happen
			 */
			if (flock (file_descriptor, LOCK_EX) != 0) {
				jr_esprintf (error_buf, "couldn't flock: %s", strerror (errno));
				return_status = jr_INTERNAL_ERROR;
			}
			else {
				return_status = 0;
			}
		}
		else {
			/*
			 * we timed out
			 */
			jr_esprintf (error_buf, "timed out");
			return_status = jr_TIME_OUT_ERROR;
		}

		alarm (0);
		sigaction (SIGALRM, old_action, 0);
		alarm (savealarm);
	}
	else {
		/*
		 * try a no-blocking lock
		 */
		if (flock (file_descriptor, LOCK_EX | LOCK_NB) != 0) {
			if (errno != EWOULDBLOCK) {
				jr_esprintf (error_buf, "couldn't flock: %s", strerror (errno));
				return_status = jr_INTERNAL_ERROR;
			}
			else {
				jr_esprintf (error_buf, "timed out");
				return_status = jr_TIME_OUT_ERROR;
			}
		}
		else {
			return_status = 0;
		}
	}

	return return_status;
}


void jr_FileDescUnLock (file_descriptor)
	jr_int			file_descriptor;
{
	if (flock (file_descriptor, LOCK_UN) < 0) {
		/*
		 * only possible if file descriptor was bad
		 */
	}
}


