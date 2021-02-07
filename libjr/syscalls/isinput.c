#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "jr/syscalls.h"

jr_int jr_FileDescHasPendingInput(fd)
	jr_int fd ;
{
#	ifdef FIONREAD
	{
		long nchars = 0 ;

		if (ioctl(fd, FIONREAD, &nchars) == 0) {
			return((jr_int) nchars) ;
		}
		else {
			return(-1) ;
		}
	}
#	else
	{
		errno = ENOSYS ;		/* function not implemented */
		return(-1) ;
	}
#	endif

}
