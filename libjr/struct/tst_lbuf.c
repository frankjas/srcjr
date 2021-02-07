#include "ezport.h"

#include <stdio.h>

#include "jr/linebuf.h"
#include "jr/malloc.h"

void main ()
{
	jr_LB_LineBuffer line_buffer[1];
	jr_LB_LineBuffer copy_buffer[1];
	char buffer[1024];

	jr_LB_LineBufferInit (line_buffer, 40, 4);
	jr_LB_LineBufferInit (copy_buffer, 40, 4);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_AddString (line_buffer, "he", 4, 1);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_AddString (line_buffer, "hel", 4, -16);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_AddString (line_buffer, "hel", 4, 10);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_AddString (line_buffer, "this is too long", 4, 7);
	jr_LB_AddString (line_buffer, "more strings", 6, 0);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_LineBufferCopy (copy_buffer, line_buffer);
	jr_LB_sprint (buffer, copy_buffer, "%d");
	printf ("copy:\n%s\n", buffer);

	jr_LB_sprint_lines (buffer, line_buffer, 4, 5, "%d");
	printf ("%s\n", buffer);

	jr_LB_sprint_lines (buffer, line_buffer, 4, 8, "%d");
	printf ("%s\n", buffer);

	jr_LB_SetCurrentLine (line_buffer, 7);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_LineBufferCopy (copy_buffer, line_buffer);
	jr_LB_sprint (buffer, copy_buffer, "%d");
	printf ("copy:\n%s\n", buffer);

	jr_LB_sprint_lines (buffer, line_buffer, 4, 6, "%d");
	printf ("%s\n", buffer);

	jr_LB_SetCurrentLine (line_buffer, 6);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_AddString (line_buffer, "this is too long", 4, 7);

	jr_LB_sprint (buffer, line_buffer, "%d");
	printf ("%s\n", buffer);

	jr_LB_LineBufferUndo (line_buffer);
	jr_LB_LineBufferUndo (copy_buffer);

	jr_malloc_stats (stdout, "Line Buffer - after Undo");
}
