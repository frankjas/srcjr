#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/misc.h"

#include "jr/inputbuf.h"

jr_int jr_IB_NumCharReadCalls;
jr_int jr_IB_NumCharsRead;
jr_int jr_IB_NumCharsFromString;
jr_int jr_IB_NumCharsUnRead;

void jr_IB_InputBufferInit (input_buffer)
	jr_IB_InputBuffer *input_buffer;
{
	memset (input_buffer, 0, sizeof (*input_buffer));

	jr_AListInit (input_buffer->context_stack, sizeof (jr_IB_ContextType));
	jr_HTableInitForPtrValues (input_buffer->context_names, jr_strhash, (jr_CmpFnType) strcmp);

	input_buffer->eof_line_number		= 0;
	input_buffer->eof_column_number		= 0;
	input_buffer->eof_byte_offset		= 0;

	input_buffer->tab_stops				= jr_EnvTabStops ();
}


void jr_IB_InputBufferUndo (input_buffer)
	jr_IB_InputBuffer *input_buffer;
{
	jr_IB_ContextType *input_context;
	jr_int i;

	jr_AListForEachElementPtr (input_buffer->context_stack, input_context) {
		jr_IB_UndoContext (input_context);
	}
	jr_AListUndo (input_buffer->context_stack);

	jr_HTableForEachElementIndex (input_buffer->context_names, i) {
		jr_free (jr_HTableKeyPtrValue (input_buffer->context_names, i));
	}
	jr_HTableUndo (input_buffer->context_names);
}


jr_IB_InputBuffer *jr_IB_InputBufferCreate (void)
{
	jr_IB_InputBuffer *input_buffer = jr_malloc (sizeof (jr_IB_InputBuffer));

	jr_IB_InputBufferInit (input_buffer);

	return (input_buffer);
}


void jr_IB_InputBufferDestroy (input_buffer)
	jr_IB_InputBuffer *input_buffer;
{
	jr_IB_InputBufferUndo (input_buffer);
	jr_free (input_buffer);
}


void jr_IB_InputBufferReset (input_buffer)
	jr_IB_InputBuffer *input_buffer;
{
	while (jr_IB_CloseCurrentContext (input_buffer));
}


jr_int jr_IB_getc (input_buffer)
	jr_IB_InputBuffer *		input_buffer;
{
	jr_IB_ContextType *		input_context;
	jr_int					c					= 0; 	/* to shutup 'uninit.' warning */


	if (jr_AListIsEmpty (input_buffer->context_stack)) {
		return (EOF);
	}

	jr_IB_NumCharReadCalls ++;

	input_context = jr_AListTailPtr (input_buffer->context_stack);

	if (jr_AListSize (input_context->unread_chars) > 0) {
		c = jr_AListNativeDeletedTail (input_context->unread_chars, char);
	}
	else {
		switch (input_context->context_type) {
			case jr_IB_FILE_CONTEXT : {
				c = getc (input_context->src_rfp);
				jr_IB_NumCharsRead ++;

				/* deal with reading DOS text files and getting rid of trailing ^M */
				if (c == '\r') {
					c = getc(input_context->src_rfp);
					if (c != '\n') {
						ungetc(c, input_context->src_rfp);
						c = '\r' ;
					}
				}

				if (c == EOF) {
					if (input_context->transparent_close) {
						jr_IB_CloseCurrentContext (input_buffer);

						return (jr_IB_getc (input_buffer));
					}
				}
				break;
			}
			case jr_IB_STRING_CONTEXT : {
				c = input_context->src_string [input_context->string_index];

				jr_IB_NumCharsFromString ++;

				if (c == 0) {
					if (input_context->transparent_close) {
						jr_IB_CloseCurrentContext (input_buffer);

						return (jr_IB_getc (input_buffer));
					}
				}
				else {
					/*
					 * read a character, increment our idea of
					 * where we are
					 */
					input_context->string_index ++;
				}
				break;
			}
		}
	}

	if (c == 0  &&  input_context->context_type  ==  jr_IB_STRING_CONTEXT) {
		c = EOF;
	}

	input_context->byte_offset ++;

	if (input_context->column_number != 0) {
		input_context->column_number ++;
	}

	switch (c) {
		case '\n'		: {
			input_context->line_number ++;
			input_context->last_num_columns	= input_context->column_number;
			input_context->column_number	= 1;

			break;
		}
		case '\t'		: {
			jr_int				spaces_to_tab_stop;

			spaces_to_tab_stop	= input_context->tab_stops
								- input_context->column_number % input_buffer->tab_stops;

			input_context->column_number	+= spaces_to_tab_stop;
		}
	}
	if (c != '\n') {
		input_context->saw_new_line = 0;
	}

	return (c);
}


