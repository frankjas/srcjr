#ifndef _input_bufferh
#define _input_bufferh

#include "ezport.h"

#include <stdio.h>

#include "jr/alist.h"
#include "jr/htable.h"


typedef struct {
	jr_AList	context_stack [1];
	jr_HTable	context_names [1];

	jr_int		eof_line_number;
	jr_int		eof_column_number;
	jr_int		eof_byte_offset;

	jr_int		tab_stops;
} jr_IB_InputBuffer;


typedef struct {
	jr_AList	unread_chars [1];
	jr_int		context_type;
	char *		context_name;

	FILE *		src_rfp;
	char *		src_string;
	jr_int		string_index;

	jr_int		line_number;
	jr_int		column_number;
	jr_int		byte_offset;
	jr_int		flags;

	jr_int		tab_stops;
	jr_int		last_num_columns;

	unsigned	is_inherited		: 1;
	unsigned	do_fclose_or_free	: 1;
	unsigned	transparent_close	: 1;
	unsigned	saw_new_line		: 1;
} jr_IB_ContextType;

extern jr_int jr_IB_NumCharReadCalls;
extern jr_int jr_IB_NumCharsRead;
extern jr_int jr_IB_NumCharsFromString;
extern jr_int jr_IB_NumCharsUnRead;

#define jr_IB_FILE_CONTEXT		1
#define jr_IB_STRING_CONTEXT	2



extern jr_IB_InputBuffer *jr_IB_InputBufferCreate		PROTO ((void));

extern void			jr_IB_InputBufferInit			PROTO ((
						jr_IB_InputBuffer *			input_buffer
					));

extern void			jr_IB_InputBufferDestroy		PROTO ((
						jr_IB_InputBuffer *			input_buffer
					));
extern void			jr_IB_InputBufferUndo			PROTO ((
						jr_IB_InputBuffer *			input_buffer
					));

extern void			jr_IB_InputBufferReset			PROTO ((
						jr_IB_InputBuffer *			input_buffer
					));

extern jr_int		jr_IB_getc						PROTO ((
						jr_IB_InputBuffer *			input_buffer
					));
extern void			jr_IB_ungetc					PROTO ((
						jr_IB_InputBuffer *			input_buffer,
						jr_int						c
					));

#define				jr_IB_ExtendedChar(input_buffer)		\
					((input_buffer)->extended_char)


extern char *		jr_IB_gets						PROTO ((
						jr_IB_InputBuffer *			input_buffer,
						char *						string_buf,
						jr_int						max_length
					));
extern void			jr_IB_ungetstring				PROTO ((
						jr_IB_InputBuffer *			input_buffer,
						char *						string
					));

extern char *		jr_IB_getword					PROTO ((
						jr_IB_InputBuffer *			input_buffer,
						char *						string_buf,
						jr_int						max_length
					));

#define jr_IB_DONT_FCLOSE_RFP	0
#define jr_IB_FCLOSE_RFP		1

#define jr_IB_DONT_FREE_STR	0
#define jr_IB_FREE_STR			1

#define jr_IB_DONT_SAVE_NAME	0
#define jr_IB_SAVE_NAME			1

extern void			jr_IB_OpenContext				PROTO ((
						jr_IB_InputBuffer *			input_buffer,
						jr_int						context_type,
						const char *				context_name,
						jr_int						do_save_name,
						void *						context_ptr,
						jr_int						do_fclose_or_free,
						jr_int						transparent_close
					));


#define jr_IB_OpenFileContext(ib, name, do_save_name, rfp, do_fclose) \
		jr_IB_OpenContext (ib, jr_IB_FILE_CONTEXT, name, do_save_name, rfp, do_fclose, 1)

#define jr_IB_OpenStringContext(ib, name, do_save_name, str, do_free) \
		jr_IB_OpenContext (ib, jr_IB_STRING_CONTEXT, name, do_save_name, (char *) str, do_free, 1)


/*
 * the switch functions will cause EOF to be read, the user has to
 * close the context to pop back to the previous context
 */

#define jr_IB_SwitchContextToFile(ib, name, do_save_name, rfp, do_fclose) \
		jr_IB_OpenContext (ib, jr_IB_FILE_CONTEXT, name, do_save_name, rfp, do_fclose, 0)

