#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "jr/syscalls.h"
#include "jr/malloc.h"
#include "jr/error.h"

jr_int jr_Invoke1WayCommandVector (argv, rfp_ptr, error_buf)
	char **		argv ;
	FILE **		rfp_ptr;
	char *		error_buf;
{
	jr_int		parent_rfd;
	jr_int		childs_wfd;
	jr_int		childs_pid ;
	jr_int		datapath[2] ;
	FILE *		rfp ;

	jr_int		status;


	status = pipe(datapath);
	if (status != 0) {
		jr_esprintf (error_buf, "couldn't create pipe: %s", strerror (errno));
		return -1;
	}

	parent_rfd = datapath[0] ;
	childs_wfd = datapath[1] ;

	childs_pid = fork();

	if (childs_pid < 0) {
		close (parent_rfd);
		close (childs_wfd);

		jr_esprintf (error_buf, "couldn't spawn process: %s", strerror (errno));
		return -1;
	}

	if (childs_pid == 0) {
		close(parent_rfd) ;

		if (childs_wfd != STDOUT_FILENO) {
			jr_int newfd ;

			newfd = dup2 (childs_wfd, STDOUT_FILENO) ;
			if (newfd != STDOUT_FILENO) {
				fprintf (stderr,
			"jr_Invoke1WayCommandVector() child '%s': couldn't redirect stdout: dup2() returned %d\n",
					argv[0], newfd
				);
				exit (1);
			}
			close(childs_wfd) ;
		}
		status = execvp(argv[0], argv);

		if (status != 0) {
			char buf[512] ;

			sprintf(buf, "jr_Invoke1WayCommandVector() child: couldn't execvp(%s) ", argv[0]) ;
			perror(buf) ;
			_exit(errno) ;
		}
	}

	close(childs_wfd);

	rfp = fdopen(parent_rfd, "r") ;

	if (rfp == NULL) {
		jr_esprintf (error_buf, "couldn't open child's stdout: %s", strerror (errno));
		kill (childs_pid, SIGKILL);

		status = waitpid (childs_pid, 0, 0);

		return -1;
	}
	*rfp_ptr	= rfp;

	return childs_pid;
}

jr_int jr_Invoke1WayCommandVectorWithStderr (argv, rfp_ptr, error_buf)
	char **		argv ;
	FILE **		rfp_ptr;
	char *		error_buf;
{
	jr_int		parent_rfd;
	jr_int		childs_wfd;
	jr_int		childs_pid ;
	jr_int		datapath[2] ;
	FILE *		rfp ;

	jr_int		status;


	status = pipe(datapath);
	if (status != 0) {
		jr_esprintf (error_buf, "couldn't create pipe: %s", strerror (errno));
		return -1;
	}

	parent_rfd = datapath[0] ;
	childs_wfd = datapath[1] ;

	childs_pid = fork();

	if (childs_pid < 0) {
		close (parent_rfd);
		close (childs_wfd);

		jr_esprintf (error_buf, "couldn't spawn process: %s", strerror (errno));
		return -1;
	}

	if (childs_pid == 0) {
		jr_int newfd ;

		close(parent_rfd) ;

		if (childs_wfd != STDOUT_FILENO) {
			newfd = dup2 (childs_wfd, STDOUT_FILENO) ;
			if (newfd != STDOUT_FILENO) {
				fprintf (stderr,
	"jr_Invoke1WayCommandVector() child '%s': couldn't redirect stdout: dup2() returned %d != %d\n",
					argv[0], newfd, STDOUT_FILENO
				);
				exit (1);
			}
		}

		newfd = dup2 (childs_wfd, STDERR_FILENO);

		if (newfd != STDERR_FILENO) {
			fprintf (stderr,
	"jr_Invoke1WayCommandVector() child '%s': couldn't redirect stderr: dup2() returned %d != %d\n",
				argv[0], newfd, STDERR_FILENO
			);
			exit (1);
		}

		if (childs_wfd != STDOUT_FILENO) {
			close(childs_wfd) ;
		}

		status = execvp(argv[0], argv);

		if (status != 0) {
			char buf[512] ;

			sprintf(buf, "jr_Invoke1WayCommandVector() child: couldn't execvp(%s) ", argv[0]) ;
			perror(buf) ;
			_exit(errno) ;
		}
	}

	close(childs_wfd);

	rfp = fdopen(parent_rfd, "r") ;

	if (rfp == NULL) {
		jr_esprintf (error_buf, "couldn't open child's stdout: %s", strerror (errno));
		kill (childs_pid, SIGKILL);

		status = waitpid (childs_pid, 0, 0);

		close (parent_rfd);

		return -1;
	}
	*rfp_ptr	= rfp;

	return childs_pid;
}


