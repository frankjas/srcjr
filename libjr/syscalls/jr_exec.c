#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdlib.h>
#include <stdio.h>

#include "jr/syscalls.h"
#include "jr/error.h"

jr_int jr_exec_cmd (
	const char *			command_str,
	char *					error_buf)
{
	jr_int					status;

	status	= system (command_str);

	if (status < 0) {
		jr_esprintf (error_buf, "couldn't spawn");
		return jr_EXEC_SPAWN_ERROR;
	}

	if (WIFEXITED (status)) {
		jr_esprintf (error_buf, "exit status was %d", WEXITSTATUS (status));
		return WEXITSTATUS (status);
	}
	if (WIFSIGNALED (status)) {
		jr_esprintf (error_buf, "process was killed by signal %d (%.64s%.32s)",
			WTERMSIG (status), jr_SignalName (WTERMSIG (status)),
			WIFCORE (status) ? " core dumped" : ""
		);
		return jr_EXEC_ERROR;
	}
	if (WIFSTOPPED (status)) {
		jr_esprintf (error_buf, "process was stopped");
		return jr_EXEC_ERROR;
	}

	jr_esprintf (error_buf, "no exit status, not stopped or killed");
	return jr_EXEC_INTERNAL_ERROR;
}

