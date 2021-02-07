#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "jr/syscalls.h"

size_t jr_FilePtrDiskSize(fp)
	FILE *fp ;
{
	struct stat buf ;
	jr_int fd = fileno(fp) ;

	if (fstat(fd,&buf) != 0) {
		return(0) ;
	}
	return(buf.st_size) ;
}

size_t jr_FileDescDiskSize(fd)
	jr_int fd ;
{
	struct stat buf ;

	if (fstat(fd,&buf) != 0) {
		return(0) ;
	}
	return(buf.st_size) ;
}

size_t jr_FileNameDiskSize(fn)
	const char *	fn ;
{
	struct stat buf ;

	if (stat(fn,&buf) != 0) {
		return(0) ;
	}
	return(buf.st_size) ;
}

off_t jr_FileDescBlockSize (fd)
	jr_int fd;
{
	struct stat buf ;

	if (fstat(fd,&buf) != 0) {
		return(0) ;
	}
	return(buf.st_blksize) ;
}


/*******************************************************
main(argc,argv)
	jr_int argc ;
	char **argv ;
{
	off_t size ;
	FILE *fp ;
	
	argv++ ;
	if ((fp = fopen(*argv,"a")) == NULL) {
		printf("Cant append to %s\n",*argv) ;
		exit(1) ;
	}
	size = (off_t) fsize(fp) ;
	printf ("Size of %s is %d.\n",*argv, size);
	fseek(fp,0,0) ;
	fprintf(fp,"start") ;
	fseek(fp,(size - strlen("finish\n"))+1,0) ;
	fprintf(fp,"finish\n") ;
	printf("%s : munged.\n",*argv) ;
}
*******************************************************/
