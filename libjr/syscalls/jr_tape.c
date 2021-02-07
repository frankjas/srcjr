#include "ezport.h"

#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "jr/syscalls.h"
#include "jr/error.h"


jr_int jr_IsTapeFilePtr (fp, error_buf)
	FILE *						fp;
	char *						error_buf;
{
	struct stat					stat_info[1];

	jr_int						status;


	status	= fstat (fileno (fp), stat_info);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't 'stat' file pointer: %s", strerror (errno));
		return -1;
	}

	return S_ISCHR (stat_info->st_mode);
}

