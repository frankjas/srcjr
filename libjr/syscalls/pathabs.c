#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <unistd.h>

#include "jr/syscalls.h"
#include "jr/string.h"
#include "jr/error.h"

jr_int jr_FileNameToAbsolutePath(result, result_length, filename)
	char *			result ;
	jr_int			result_length;
	const char *	filename ;
{
	jr_int			status;
	/*
	 * This function is supposed to fill up its
	 * result argument with the absolute path of its
	 * filename argument.
	 */

	if ((filename[0] == '.') && (filename[1] == '/')) {
		filename += 2 ;
	}

	if (*filename == '/') {
		strncpy(result, filename, result_length - 1) ;
		result [result_length - 1] = 0;
	}
	else {
		char	dirname_buffer[2048] ;
		jr_int	nbytes ;
		char *	end_ptr ;

		status	= jr_getcwd (dirname_buffer, sizeof (dirname_buffer), 0);

		if (status != 0) {
			return(-1) ;
		}

		nbytes = result_length - 1 ;
		sprintf(result, "%.*s", nbytes, dirname_buffer) ;
		end_ptr = strchr(result, 0) ;

		nbytes = result_length - (end_ptr - result) - 2 ;  /* null and / */
		if (nbytes <= 0) return(-1) ;

		sprintf(end_ptr, "/%.*s", nbytes, filename) ;
	}

	jr_RemoveContiguousDuplicateChar(result, '/') ;

	{
		char * end_ptr = strrchr(result, '/') ;
		if (end_ptr && (end_ptr[1] == 0)) *end_ptr = 0 ;	/* remove trailing '/' */
	}

	return(0) ;
}
