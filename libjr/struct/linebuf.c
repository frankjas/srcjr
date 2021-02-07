#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"
#include "jr/linebuf.h"
#include "jr/misc.h"

void jr_LB_LineBufferInit (line_buffer, max_size, max_lines)
	jr_LB_LineBuffer *	line_buffer;
	jr_int				max_size;
	jr_int				max_lines;
{
	memset (line_buffer, 0, sizeof (*line_buffer));

	if (max_size  <  256) {
		max_size			= 256;
	}

	line_buffer->buffer		= jr_malloc (max_size);
	line_buffer->max_size	= max_size;

	max_lines--;
	if (max_lines < 1) max_lines = 1;

	line_buffer->prev_buffers = jr_malloc (max_lines * sizeof(char *));
	memset (line_buffer->prev_buffers, 0, max_lines * sizeof(char *));

	line_buffer->prev_lines	= jr_malloc (max_lines* sizeof(jr_int));
	line_buffer->max_lines	= max_lines;

	line_buffer->tab_stops	= jr_EnvTabStops ();

	jr_LB_LineBufferReset (line_buffer, 1);
}

void jr_LB_LineBufferReset (line_buffer, line)
	jr_LB_LineBuffer *		line_buffer;
	jr_int					line;
{
	line_buffer->line		= line;
	line_buffer->column		= 0;
	line_buffer->buffer [0]	= 0;

	{
		jr_int i;

		for (i=0; i < line_buffer->max_lines; i++) {
			line_buffer->prev_lines [i] = 0;

			if (line_buffer->prev_buffers [i]) {
				jr_free (line_buffer->prev_buffers [i]);
				line_buffer->prev_buffers [i] = 0;
			}
		}
	}
}

void jr_LB_LineBufferUndo (line_buffer)
	jr_LB_LineBuffer *		line_buffer;
{
	jr_LB_LineBufferReset (line_buffer, 0);

	jr_free (line_buffer->buffer);
	jr_free (line_buffer->prev_buffers);
	jr_free (line_buffer->prev_lines);
}

/* 
 * both line and column can be negative or less than
 * the current position.  In these cases they will be
 * ignored and the str appended with one space
 * separating it from the previous addition
 */

void jr_LB_AddString (line_buffer, str, line, desired_column)
	jr_LB_LineBuffer *		line_buffer;
	char *					str;
	jr_int					line;
	jr_int					desired_column;
{
	jr_int					add_space				= 0;


	if (line < line_buffer->line) {
		/*
		 * ignore line number less than the current
		 */
		line = line_buffer->line;
	}

	if (line > line_buffer->line) {
		jr_LB_SetCurrentLine (line_buffer, line);
	}

	if (desired_column <= line_buffer->column) {
		/* 
		 * The column info is messed up.  This means
		 * we need to put a space before the str if
		 * there isn't one already
		 */
		if (	line_buffer->column > 0
			&&	line_buffer->buffer [line_buffer->column - 1]  !=  ' ') {
			jr_LB_AddChar (line_buffer, ' ');
		}
		desired_column	= jr_LB_CurrentColumn (line_buffer) + 1;
		add_space		= 1;
	}

	/*
	 * add blanks between end of line and column
	 */

	/* Will changed to be easier for debug analysis */ {
		jr_int num_blanks_needed ;
		jr_int	i ;
		
		num_blanks_needed = desired_column - (jr_LB_CurrentColumn(line_buffer) + 1);

		for (i=0; i < num_blanks_needed; i++) {
			jr_LB_AddChar (line_buffer, ' ') ;
		}
	}

	{
		jr_int		i;

		for (i=0;  str[i];  i++) {
			jr_LB_AddChar (line_buffer, str[i]);
		}
	}

	/*
	 * put a space after since an adjacent column on the next
	 * str might cause words to run together
	 */
	if (add_space) {
		jr_LB_AddChar (line_buffer, ' ');
	}
}

void jr_LB_AddChar (line_buffer, c) 
	jr_LB_LineBuffer *		line_buffer;
	jr_int					c;
{
	if (c == '\t') {
		jr_int				spaces_to_tab_stop;
		jr_int				i;

		spaces_to_tab_stop	= line_buffer->tab_stops
							- line_buffer->column % line_buffer->tab_stops;

		for (i=0;  i < spaces_to_tab_stop;  i++) {
			jr_LB_AddChar (line_buffer, ' ');
		}
	}
	else {
		if (line_buffer->column < line_buffer->max_size - 1) {

			line_buffer->buffer [line_buffer->column] = c;
			line_buffer->column ++;

			line_buffer->buffer [line_buffer->column] = 0;
		}
	}
}

void jr_LB_SetPreviousLine (line_buffer, line, buffer)
	jr_LB_LineBuffer *		line_buffer;
	jr_int					line;
	char *					buffer;
{
	jr_int line_index = line % line_buffer->max_lines;

	line_buffer->prev_lines [line_index] = line;

	if (line_buffer->prev_buffers [line_index]) {
		jr_free (line_buffer->prev_buffers [line_index]);

		line_buffer->prev_buffers [line_index] = 0;
	}
	if (buffer) {
		line_buffer->prev_buffers [line_index] = jr_strdup (buffer);
	}
}

void jr_LB_SetCurrentColumn (line_buffer, column)
	jr_LB_LineBuffer *		line_buffer;
	jr_int					column;
{
	if (column  <  line_buffer->max_size) {
		line_buffer->column		= column;
		line_buffer->buffer[column] = 0;
	}
}

