#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#ifndef missing_unistd_h
#include <unistd.h>
#endif

#include "jr/misc.h"
#include "jr/syscalls.h"

void jr_coredump ()
{
	/*
	** 3/2/07: Can't do a malloc(), since called from inside jr_MallocDiag() code
	*/
	fflush(stdout); 
	fflush(stderr);
	abort();

#	ifdef SIGTRAP
		kill(getpid(), SIGTRAP);
		pause ();
#	endif
}

void jr_debugger_trap ()
{
	fflush(stdout); 
	fflush(stderr);

#	if defined(ostype_winnt)
		__debugbreak();
		/*
		** 6-8-2011: Could also use DebugBreak(), but that requires
		** linking to Kernel32.lib
		*/
#	elif defined(SIGTRAP)
		kill(getpid(), SIGTRAP);
#	else
		abort ();
#	endif
}



char	jr_DiagFileDirectory[256];


char *jr_MakeDiagFileName (diag_file_buf, diag_file_buf_size, file_name)
	char *						diag_file_buf;
	jr_int						diag_file_buf_size;
	const char *				file_name;
{
	/*
	 * Can't do dynamic allocation, since this function is used
	 * as part of the resource allocation bookkeeping.
	 */

	jr_int						str_length;
	jr_int						copy_length;
	jr_int						curr_length				= 0;


	if (jr_DiagFileDirectory[0]) {

		str_length	= (jr_int) strlen (jr_DiagFileDirectory);

		if (diag_file_buf_size - 1 - curr_length  >  str_length) {
			copy_length = str_length;
		}
		else {
			copy_length	= diag_file_buf_size - 1  -  curr_length;
		}

		if (copy_length > 0) {
			strncpy (diag_file_buf + curr_length, jr_DiagFileDirectory, copy_length + 1);
			curr_length		+= copy_length;
		}

		if (curr_length  <  diag_file_buf_size - 1) {
			diag_file_buf[curr_length++]		= '/';
		}
	}


	str_length	= (jr_int) strlen (file_name);


	if (diag_file_buf_size - 1 - curr_length  >  str_length) {
		copy_length = str_length;
	}
	else {
		copy_length	= diag_file_buf_size - 1  -  curr_length;
	}

	if (copy_length > 0) {
		strncpy (diag_file_buf + curr_length, file_name, copy_length + 1);

		curr_length		+= copy_length;
	}


	diag_file_buf[curr_length] = 0;

	return diag_file_buf;
}
