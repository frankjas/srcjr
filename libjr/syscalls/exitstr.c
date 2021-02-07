#define _POSIX_SOURCE 1

#include "ezport.h"

#include <sys/wait.h>

#include "jr/syscalls.h"

char *jr_ExitStatusToString (status_buf, status)
	char *		status_buf;
	jr_int		status;
{
	if (WIFEXITED(status)) {
		sprintf(status_buf, "exit status: %d", WEXITSTATUS(status)) ;
	}
	if (WIFSTOPPED(status)) {
		sprintf(status_buf, "stop signal: %d", WSTOPSIG(status)) ;
	}
	if (WIFSIGNALED(status)) {
		sprintf(status_buf, "terminated by signal: %d%s",
			WTERMSIG(status),
			WIFCORE(status) ? " (core dumped)" : ""
		) ;
	}

	return status_buf;
}