void jr_IB_ungetc (input_buffer, c)
	jr_IB_InputBuffer *input_buffer;
	jr_int c;
{
	jr_IB_ContextType *input_context;


	jr_IB_NumCharsUnRead ++;

	if (jr_AListIsEmpty (input_buffer->context_stack)) {
		return ;
	}
	input_context = jr_AListTailPtr (input_buffer->context_stack);

	jr_AListNativeSetNewTail (input_context->unread_chars, c, char);

	if (input_context->column_number != 0) {
		input_context->column_number --;
	}
	if (input_context->byte_offset != 0) {
		input_context->byte_offset --;
	}

	if (c == '\n') {
		input_context->line_number --;
		input_context->column_number		= input_context->last_num_columns;
		input_context->saw_new_line			= 1;
		input_context->last_num_columns		= 0;
	}
}


char *jr_IB_gets (input_buffer, string_buf, max_length)
	jr_IB_InputBuffer *	input_buffer;
	char *			string_buf;
	jr_int			max_length;
{
	jr_int c, i;

	if (max_length == 0) return (string_buf);

	i = 0;

	while ((c = jr_IB_getc (input_buffer)) != EOF) {
		string_buf [i] = c;
		i++;

		if (i == max_length - 1)	break;
		if (c == '\n')				break;
	}
	if (i == 0) return (NULL);

	string_buf [i] = 0;
	return (string_buf);
}


void jr_IB_ungetstring (input_buffer, string)
	jr_IB_InputBuffer *input_buffer;
	char *string;
{
	jr_IB_ContextType *input_context;
	char *end_string;
	jr_int c;


	if (jr_AListIsEmpty (input_buffer->context_stack)) {
		return ;
	}
	input_context = jr_AListTailPtr (input_buffer->context_stack);

	end_string = strchr (string, 0);

	for (end_string--;  end_string >= string;  end_string--) {
		c = *end_string;

		jr_IB_NumCharsUnRead ++;

		jr_AListNativeSetNewTail (input_context->unread_chars, c, char);


		if (input_context->column_number != 0) {
			input_context->column_number --;
		}
		if (input_context->byte_offset != 0) {
			input_context->byte_offset --;
		}

		if (c == '\n') {
			input_context->line_number --;
			input_context->column_number		= input_context->last_num_columns;
			input_context->saw_new_line			= 1;
			input_context->last_num_columns		= 0;
		}
	}
}


char *jr_IB_getword (input_buffer, string_buf, max_length)
	jr_IB_InputBuffer *	input_buffer;
	char *			string_buf;
	jr_int			max_length;
{
	jr_int c, i;

	if (max_length == 0) return (string_buf);

	i = 0;

	while ((c = jr_IB_getc (input_buffer)) != EOF) {
		string_buf [i] = c;
		i++;

		if (i == max_length - 1)	break;
		if (isspace (c))			break;
	}
	if (i == 0) return (NULL);

	string_buf [i] = 0;
	return (string_buf);
}


void jr_IB_OpenContext (		input_buffer, type,
								name, do_save_name,
								ptr, do_fclose_or_free,
								transparent_close
							)
	jr_IB_InputBuffer	*	input_buffer;
	jr_int					type;
	const char *			name;
	jr_int					do_save_name;
	void *					ptr;
	jr_int					do_fclose_or_free;
	jr_int					transparent_close;
{
	jr_IB_ContextType *		input_context;
	jr_IB_ContextType *		parent_context;


	input_context = jr_AListNewTailPtr (input_buffer->context_stack);


	memset (input_context, 0, sizeof (jr_IB_ContextType));

	input_context->context_type			= type;
	input_context->do_fclose_or_free	= do_fclose_or_free;
	input_context->transparent_close	= transparent_close;
	input_context->tab_stops			= input_buffer->tab_stops;
	input_context->last_num_columns		= 0;

	if (name) {
		input_context->saw_new_line		= 1;
		input_context->line_number		= 1;
		input_context->column_number	= 1;
	}
	else if (jr_AListSize (input_buffer->context_stack)  >  1) {
		/*
		 * if no name and a parent context, then the
		 * name and line numbers come from the parent
		 */

		parent_context 					= jr_IB_ContextPtr (
											input_buffer, jr_IB_ContextDepth (input_buffer) - 2
										);

		name							= parent_context->context_name;

		input_context->line_number		= parent_context->line_number;
		input_context->column_number	= parent_context->column_number;

		input_context->is_inherited		= 1;
	}
	else {
		name							= "";

		input_context->saw_new_line		= 1;
		input_context->line_number		= 1;
		input_context->column_number	= 1;
	}

	switch (input_context->context_type) {
		case jr_IB_FILE_CONTEXT : {
			input_context->src_rfp		= ptr;
			break;
		}
		case jr_IB_STRING_CONTEXT : {
			input_context->src_string	= ptr;
			input_context->string_index	= 0;
			break;
		}
	}
	jr_AListInit (input_context->unread_chars, sizeof (char));

	jr_IB_SetCurrentContextName (input_buffer, (char *) name, do_save_name);
}


