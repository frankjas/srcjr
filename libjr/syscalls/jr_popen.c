#include "ezport.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "jr/syscalls.h"
#include "jr/error.h"


FILE *jr_popen (
	const char *			command_str,
	const char *			mode_str,
	char *					error_buf)
{
	FILE *					fp;

	fp = popen (command_str, mode_str);

	if (fp == 0) {
		jr_esprintf (error_buf, "%s", strerror (errno));
		return 0;
	}
	return fp;
}

jr_int jr_pclose (
	FILE *					fp,
	char *					error_buf)
{
	jr_int					status;

	status	= pclose (fp);

	if (WIFEXITED (status)) {
		jr_esprintf (error_buf, "exit status was %d", WEXITSTATUS (status));
		return WEXITSTATUS (status);
	}
	if (WIFSIGNALED (status)) {
		jr_esprintf (error_buf, "process was killed with %d", WTERMSIG (status));
		return jr_EXEC_ERROR;
	}
	if (WIFSTOPPED (status)) {
		jr_esprintf (error_buf, "process was stopped");
		return jr_EXEC_ERROR;
	}

	jr_esprintf (error_buf, "no exit status, not stopped or killed");
	return jr_EXEC_INTERNAL_ERROR;
}
