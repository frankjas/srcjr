#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_getcwd_objects

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

/* getcwd should be declared extern in unistd.h */
#include <unistd.h>

char *	getcwd(dirname_buf_arg, buffer_length)
	char *		dirname_buf_arg ;
	size_t		buffer_length ;
{
	char	local_buffer[2048] ;

	if ((dirname_buf_arg != 0) && (buffer_length == 0)) {
		errno = EINVAL ;
		return(0) ;
	}

	if (getwd(local_buffer) == 0) {
		/* errno should be set by getwd */
		strncpy(dirname_buf_arg, local_buffer, buffer_length-1) ;
		dirname_buf_arg[buffer_length-1] = 0 ;
		return(0) ;
	}
	else {
		if (dirname_buf_arg == 0) {
			buffer_length	= strlen(local_buffer) + 1 ;
			dirname_buf_arg = (char *) malloc(buffer_length) ;
			if (dirname_buf_arg == 0) {
				errno = EAGAIN ;
				return(0) ;
			}
		}
		if (strlen(local_buffer) > (buffer_length-1)) {
			errno = ERANGE ;
			return(0) ;
		}
		strcpy(dirname_buf_arg, local_buffer) ;
	}

	return(dirname_buf_arg) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
