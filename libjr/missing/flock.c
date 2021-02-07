#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_flock_objects

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int flock (fd, operation)
	int fd;
	int operation;
{
	struct flock	new_lock [1];
	jr_int			status;

	new_lock->l_whence		= SEEK_SET;
	new_lock->l_start		= 0;
	new_lock->l_len			= 0;

	if (operation & LOCK_UN) {
		new_lock->l_type		= F_UNLCK;
	}
	if (operation & LOCK_SH) {
		new_lock->l_type		= F_RDLCK;
	}
	if (operation & LOCK_EX) {
		new_lock->l_type		= F_WRLCK;
	}

	if (operation & LOCK_NB) {
		status		= fcntl (fd, F_SETLK, new_lock);
	}
	else {
		status		= fcntl (fd, F_SETLKW, new_lock);
	}

	return status;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
