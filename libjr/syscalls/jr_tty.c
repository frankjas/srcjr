#include "ezport.h"

#include <unistd.h>
#include <stdio.h>

jr_int jr_IsInteractiveFilePtr (
	FILE *						fp,
	char *						error_buf)
{
	return isatty (fileno (fp));
}
