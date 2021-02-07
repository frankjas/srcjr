#include "ezport.h"

#include <string.h>

#include "jr/io.h"
#include "jr/malloc.h"

jr_int jr_GetWholeLine (line_buf_ref, line_buf_size_ref, rfp)
	char **				line_buf_ref;
	jr_int *			line_buf_size_ref;
	FILE *				rfp;
{
	char *				line_buf			= *line_buf_ref;
	jr_int				line_buf_size		= *line_buf_size_ref;
	jr_int				data_length;


	if (line_buf_size < 2) {
		return 0;
	}


	line_buf[line_buf_size - 2]	= '\n';
	/*
	** 8/17/06: this is where the newline should be if the whole line is read.
	** If it's not a '\n', then it was overwritten the end of a partial line.
	*/

	if (fgets (line_buf, line_buf_size, rfp)  ==  NULL) {
		return -1;
	}

	while (line_buf[line_buf_size - 2] != '\n') {
		data_length					= line_buf_size - 1;
		line_buf_size				*= 2;
		line_buf					= jr_realloc (line_buf, line_buf_size);

		line_buf[line_buf_size - 2]	= '\n';
		/*
		** 8/17/06: this is where the newline should be if the whole line is read.
		** If it's not a '\n', then it was overwritten the end of a partial line.
		*/

		if (fgets (line_buf + data_length, line_buf_size - data_length, rfp)  ==  NULL) {
			break;
		}
	}

	*line_buf_ref		= line_buf;
	*line_buf_size_ref	= line_buf_size;

	return 0;
}