#define jr_IB_SwitchContextToString(ib, name, do_save_name, str, do_free) \
		jr_IB_OpenContext (ib, jr_IB_STRING_CONTEXT, name, do_save_name, str, do_free, 0)


extern jr_int		jr_IB_CloseCurrentContext		PROTO ((
						jr_IB_InputBuffer *			input_buffer
					));

extern void			jr_IB_UndoContext				PROTO ((
						jr_IB_ContextType *			input_context
					));


extern char *		jr_IB_ContextDescription		PROTO ((
						jr_IB_InputBuffer *			input_buffer,
						char *						string,
						jr_int						max_size
					));

extern void			jr_IB_SetCurrentContextName		PROTO ((
						jr_IB_InputBuffer *			input_buffer,
						char *						string,
						jr_int						save_name
					));


/*
 * The EOF info of the last open context
 */
#define jr_IB_EofLine(ib)			((ib)->eof_line_number)
#define jr_IB_EofColumn(ib)			((ib)->eof_column_number)
#define jr_IB_EofOffset(ib)			((ib)->eof_byte_offset)


#define jr_IB_CurrentContext(ib)	\
		((jr_IB_ContextType *) jr_AListTailPtr((ib)->context_stack))

#define jr_IB_CurrentContextName(ib)				\
		(jr_IB_CurrentContext (ib)->context_name)

#define jr_IB_CurrentContextLine(ib)			\
		(jr_IB_CurrentContext (ib)->line_number)

#define jr_IB_CurrentContextColumn(ib)		\
		(jr_IB_CurrentContext (ib)->column_number)

#define jr_IB_CurrentContextOffset(ib)		\
		(jr_IB_CurrentContext (ib)->byte_offset)

#define jr_IB_CurrentContextFlags(ib)		\
		(jr_IB_CurrentContext (ib)->flags)

#define jr_IB_CurrentContextRfp(ib)		\
		(jr_IB_CurrentContext (ib)->src_rfp)

#define jr_IB_CurrentContextString(ib)		\
		(jr_IB_CurrentContext (ib)->src_string)


#define jr_IB_SetCurrentContextLine(ib, line)			\
		(jr_IB_CurrentContext (ib)->line_number = (line))

#define jr_IB_SetCurrentContextColumn(ib, column)		\
		(jr_IB_CurrentContext (ib)->column_number = (column))

#define jr_IB_SetCurrentContextFlags(ib, line)			\
		(jr_IB_CurrentContext (ib)->flags = (line))


#define jr_IB_IsNestedContext(ib)		\
		(jr_AListSize ((ib)->context_stack) > 1)

#define jr_IB_HasContext(ib)		\
		(jr_AListSize ((ib)->context_stack) > 0)

#define jr_IB_ContextDepth(ib)			\
		jr_AListSize ((ib)->context_stack)


#define jr_IB_ContextPtr(ib, i)			\
		((jr_IB_ContextType *) jr_AListElementPtr ((ib)->context_stack, i))

#define jr_IB_ContextName(ib, i)			\
		(jr_IB_ContextPtr (ib, i)->context_name)

#define jr_IB_ContextLine(ib, i)			\
		(jr_IB_ContextPtr (ib, i)->line_number)

#define jr_IB_ContextColumn(ib, i)			\
		(jr_IB_ContextPtr (ib, i)->column_number)

#define jr_IB_ContextOffset(ib, i)			\
		(jr_IB_ContextPtr (ib, i)->byte_offset)

#define jr_IB_ContextRfp(ib, i)			\
		(jr_IB_ContextPtr (ib, i)->context_type == jr_IB_FILE_CONTEXT) ? \
			jr_IB_ContextPtr (ib, i)->src_rfp : 0 \
		)

#define jr_IB_ContextStr(ib, i)			\
		(jr_IB_ContextPtr (ib, i)->context_type == IB_STR_CONTEXT) ? \
			jr_IB_ContextPtr (ib, i)->src_string : 0 \
		)


#define jr_IB_ForEachContextIndex(ib, i)			\
		jr_AListForEachElementIndexRev ((ib)->context_stack, i)

#endif