jr_int jr_IB_CloseCurrentContext (input_buffer)
	jr_IB_InputBuffer *input_buffer;
{
	jr_IB_ContextType *input_context;

	if (jr_AListIsEmpty (input_buffer->context_stack)) {
		return (0);
	}

	input_context = jr_AListTailPtr (input_buffer->context_stack);

	input_buffer->eof_line_number		= input_context->line_number + 1;
	input_buffer->eof_column_number		= 1;
	input_buffer->eof_byte_offset		= input_context->byte_offset + 1;

	jr_IB_UndoContext (input_context);

	jr_AListDeletedTailPtr (input_buffer->context_stack);

	return (1);
}



void jr_IB_UndoContext (input_context)
	jr_IB_ContextType	*input_context;
{
	jr_AListUndo (input_context->unread_chars);

	switch (input_context->context_type) {
		case jr_IB_FILE_CONTEXT :  {
			if (input_context->do_fclose_or_free) {
				fclose (input_context->src_rfp);
			}
			break;
		}
		case jr_IB_STRING_CONTEXT :  {
			if (input_context->do_fclose_or_free) {
				jr_free (input_context->src_string);
			}
			break;
		}
	}
}


char *jr_IB_ContextDescription (input_buffer, string, max_size)
	jr_IB_InputBuffer *		input_buffer;
	char *					string;
	jr_int					max_size;
{
	jr_IB_ContextType *		input_context;
	char					digit_str[64];
	char *					curr_end;
	jr_int					current_length;
	jr_int					added_length;

	current_length = 0;

	*string = 0;
	jr_AListForEachElementPtrRev (input_buffer->context_stack, input_context) {

		sprintf (digit_str, "%d", input_context->line_number);

		added_length =	(jr_int) (strlen (input_context->context_name) + strlen (digit_str));

		if (current_length != 0) {
			added_length += 4;
			/*
			 * for the ->, separating contexts
			 */
		}

		if (current_length + added_length + 1 > max_size) {
			return (string);
		}
		curr_end = strchr (string, 0);

		sprintf (curr_end, "%s%s:%s",
			current_length == 0 ? "" : "\n\tincluded by: ",
			input_context->context_name, digit_str
		);
		current_length += added_length;
	}

	return (string);
}

void jr_IB_SetCurrentContextName (input_buffer, name, save_name)
	jr_IB_InputBuffer *	input_buffer;
	char *			name;
	jr_int			save_name;
{
	jr_IB_ContextType *input_context;

	if (jr_AListIsEmpty (input_buffer->context_stack)) {
		return;
	}

	input_context = jr_AListTailPtr (input_buffer->context_stack);

	if (save_name) {
		/*
		 * put the name in the context name table
		 */
		jr_int i;

		i = jr_HTableFindPtrValueIndex (input_buffer->context_names, name);

		if (i == -1) {
			name	= jr_strdup (name);
			i		= jr_HTableNewPtrValueIndex (input_buffer->context_names, name);
		}
		name = jr_HTableKeyPtrValue (input_buffer->context_names, i);
	}

	input_context->context_name		= name;
	/*
	 * name will either be the arg passed in or a jr_strdupd copy
	 */
}


jr_int _jr_IB_CurrentContextLine (input_buffer)
	jr_IB_InputBuffer *	input_buffer;
{
	return jr_IB_CurrentContextLine(input_buffer);
}

char * _jr_IB_CurrentContextName (input_buffer)
	jr_IB_InputBuffer *	input_buffer;
{
	return jr_IB_CurrentContextName(input_buffer);
}
