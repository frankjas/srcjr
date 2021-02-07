#define _POSIX_SOURCE 1

#include "ezport.h"

#include "jr/syscalls.h"

char *			ProgramName;


void main (argc, argv)
	jr_int			argc;
	char **			argv;
{
	char *			command_vector[32];
	jr_int			i;

	char			line_buffer[256];
	FILE *			fp_array[3];

	pid_t			childs_pid;

	jr_int			status;


	ProgramName				= argv[0];

	i = 0;
	command_vector[i++]		= "cat";
	command_vector[i++]		= 0;


	childs_pid = jr_Invoke2WayCommandVector (command_vector, fp_array, error_buf);

	if (childs_pid < 0) {
		fprintf (stderr, "%s: couldn't invoke command '%s': %s\n",
			ProgramName, command_vector[0], error_buf
		);
		exit (1);
	}

	while (fgets (line_buffer, sizeof (line_buffer), stdin)  !=  NULL) {
		fprintf (fp_array[0], "%s", line_buffer);
	}
	fclose (fp_array[0]);

	while (fgets (line_buffer, sizeof (line_buffer), fp_array[1])  !=  NULL) {
		fprintf (stdout, "Read: %s", line_buffer);
	}
	exit (0);
}
