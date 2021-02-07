#define _POSIX_SOURCE 1

#include "ezport.h"

#include <sys/wait.h>

#include "jr/syscalls.h"

void jr_SigChildHandler (signum)
	jr_int		signum;
{
	while (waitpid(-1, 0, WNOHANG) > 0);
}
