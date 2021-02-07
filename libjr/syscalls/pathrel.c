#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <stdlib.h>
#include <alloca.h>

#include "jr/syscalls.h"
#include "jr/string.h"

void jr_FileNameToRelativePath(target_path_buf, buflen, source_path_arg, relative_to_here_arg)
	char *			target_path_buf ;
	jr_int			buflen ;
	const char *	source_path_arg ;
	const char *	relative_to_here_arg ;
{
	char *	source_abspath		= alloca(buflen) ;
	char *	relative_to_here	= alloca(buflen) ;
	jr_int	skip_length ;

	/* expand and adjust the two path arguments to be complete path names */ {
		char *	expanded_source_path = alloca(buflen*2) ;
		char *	expanded_relative_to_here = alloca(buflen*2) ;

		jr_PathStringCopyWithDollarAndTildeExpansion(expanded_source_path, source_path_arg) ;
		jr_PathStringCopyWithDollarAndTildeExpansion(expanded_relative_to_here,relative_to_here_arg) ;

		jr_FileNameToAbsolutePath(source_abspath,	buflen-2, expanded_source_path) ;
		jr_FileNameToAbsolutePath(relative_to_here,	buflen-2, expanded_relative_to_here) ;
	}

	strcat(relative_to_here, "/") ;
	jr_RemoveContiguousDuplicateChar(relative_to_here, '/') ;
	jr_RemoveContiguousDuplicateChar(source_abspath, '/') ;

	skip_length = strlen(relative_to_here) ;
	if (strncmp(source_abspath, relative_to_here, skip_length) == 0) {
		/*
		 * if the leading section of the source path contains the relative_to_here path
		 * then we want to skip over the relative to here path and just get
		 * the trailing relative path then.
		 */
		strncpy(target_path_buf, source_abspath + skip_length, buflen-1) ;
		target_path_buf[buflen-1] = 0 ;
	}
	else {
		/*
		 * If the source path does not start with the relative_to_here path
		 * then the source and relative_to arguments were not even
		 * part of the same directory hierarchy and so we can't
		 * really adjust anything, in this case the abspath is put
		 * into the result.
		 */
		strncpy(target_path_buf, source_abspath, buflen-1) ;
		target_path_buf[buflen-1] = 0 ;
	}
}

