#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "jr/syscalls.h"

time_t jr_FilePtrModifiedTime(fp)
	FILE *fp ;
{
	struct stat buf ;
	jr_int fd = fileno(fp) ;

	if (fstat(fd,&buf) != 0) {
		return(0) ;
	}
	return(buf.st_mtime) ;
}

time_t jr_FileDescModifiedTime(fd)
	jr_int fd ;
{
	struct stat buf ;

	if (fstat(fd,&buf) != 0) {
		return(0) ;
	}
	return((jr_int) buf.st_mtime) ;
}

time_t jr_FileNameModifiedTime(fn)
	const char *	fn ;
{
	struct stat buf ;

	if (stat(fn,&buf) != 0) {
		return(0) ;
	}
	return((jr_int) buf.st_mtime) ;
}
