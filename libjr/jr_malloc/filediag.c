#include "project.h"

#ifndef missing_file_descriptors
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef missing_file_descriptors

void jr_MallocDiagInitExemptDescriptorArray (mdp)
	jr_MallocDiagStruct *	mdp;
{
}

jr_int jr_MallocDiagNumOpenFiles (mdp, wfp, error_buf)
	jr_MallocDiagStruct *	mdp;
	FILE *					wfp;
	char *					error_buf;
{
	return 0;
}

#else

void jr_MallocDiagInitExemptDescriptorArray (mdp)
	jr_MallocDiagStruct *	mdp;
{
	jr_int						num_fds;
	jr_int						i;
	jr_int *					fd_array;


	num_fds						= sysconf (_SC_OPEN_MAX);
	fd_array					= jr_malloc (num_fds * sizeof (jr_int));


	for (i=0;  i < num_fds; i++) {
		fd_array [i] = open ("/", O_RDONLY);

		if (fd_array[i]  >=  0   &&  fd_array[i] != i) {
			/*
			 * file descriptor is in use
			 */
			close (fd_array [i]);
			fd_array [i] = -1;
		}
	}

	for (i=0;  i < num_fds; i++) {
		if (fd_array[i] == -1) {
			fd_array[i]		= 1;
		}
		else {
			/*
			 * these file descriptors weren't in use
			 */
			close (fd_array [i]);
			fd_array[i]		= 0;
		}
	}
	mdp->exempt_fd_array		= fd_array;
}

jr_int jr_MallocDiagNumOpenFiles (mdp, wfp, error_buf)
	jr_MallocDiagStruct *	mdp;
	FILE *					wfp;
	char *					error_buf;
{
	jr_int					num_fds					= sysconf (_SC_OPEN_MAX);
	jr_int					num_open_files			= 0;
	jr_int *				fd_array;
	jr_int					i;
	jr_int					first_bad_fd			= 1;

	char					tmp_buf [32];


	if (!mdp->did_initialization) {
		return 0;
	}
	fd_array = (jr_int *) malloc (num_fds * sizeof (jr_int));

	for (i=0;  i < num_fds; i++) {
		fd_array [i] = open ("/", O_RDONLY);

		if (fd_array[i] >= 0   &&  fd_array[i] != i) {
			if (! mdp->exempt_fd_array [i]) {
				num_open_files ++;

				if (wfp) {
					fprintf (wfp, "File descriptor %d still in use\n", i);
				}
				if (error_buf) {
					if (first_bad_fd) {
						sprintf (tmp_buf, "%d", i);
						error_buf [0]	= 0;
						first_bad_fd	= 0;
					}
					else {
						sprintf (tmp_buf, ", %d", i);
					}
					if (strlen (error_buf)  +  strlen (tmp_buf)  <  256 - 1) {
						strcat (error_buf, tmp_buf);
					}
				}
			}
			close (fd_array [i]);
			fd_array [i] = -1;
		}
	}
	for (i=0;  i < num_fds; i++) {
		if (fd_array[i] != -1) {
			close (fd_array [i]);
		}
	}
	free (fd_array);

	return num_open_files;
}

#endif

