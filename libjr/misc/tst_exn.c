#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/exceptn.h"
#include "jr/string.h"

#define MAX_WORDS 32

void main (argc, argv)
	jr_int		argc;
	char **		argv;
{
	char		line_buf	[256];
	char		error_buf [jr_ERROR_LEN];
	char *		words		[MAX_WORDS];
	char *		prompt_str					= argv[0];
	jr_int		len;

	fprintf (stderr, "Running with one argument turns on 'jr_malloc_trap'\n");
	if (argc == 2) {
		jr_set_trace (jr_malloc_trap);
	}
	else {
		jr_set_trace (jr_malloc_calls);
	}
	jr_set_trace (jr_malloc_stats);

	fprintf (stderr, "%s> ", prompt_str);

	while (fgets (line_buf, sizeof (line_buf), stdin)) {
		*strchr (line_buf, '\n') = 0;
		len = jr_VectorizeIntoWordsInPlace (line_buf, " \t", words, MAX_WORDS);

		switch (*words[0]) {
			case 'c' : {
				if (len < 2) {
					fprintf (stderr, "\t'catch' requires additional exception name argument\n");
					break;
				}

				if (jr_ExceptionCatch (words[1], error_buf)) {
					fprintf (stderr, "Caught exception '%s': %s\n",
						words[1], error_buf
					);
				}
				break;
			}
			case 'r' : {
				if (len < 2) {
					fprintf (stderr, "\t'raise' requires additional exception name argument\n");
					break;
				}
				jr_ExceptionRaise (words[1], "dummy message");
				break;
			}
			case 'u' : {
				if (len < 2) {
					fprintf (stderr, "\t'uncatch' requires additional exception name argument\n");
					break;
				}
				jr_ExceptionUnCatch (words[1]);
				break;
			}
			case 'p' : {
				char *	exception_name;
				jr_int	index;

				jr_ExceptionForEachName (index, exception_name) {
					fprintf (stderr, "\t%s\n", exception_name);
				}
				break;
			}
			case 'h' :
			case '?' : {
				fprintf (stderr, "\n");
				fprintf (stderr, "\tcatch <exception name>\n");
				fprintf (stderr, "\tprint all\n");
				fprintf (stderr, "\traise <exception name>\n");
				fprintf (stderr, "\tuncatch <exception name>\n");
				fprintf (stderr, "\thelp\n");
				break;
			}
			default : {
				fprintf (stderr, "Unrecognized command: %s\n", words[0]);
				break;
			}
		}
		fprintf (stderr, "%s> ", prompt_str);
	}

	jr_malloc_dump ();
	jr_malloc_stats (stderr, argv[0]);
}
