#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "jr/string.h"
#include "jr/alist.h"
#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/syscalls.h"
#include "jr/error.h"

#define MAX_LINE		1024

char *jr_NewStringFilteredFromCommandVector (argv, input_to_filter_str, error_buf)
	char **			argv;
	const char *	input_to_filter_str;
	char *			error_buf;
{
	const char *	begin_ptr;
	jr_int			i;
	jr_int			line_length;
	jr_int			nbytes;
	jr_int			flags;
	jr_int			wait_status;
	char			output_buffer [MAX_LINE];
	char *			new_str;
	FILE *			pipe_fps [2];
	jr_int			rfd;
	jr_int			wfd;
	jr_AList		output_string [1];

	pid_t			childs_pid;



	childs_pid = jr_Invoke2WayCommandVector (argv, pipe_fps, error_buf);

	if (childs_pid < 0) {
		jr_esprintf (error_buf, "couldn't spawn '%s': %s", argv[0], error_buf);
		return (0);
	}

	rfd = fileno (pipe_fps [1]);
	wfd = fileno (pipe_fps [0]);

	flags = fcntl (rfd, F_GETFL, 0);
	flags |= O_NDELAY;
	fcntl (rfd, F_SETFL, flags);

	jr_AListInit (output_string, sizeof (char));


	for (begin_ptr = input_to_filter_str, i = 0;  begin_ptr [i];  i++) {

		if (begin_ptr [i] == '\n') {
			line_length = i + 1;
			nbytes = write (wfd, (char *) begin_ptr, line_length);

			if (nbytes != line_length) {
				jr_esprintf (error_buf, "couldn't write(%s:%d, %p, %d) return of %d : %s",
					argv[0], wfd, begin_ptr, line_length, nbytes, strerror(errno)
				);
				goto return_bad;
			}

			while (1) {
				nbytes = read (rfd, output_buffer, sizeof (output_buffer));

				if (nbytes == -1) {
					if (errno == EWOULDBLOCK) break;    /* non-posix errno */
					if (errno == EAGAIN) break;

					jr_esprintf (error_buf,
						"couldn't read from '%s': %s", argv [0], strerror (errno)
					);
					goto return_bad;
				}
				if (nbytes == 0) {
					break;
				}
				{
					jr_int i;

					for (i=0;  i < nbytes;  i++) {
						jr_AListNativeSetNewTail (output_string, output_buffer [i], char);
					}
				}
			}
			begin_ptr += line_length;
			i = -1;
			/*
			 * so next time through the loop i will be 0
			 */
		}
	}


	fclose (pipe_fps [1]);
	/*
	 * send eof to command
	 */


	flags = fcntl (rfd, F_GETFL);
	flags &= ~O_NDELAY;
	fcntl (rfd, F_SETFL, flags);

	/*
	 * set up blocking io to wait for eof
	 */

	while (1) {
		nbytes = read (rfd, output_buffer, sizeof (output_buffer));

		if (nbytes == -1) {
			jr_esprintf (error_buf, "couldn't read from '%s': %s", argv [0], strerror (errno));
			goto return_bad;
		}
		if (nbytes == 0) {
			break;
		}
		{
			jr_int i;

			for (i=0;  i < nbytes;  i++) {
				jr_AListNativeSetNewTail (output_string, output_buffer [i], char);
			}
		}
	}
	jr_AListNativeSetNewTail (output_string, 0, char);

	if (1) {
		new_str = jr_strdup (jr_AListHeadPtr (output_string));
	}
	else return_bad : {
		kill (childs_pid, SIGKILL);

		new_str	= 0;
	}

	jr_AListUndo (output_string);

	waitpid (childs_pid, &wait_status, 0);

	fclose (pipe_fps [0]);
	/*
	 * Do this after waitpid() so the child won't get SIGPIPE
	 */

	if (WIFEXITED (wait_status)  &&  WEXITSTATUS (wait_status) != 0) {
		if (new_str) {
			jr_free (new_str);
		}

		jr_esprintf (error_buf, "'%s' exited with bad status %d", argv[0], WEXITSTATUS (wait_status));
		return (0);
	}
	if (WIFSIGNALED (wait_status)  &&  WTERMSIG (wait_status) != SIGKILL) {
		if (new_str) {
			jr_free (new_str);
		}

		jr_esprintf (error_buf, "'%s' was terminated by a signal%s",
			argv[0], WIFCORE (wait_status) ? " and dumped core" : ""
		);
		return (0);
	}
	return new_str;
}


#ifdef _test_filter_string

void main (argc, argv)
	jr_int argc;
	char **argv;
{
	char test_string [512];
	char error_buf [512];
	char *sed_string;
	jr_int status;


	jr_set_trace (jr_malloc_stats);

	sprintf (test_string, "hello there everyone\n");

	sed_string = jr_FilterStringWithCommandVector (argv + 1, test_string, error_buf);

	if (sed_string == 0) {
		fprintf (stderr, "Couldn't filter the string: %.150s\n", error_buf);
	}
	else {
		printf ("Original string: '%s'\n", test_string);
		printf ("Filtered string: '%s'\n", sed_string);
	}

	jr_free (sed_string);

	jr_malloc_stats (stderr, "After UndoSedType");
}

#endif
