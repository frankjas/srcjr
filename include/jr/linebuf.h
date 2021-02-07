#ifndef _linebufh
#define _linebufh

#include "ezport.h"

typedef struct {
	char *		buffer;

	char **		prev_buffers;
	jr_int *	prev_lines;
	jr_int		max_lines;

	jr_int		line;

	jr_int		column;

	jr_int		max_size;
	jr_int		tab_stops;
} jr_LB_LineBuffer;


extern void			jr_LB_LineBufferInit		PROTO ((
						jr_LB_LineBuffer *		line_buffer,
						jr_int					max_size,
						jr_int					max_lines
					));

extern void			jr_LB_LineBufferUndo		PROTO ((
						jr_LB_LineBuffer *		line_buffer
					));

extern void			jr_LB_LineBufferReset		PROTO ((
						jr_LB_LineBuffer *		line_buffer,
						jr_int					line
					));

extern void			jr_LB_AddString				PROTO ((
						jr_LB_LineBuffer *		line_buffer,
						char *					str,
						jr_int					line,
						jr_int					desired_column
					));

extern void			jr_LB_AddChar				PROTO ((
						jr_LB_LineBuffer *		line_buffer,
						jr_int					c
					));

extern void			jr_LB_SetPreviousLine		PROTO ((
						jr_LB_LineBuffer *		line_buffer,
						jr_int					line,
						char *					buffer
					));

extern void			jr_LB_SetCurrentColumn		PROTO ((
						jr_LB_LineBuffer *		line_buffer,
						jr_int					column
					));

extern void			jr_LB_SetCurrentLine		PROTO ((
						jr_LB_LineBuffer *		line_buffer,
						jr_int					line
					));

extern void			jr_LB_LineBufferCopy		PROTO ((
						jr_LB_LineBuffer *		dest,
						jr_LB_LineBuffer *		src
					));

extern char *		jr_LB_sprint_lines			PROTO ((
						char *					str,
						jr_LB_LineBuffer *		line_buffer,
						jr_int					start_line,
						jr_int					end_line,
						char *					number_format
					));

#define				jr_LB_sprint(str, lb, format) \
					jr_LB_sprint_lines (str, lb, 0, 0, format)


#define jr_LB_CurrentColumn(lb)					((lb)->column)
#define jr_LB_NumLines(lb)						((lb)->line)

#define jr_LB_TabStops(lb, v)					((lb)->tab_stops)
#define jr_LB_SetTabStops(lb, v)				((lb)->tab_stops = (v))

#endif
