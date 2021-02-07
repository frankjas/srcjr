#include "tst_ibuf.h"

char *ProgramName;

void main (argc, argv)
	jr_int argc;
	char **argv;
{
	jr_IB_InputBuffer input_buffer [1];
	char buffer [1024];
	jr_int c;


	ProgramName = *argv;

	argv = ArgInterp (argv);

	jr_IB_InputBufferInit (input_buffer);

	jr_IB_OpenStringContext (
		input_buffer,
		"myfile.c", jr_IB_SAVE_NAME,
		"hello this X is a test\n", jr_IB_DONT_FREE_STR
	);


	while ( (c = jr_IB_getc (input_buffer)) != EOF) {
		putc (c, stderr);

		if (c == 'X') {
			jr_IB_ungetc (input_buffer, 'A');
			jr_IB_ungetc (input_buffer, 'B');

			jr_IB_ungetstring (input_buffer, "ungotten string");

			jr_IB_OpenStringContext (
				input_buffer,
				"myfile.h", jr_IB_SAVE_NAME,
				"\nInserted Y String\n", jr_IB_DONT_FREE_STR
			);
		}
		if (c == 'Y') {
			jr_IB_ungetc (input_buffer, 'C');
			jr_IB_ungetc (input_buffer, 'D');

			jr_IB_OpenFileContext (
				input_buffer,
				"mymacro for myfile.h", jr_IB_SAVE_NAME,
				stdin, jr_IB_DONT_FCLOSE_RFP
			);
		}
		if (c == '\n') {
			fprintf (stderr, "\n%s: ",
				jr_IB_ContextDescription (
					input_buffer, buffer, sizeof (buffer)
				)
			);
		}
	}

	jr_IB_InputBufferUndo (input_buffer);

	if (jr_do_trace (jr_malloc_stats)) {
		jr_malloc_stats (stderr, "FSA Test- After Undo");
	}
	if (jr_do_trace (jr_malloc_calls)) {
		jr_malloc_dump ();
	}
	exit (0);
}


CommandLineInfo ArgvFlags[1] = {
	{
		0,           /* numeric */
		"unknown",   /* printer_name */
		1,           /* diag */
		0,           /* other */
	}
} ;

/*
 * All fields in ArgvFlags are guaranteed
 * to start as zero since it is a global variable unless
 * we use aggregate initialization.
 */

char **ArgInterp(argv)
	char **argv ;
{
	register char *curarg ;

	for (argv++; *argv ; argv++) {
		curarg = *argv ;

		if (curarg[0] != '-' ) break ;
		if (curarg[1] == '\0') break ;
		/*
		 * if the curarg doesn't start with a dash, or is
		 * a dash by itself, then return.
		 */
		argv = DashFlagInterp(curarg, argv) ;
	}

	return(argv) ;
}

/* .bp */

char **DashFlagInterp(curarg, argv)
	char  *  curarg ;
	char **  argv ;
{
	for (curarg++; *curarg ; curarg++) {

		switch (*curarg) {

			case 'd':  /* BOOLEAN TOGGLE FLAG */ {

				ArgvFlags->diag = (! ArgvFlags->diag) ;
				break ;
			}

			case 'P': /* STRING VALUED FLAG */ {

				if (curarg[1]) curarg++ ;
				else {
					/* if next char null, get next arg */
					argv++ ;
					curarg = *argv;
				}

				if ((! curarg) || (! *curarg)) {
					UsageError("-P option expects additional printer name") ;
				}

				strcpy(ArgvFlags->printer_name, curarg) ;

				return(argv) ;
			}

			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
			case '8': case '9':  /* NUMERIC FLAG */ {

				ArgvFlags->numeric = atoi(curarg) ;

				while(isdigit(curarg[1])) curarg++ ;

				break ;
			}

			case 'x': UsageError("") ;
			default : UsageError(curarg) ;

			/* .bp */

			case 't' : /* SUBARGUMENTS */ {
				/*
				 * set various traces, sample usage:
				 * % myprog -tm  -t M  -t2m  -t 3M
				 */
				jr_int level = 1 ;

				if (curarg[1]) curarg++ ;
				else {
					/* if next char null, get next arg */
					argv++ ;
					curarg = *argv;
				}

				if ((! curarg) || (! *curarg)) {
					UsageError (
						"-t option expects additional trace specifier"
					) ;
				}

				for ( ; *curarg; curarg++) {
					switch (*curarg) {
						case '0' : case '1' : case '2' : case '3' :
						case '4' : case '5' : case '6' : case '7' :
						case '8' : case '9' : {
							level = atoi (curarg) ;

							while (isdigit (curarg[1])) curarg++ ;
							break ;
						}

						case 'm' :
							jr_set_trace_level (jr_malloc_stats, level);
							break;

						case 'M' :
							jr_set_trace_level (jr_malloc_calls, level);
							break;
						case 'T' :
							jr_set_trace_level (jr_malloc_trap, level);
							break;
						default :	UsageError (*argv);
					} /* switch for -t sub args */
					if (jr_do_trace (jr_malloc_calls)) {
						jr_malloc_set_max_request_size (4 * 1024 * 1024);
					}
					else {
						jr_malloc_set_max_request_size (jr_INT_MAX);
					}
				} /* for -t sub args */

				return (argv) ;

			}  /* end  case 't' with subarguments */

		}  /* end  switch (*curarg) */
	}  /* end  for ( ; *curarg; curarg++) */

	return(argv) ;
}


void UsageError (str)
	char *str;
{
	if (*str) {
		fprintf (stderr, "\n%s : incorrect usage: \"%s\" \n",
			ProgramName, str
		) ;
	}

	fprintf (stderr,
		"\nUsage: %% %s [-P printer] [-t 2mdMT] [-50] [-d] files\n",
		ProgramName
	) ;

	fprintf (stderr, "\n") ;

	fprintf(stderr,"\t-P string      : string valued option,          default '%s'\n",
		ArgvFlags->printer_name
	) ;
	fprintf(stderr,"\t-50            : numeric argument,              default '%d'\n",
		ArgvFlags->numeric
	) ;
	fprintf(stderr,"\t-d             : diagnostic mode,               default '%s'\n",
		ArgvFlags->diag ? "on" : "off"
	) ;
	fprintf(stderr,"\t-t <trace>     : trace, precede with level\n") ;
	fprintf(stderr,"\t   m emory     : jr_malloc_stats usage statistics kept\n") ;
	fprintf(stderr,"\t   M alloc     : jr_malloc calls tracked\n") ;
	fprintf(stderr,"\t   T rap       : jr_malloc calls trapped\n") ;


	fprintf (stderr, "\n") ;

	exit(-1) ;
}