void jr_LB_SetCurrentLine (line_buffer, line)
	jr_LB_LineBuffer *		line_buffer;
	jr_int					line;
{

	if (line > line_buffer->line) {
		/*
		 * for every line in between, put in an empty line
		 */
		jr_int i, first_line, line_index;

		if (line - line_buffer->line > line_buffer->max_lines) {
			/*
			 * skipping ahead past what we can record
			 */
			first_line = line - line_buffer->max_lines;
		}
		else {
			/*
			 * put the current line in the prev line buffer
			 */
			jr_LB_SetPreviousLine (line_buffer, line_buffer->line, line_buffer->buffer);

			first_line = line_buffer->line + 1;
		}
		for (i = first_line; i < line; i++) {
			line_index = i % line_buffer->max_lines;

			line_buffer->prev_lines [line_index] = i;

			if (line_buffer->prev_buffers [line_index]) {
				jr_free (line_buffer->prev_buffers [line_index]);

				line_buffer->prev_buffers [line_index] = 0;
			}
		}
		line_buffer->line		= line;
		line_buffer->column		= 0;
		line_buffer->buffer [0]	= 0;

		return;
	}
	 
	if (line < line_buffer->line) {
		jr_int				line_index		= line % line_buffer->max_lines;
		jr_int				current_line	= line_buffer->line;
		jr_int				i;


		if (line_buffer->prev_lines [line_index] != line) {
			/* 
			 * line not in previous lines
			 */
			jr_LB_LineBufferReset (line_buffer, line);
			return;
		}
		/* 
		 * restore line from previous lines
		 */

		strcpy (line_buffer->buffer, line_buffer->prev_buffers [line_index]);

		line_buffer->line		= line;
		line_buffer->column		= (jr_int) strlen (line_buffer->prev_buffers [line_index]);
			
		/*
		 * null out later lines
		 */
		for (i = line; i < current_line; i++) {
			line_index = i % line_buffer->max_lines;
			
			line_buffer->prev_lines [line_index] = 0;

			if (line_buffer->prev_buffers [line_index]) {
				jr_free (line_buffer->prev_buffers [line_index]);

				line_buffer->prev_buffers [line_index] = 0;
			}
		}
		return;
	}
}

void jr_LB_LineBufferCopy (dest, src)
	jr_LB_LineBuffer *		dest;
	jr_LB_LineBuffer *		src;
{
	jr_int i;

	dest->line = src->line;

	if (src->column  >  dest->max_size - 1) {
		dest->column = dest->max_size - 1;
	}
	else {
		dest->column = src->column;
	}
	strncpy (dest->buffer, src->buffer, dest->max_size - 1);
	dest->buffer[dest->max_size - 1] = 0;

	for (i=0; i < src->max_lines && i < dest->max_lines; i++) {
		jr_LB_SetPreviousLine (dest, src->prev_lines[i], src->prev_buffers[i]);
	}
	for (; i < dest->max_lines; i++) {
		jr_LB_SetPreviousLine (dest, 0, 0);
	}
}

char *jr_LB_sprint_lines (str, line_buffer, start_line, end_line, number_format)
	char *					str;
	jr_LB_LineBuffer *		line_buffer;
	jr_int					start_line;
	jr_int					end_line;
	char *					number_format;
{
	char *ret_str = str;
	jr_int first_index, first_line, line;
	jr_int i, index;

	/*
	 * The buffer hasn't caught up to start_line yet
	 */
	if (line_buffer->line < start_line) {
		*str = 0;
		return (str);
	}

	/*
	 * end line == 0 means from start to current line
	 */
	if (end_line == 0) end_line = line_buffer->line;

	/*
	 * find the first line in the buffer
	 */
	first_index = line_buffer->line % line_buffer->max_lines;
	first_line = 0;

	for (i = 0; i < line_buffer->max_lines; i++) {

		index = (first_index + i) % line_buffer->max_lines;
		first_line = line_buffer->prev_lines[index];

		if (first_line) break;
	}
	if (!first_line) first_line = line_buffer->line;

	/*
	 * The buffer has passed end_line by
	 */
	if (first_line > end_line) {
		*str = 0;
		return (str);
	}

	/*
	 * find the intersection of the interval passed in
	 * and the interval in the buffer
	 */
	if (start_line < first_line) start_line = first_line;
	if (end_line > line_buffer->line) end_line = line_buffer->line;

	/*
	 * start line is in the buffer, but could be the current line
	 * similarly for end line
	 * all lines between are in buffer
	 */
	for (line = start_line; line <= end_line; line++) {
		if (line == line_buffer->line) break;

		index = line % line_buffer->max_lines;

		if (number_format) {
			sprintf (str, number_format, line_buffer->prev_lines [index]);
			str += strlen (str);
		}
		if (line_buffer->prev_buffers [index]) {
			strcpy (str, line_buffer->prev_buffers [index]);
			str += strlen (line_buffer->prev_buffers [index]);
		}
		*str++ = '\n';
	}
	if (end_line == line_buffer->line) {
		if (number_format) {
			sprintf (str, number_format, line_buffer->line);
			str += strlen (str);
		}
		strcpy (str, line_buffer->buffer);
		str += strlen (line_buffer->buffer);
		*str++ = '\n';
	}
	/*
	 * null out the new line
	 */
	str[-1] = 0;
	return (ret_str);
}
