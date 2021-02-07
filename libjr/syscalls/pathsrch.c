#define _POSIX_SOURCE 1

#include "ezport.h"

#include <unistd.h>
#include <string.h>

#include "jr/syscalls.h"
#include "jr/malloc.h"


char *jr_PathSearch (file_name, dir_vector)
	char *				file_name;
	char **				dir_vector;
{
	char *				file_path;
	char *				curr_dir;
	jr_int				i;

	jr_int				status;


	for (i=0;  dir_vector[i];  i++) {
		curr_dir	= dir_vector[i];

		file_path	= jr_malloc (strlen (curr_dir) + 1 + strlen (file_name) + 1);

		sprintf (file_path, "%s/%s", curr_dir, file_name);

		status		= access (file_path, R_OK);

		jr_free (file_path);

		if (status == 0) {
			return curr_dir;
		}
	}

	return 0;
}