jr_int jr_InvokeCommandVector (argv, fp_array, num_fps, error_buf)
	char **			argv ;
	FILE *			fp_array[] ;
	jr_int			num_fps;
	char *			error_buf;
{
	pid_t			childs_pid;
	jr_int *		fd_array;
	jr_int			i;

	jr_int			parents_fd			= 0;		/* to shutup 'uninit.' msg */
	jr_int			childs_fd			= 0;

	jr_int			status;


	if (num_fps > 3) {
		jr_esprintf (error_buf, "too many pipes (%d), max is %d", num_fps, 3);
		return -1;
	}

	fd_array		= jr_malloc (2 * num_fps * sizeof (jr_int));

	for (i=0;  i < num_fps;  i++) {
		status = pipe (fd_array + 2 * i);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't open pipe: %s", strerror (errno));
			jr_free (fd_array);
			return -1;
		}
		fp_array[i] = 0;
	}


	childs_pid = fork ();

	if (childs_pid < 0) {
		jr_esprintf (error_buf, "couldn't spawn process: %s", strerror (errno));
		goto return_bad;
	}

	for (i = 0;  i < num_fps;  i++) {
		switch (i) {
			case STDIN_FILENO		: {
				childs_fd		= fd_array [2 * i];
				parents_fd		= fd_array [2 * i + 1];
				break;
			}
			case STDOUT_FILENO		:
			case STDERR_FILENO		: {
				childs_fd		= fd_array [2 * i + 1];
				parents_fd		= fd_array [2 * i];
				break;
			}
		}

		if (childs_pid == 0) {
			close (parents_fd);

			if (childs_fd != i) {
				status = dup2 (childs_fd, i);

				if (status != i) {
					fprintf (stderr,
			"jr_InvokeCommandVector() child '%s': couldn't redirect: dup2() returned %d != %d\n",
						argv[0], status, i
					);
					exit (1);
				}

				close(childs_fd) ;
			}
		}
		else {
			close (childs_fd);

			if (i == STDIN_FILENO) {
				fp_array[i]	= fdopen (parents_fd, "w");
			}
			else {
				fp_array[i]	= fdopen (parents_fd, "r");
			}
			if (fp_array[i]  ==  NULL) {
				jr_esprintf (error_buf, "couldn't open child's %s: %s",
					i == STDIN_FILENO ? "stdin" : i == STDOUT_FILENO ? "stdout" : "stderr",
					strerror (errno));

				kill (childs_pid, SIGKILL);

				status = waitpid (childs_pid, 0, 0);

				goto return_bad;
			}
		}
	}

	if (childs_pid  ==  0) {

		status = execvp(argv[0], argv);

		if (status != 0) {
			fprintf (stderr, "jr_InvokeCommandVector() child: couldn't execvp(%s): %s\n",
				argv[0], strerror (errno)
			) ;
			fflush (stderr);
			_exit(errno) ;
		}
	}



	if (0) return_bad : {
		childs_pid		= -1;

		for (i = 0;  i < num_fps;  i++) {
			close (fd_array[i]);

			if (fp_array[i]) {
				fclose (fp_array[i]);
			}
		}
	}

	jr_free (fd_array);

	return childs_pid;
}
