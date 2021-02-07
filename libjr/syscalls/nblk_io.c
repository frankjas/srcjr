#define _POSIX_SOURCE 1

#include "ezport.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include "jr/io.h"
#include "jr/error.h"


jr_int jr_FileDescInitNonBlockingIO (fd, error_buf)
	jr_int			fd;
	char *			error_buf;
{
	jr_int			flags;
	jr_int			status;

	flags = fcntl (fd, F_GETFL, 0);

	if (flags == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_GETFL): %s",
			fd, strerror (errno)
		);
		return jr_INTERNAL_ERROR;
	}

	flags |= O_NONBLOCK;      

	status = fcntl (fd, F_SETFL, flags);

	if (status == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_SETFL, %#x): %s",
			fd, flags, strerror (errno)
		);
		return jr_INTERNAL_ERROR;
	}

	return 0;
}


jr_int jr_FileDescUndoNonBlockingIO (fd, error_buf)
	jr_int				fd;
	char *				error_buf;
{
	jr_int				flags;
	jr_int				status;


	flags = fcntl (fd, F_GETFL, 0);

	if (flags == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_GETFL): %s",
			fd, strerror (errno)
		);
		return jr_INTERNAL_ERROR;
	}

	flags		&= ~O_NONBLOCK;

	status = fcntl (fd, F_SETFL, flags);

	if (status == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_SETFL, %#x): %s",
			fd, flags, strerror (errno)
		);
		return jr_INTERNAL_ERROR;
	}

	return 0;
}
