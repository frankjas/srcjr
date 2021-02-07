#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/misc.h"
#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/trace.h"

#include "jr/syscalls.h"

void UsageError(s)
	char *s ;
{
	fprintf(stderr, "%s\n\n",s) ;
	fprintf(stderr, "Usage:  %% jr_FileNameFindInPathString <filename>\n") ;
	exit(-1) ;
}

void main(argc,argv)
	jr_int argc ;
	char **argv ;
{
	char *	filename ;
	char *	path_string = getenv("PATH") ;
	char	buf[128] ;
	jr_int	status ;

	if (argc < 2) UsageError("Expected arguments") ;

	filename = argv[1] ; 

	jr_set_trace(jr_malloc_stats) ;

	*buf = 0 ;

	printf("Looking for '%s' in '%s'.\n", filename, path_string) ;
	status = jr_FileNameFindInPathString(file_name, path_string, buf, sizeof(buf));

	printf("Status        = %d\n", status) ;
	printf("Pathname      = %s\n", buf) ;

	if (jr_do_trace(jr_malloc_stats)) {
		jr_malloc_stats(stdout, "After FileName Path search") ;
	}

	exit(0) ;
}

