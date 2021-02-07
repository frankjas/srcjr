#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>


#include "jr/syscalls.h"
#include "jr/time.h"
#include "jr/error.h"

jr_seconds_t 	jr_FileNameModTimeTouch(filename, error_buf)
	const char *	filename ;
	char *			error_buf;
{
	jr_seconds_t	current_time ;
	jr_int			status;

	if (access((char *) filename,R_OK) != 0) {
		FILE *	tfp ;
		
		tfp = fopen(filename,"w") ;
		if (tfp != NULL) {
			fclose(tfp) ;
		}
		else {
			jr_esprintf (error_buf, "%s", strerror (errno));
			return(jr_ConvertErrno (errno)) ;
		}
	}

	current_time	= jr_time_seconds();

	status	= jr_FileNameSetModTime(filename, current_time, error_buf) ;

	if (status != 0) {
		return status;
	}
	return current_time;
}

jr_seconds_t jr_FileNameSetModTime(filename, mod_time, error_buf)
	const char *	filename ;
	jr_seconds_t	mod_time ;
	char *			error_buf;
{
#	ifdef NeXT
	struct timeval t1, t2 ;
	struct timeval *tvp[2] ;

	tvp[0] = &t1 ;
	tvp[1] = &t2 ;
	t1.tv_sec = mod_time ;
	t1.tv_usec = 0 ;
	t2.tv_sec = mod_time ;
	t2.tv_usec = 0 ;
	if (utimes(filename, (void *) tvp) != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		return(jr_ConvertErrno (errno)) ;
	}
#	else
	struct timeval tvp[2] ;

	tvp[0].tv_sec = mod_time ;
	tvp[0].tv_usec = 0 ;
	tvp[1].tv_sec = mod_time ;
	tvp[1].tv_usec = 0 ;
	if (utimes((char *) filename, tvp) != 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		return(jr_ConvertErrno (errno)) ;
	}
#	endif
	
	return(0) ;
}
