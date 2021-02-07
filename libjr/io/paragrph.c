#define _POSIX_SOURCE 1
#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef missing_regex_h
#	include <regex.h>
#endif


#include "jr/inputbuf.h"
#include "jr/string.h"
#include "jr/trace.h"

#include "jr/paragrph.h"
#include "jr/malloc.h"

char *	jr_ParagraphReadNewString(paragraph_info, input_buffer)
	jr_ParagraphStateInfoType *		paragraph_info ;
	jr_IB_InputBuffer *				input_buffer ;
{
	jr_AList *	result_listbuf			= paragraph_info->result_listbuf ;
	char *		push_back_string		= 0 ;
	jr_int		did_read_line			= 0 ;

	jr_ParagraphStart(paragraph_info) ;		/* re-sets initial values of some fields */

	jr_ParagraphReadLine(paragraph_info, input_buffer) ;
	/* this reads the heading, or minimum allowed text in a paragraph (1 line) */
	if (jr_is_trace_at_level(jr_io,2)) {
		fprintf(stderr,"Heading: {%s}", jr_ParagraphCurrentParagraphPtr(paragraph_info)) ;
	}

	if (paragraph_info->paragraph_ends_with_unescaped_newline) {
		if (paragraph_info->number_of_unescaped_newlines > 0) {
			jr_ParagraphSkipToNonBlankOrCommentLine(paragraph_info, input_buffer) ;
			if (jr_is_trace_at_level(jr_io,2)) {
				fprintf(stderr, "Stopping at unescaped newline\n") ;
			}
			goto return_point ;
		}
	}
				
	/*
	 * This loop will absorb trailing blank lines and other text until
	 * we see the start of another paragraph.
	 */
	while (did_read_line = jr_ParagraphReadLine(paragraph_info, input_buffer)) {
		if (jr_is_trace_at_level(jr_io,2)) {
			fprintf(stderr,"Offset %d: {%s}",
				paragraph_info->current_line_starting_index,
				paragraph_info->current_line_ptr
			) ;
		}

		push_back_string = 0 ;

		if (paragraph_info->paragraph_ends_with_unescaped_newline) {
			if (paragraph_info->number_of_unescaped_newlines > 0) {
				jr_ParagraphSkipToNonBlankOrCommentLine(paragraph_info, input_buffer) ;

				if (jr_is_trace_at_level(jr_io,2)) {
					fprintf(stderr, "Stopping at unescaped newline\n") ;
				}
				break ;
			}
		}
				
		if (paragraph_info->paragraph_ends_with_blank_line) {
			if (! paragraph_info->current_line_is_blank_or_comment) {
				if (paragraph_info->number_of_blank_lines > 0) {
					push_back_string = jr_ParagraphCurrentLinePtr(paragraph_info) ;

					if (jr_is_trace_at_level(jr_io,2)) {
						fprintf(stderr, "Stopping at blank line\n") ;
					}
					break ;
				}
			}
		}

		if (! paragraph_info->current_line_is_blank_or_comment) {
			if (paragraph_info->paragraph_starts_with_indent_less_than) {
				if (jr_is_trace_at_level(jr_io,2)) {
					fprintf(stderr, "Testing current indent %d < allowable indent %d\n",
						paragraph_info->current_line_indentation,
						paragraph_info->end_indentation
					) ;
				}
					
				if (paragraph_info->current_line_indentation < paragraph_info->end_indentation) {
					push_back_string = jr_ParagraphCurrentLinePtr(paragraph_info) ;

					if (jr_is_trace_at_level(jr_io,2)) {
						fprintf(stderr, "Stopping because line indent %d, less than allowed %d\n",
							paragraph_info->current_line_indentation,
							paragraph_info->end_indentation
						) ;
					}
					break ;
				}
				else {
					if (jr_is_trace_at_level(jr_io,2)) {
						fprintf(stderr, "Indent allowed continuation of paragraph\n") ;
					}
				}
			}

			if (paragraph_info->paragraph_starts_with_indent_greater_than) {
				if (jr_is_trace_at_level(jr_io,2)) {
					fprintf(stderr, "Testing current indent %d > allowable indent %d\n",
						paragraph_info->current_line_indentation,
						paragraph_info->end_indentation
					) ;
				}

				if (paragraph_info->current_line_indentation > paragraph_info->end_indentation) {
					push_back_string = jr_ParagraphCurrentLinePtr(paragraph_info) ;

					if (jr_is_trace_at_level(jr_io,2)) {
						fprintf(stderr, "Stopping because line indent %d, more than allowed %d\n",
							paragraph_info->current_line_indentation,
							paragraph_info->end_indentation
						) ;
					}
					break ;
				}
				else {
					if (jr_is_trace_at_level(jr_io,2)) {
						fprintf(stderr, "Indent allowed continuation of paragraph\n") ;
					}
				}
			}
		}

		if (paragraph_info->paragraph_starts_with_regular_expression) {
			if (paragraph_info->end_expression) {
				/*
				char *	current_line_of_text = jr_ParagraphCurrentLinePtr(paragraph_info) ;

				if (recmp(paragraph_info->end_expression, current_line_of_text) == 0) { 
					push_back_string = current_line_of_text ;

					if (jr_is_trace_at_level(jr_io,2)) {
						fprintf(stderr, "Stopping cuz regular current line matched expression '%s'\n",
							paragraph_info->end_expression
						) ;
					}
					break ;
				}
				*/
			}
		}
	}

	return_point : ;

	if (push_back_string) {
		char *	start_of_paragraph ;
		jr_int	length_of_paragraph	;

		start_of_paragraph	= jr_ParagraphCurrentParagraphPtr(paragraph_info) ;
		length_of_paragraph	= push_back_string - start_of_paragraph ;

		if (jr_is_trace_at_level(jr_io,2)) {
			fprintf(stderr, "Pushing back {%s} on the input for start of next paragraph.\n",
				push_back_string
			) ;
		}
		jr_IB_ungetstring(input_buffer, push_back_string) ;

		*push_back_string = 0 ;
		jr_AListSetSize(result_listbuf, length_of_paragraph) ;
	}

	if ((jr_AListSize(result_listbuf) <= 1) && (did_read_line == 0)) {
		if (jr_is_trace_at_level(jr_io,2)) {
			fprintf(stderr, "Returning 0 at AListSize %d.\n",
				jr_AListSize(result_listbuf)
			) ;
		}

		return(0) ;
	}
	else {
		char *	new_string ;

		new_string = jr_strdup(jr_AListHeadPtr(result_listbuf)) ;

		if (jr_is_trace_at_level(jr_io,2)) {
			fprintf(stderr, "Returning paragraph string at AListSize %d.\n",
				jr_AListSize(result_listbuf)
			) ;
		}
		return(new_string) ;
	}
}

void jr_ParagraphStateInfoInit(paragraph_info, end_indentation, end_expression, comment_character, flags)
		jr_ParagraphStateInfoType *		paragraph_info ;
		jr_int							end_indentation ;
		const char *					end_expression ;
		jr_int							comment_character ;
		jr_int							flags ;
{
	memset(paragraph_info, 0, sizeof(*paragraph_info)) ;

	jr_AListInit(paragraph_info->result_listbuf, sizeof(char)) ;

	paragraph_info->end_indentation	= end_indentation ;
	if (end_expression) {
		paragraph_info->end_expression	= jr_strdup(end_expression) ;
	}

	paragraph_info->comment_character	= comment_character ;

	paragraph_info->current_line_indentation	= -1 ;
	paragraph_info->previous_line_indentation	= -1 ;
	paragraph_info->heading_line_indentation	= -1 ;

	if (flags & jr_PARAGRAPH_ENDS_WITH_BLANK_LINE)
		paragraph_info->paragraph_ends_with_blank_line = 1 ;
	if (flags & jr_PARAGRAPH_ENDS_WITH_UNESCAPED_NEWLINE)
		paragraph_info->paragraph_ends_with_unescaped_newline = 1 ;

	if (flags & jr_PARAGRAPH_STARTS_WITH_INDENT_LESS_THAN)
		paragraph_info->paragraph_starts_with_indent_less_than = 1 ;
	if (flags & jr_PARAGRAPH_STARTS_WITH_INDENT_GREATER_THAN)
		paragraph_info->paragraph_starts_with_indent_greater_than = 1 ;
	if (flags & jr_PARAGRAPH_STARTS_WITH_REGULAR_EXPRESSION)
		paragraph_info->paragraph_starts_with_regular_expression = 1 ;

	if (flags & jr_PARAGRAPH_PRESERVE_INDENTATION)
		paragraph_info->paragraph_preserve_indentation = 1 ;
}

void jr_ParagraphStart(paragraph_info)
		jr_ParagraphStateInfoType *		paragraph_info ;
{
	jr_AListEmpty(paragraph_info->result_listbuf) ;

	paragraph_info->number_of_unescaped_newlines	= 0 ;

	paragraph_info->current_line_indentation	= -1 ;
	paragraph_info->previous_line_indentation	= -1 ;
	paragraph_info->heading_line_indentation	= -1 ;

	paragraph_info->number_of_blank_lines				= 0 ;
	paragraph_info->current_line_is_blank_or_comment	= 1 ;
	paragraph_info->first_non_white_this_line			= 0 ;

	paragraph_info->current_line_starting_index		= 0 ;
	paragraph_info->previous_line_starting_index	= 0 ;

}

void jr_ParagraphStateInfoUndo(paragraph_info)
		jr_ParagraphStateInfoType *		paragraph_info ;
{
	if (paragraph_info->end_expression) {
		jr_free(paragraph_info->end_expression) ;
	}
	jr_AListUndo(paragraph_info->result_listbuf) ;

	memset(paragraph_info, 0, sizeof(*paragraph_info)) ;
}

jr_int	jr_ParagraphReadLine(paragraph_info, input_buffer)
		jr_ParagraphStateInfoType *		paragraph_info ;
		jr_IB_InputBuffer *				input_buffer ;
{
	jr_AList *				result_listbuf	= paragraph_info->result_listbuf ;

	jr_int		this_char ;
	jr_int		previous_char ;
	jr_int		newline = '\n' ;
	jr_int		back_slash = '\\' ;
	jr_int		number_of_contiguous_back_slashes ;

	if (jr_AListSize(result_listbuf) > 0) {
		jr_int end_char ;

		end_char = jr_AListNativeTail(result_listbuf, char) ;
		if (end_char == 0) {
			/*
			 * We had put a null on the end to keep the thing a valid string.
			 * Now remove the null before continuing to fillup characters
			 * in that string.
			 */
			if (jr_is_trace_at_level(jr_io,2)) {
				fprintf(stderr, "Deleting null placed after end-of-line in AList of size %d.\n",
					jr_AListSize(result_listbuf)
				) ;
			}
			jr_AListDeletedTailPtr(result_listbuf) ;
		}
	}

	paragraph_info->previous_line_starting_index	= paragraph_info->current_line_starting_index ;
	paragraph_info->previous_line_ptr				= jr_ParagraphCurrentLinePtr(paragraph_info) ;
	paragraph_info->previous_line_indentation		= paragraph_info->current_line_indentation ;

	paragraph_info->current_line_starting_index	= jr_AListSize(result_listbuf) ;

	number_of_contiguous_back_slashes	= 0 ;

	paragraph_info->current_line_is_blank_or_comment = 1 ;
	paragraph_info->first_non_white_this_line = 0 ;

	for (previous_char = 0;
			((this_char = jr_IB_getc(input_buffer)) != EOF) ;
				previous_char = this_char) {

		if (previous_char == 0) {
			paragraph_info->current_line_indentation = 0 ;
		}

		if (this_char == newline) {
			jr_AListNativeSetNewTail(result_listbuf, this_char, char) ;

			if ((previous_char == back_slash) && (number_of_contiguous_back_slashes % 2)) {
				if (jr_is_trace_at_level(jr_io,2)) {
					fprintf(stderr,"Odd number of backslashes at end count as escape no added line\n") ;
				}
			}
			else {
				if (jr_is_trace_at_level(jr_io,2)) {
					fprintf(stderr,"Even number of backslashes at end of line don't count as escape\n") ;
				}
				paragraph_info->number_of_unescaped_newlines++ ;
			}
			break ;
		}

		if ((this_char != newline) && isspace(this_char)) {
			if (paragraph_info->first_non_white_this_line == 0) {
				paragraph_info->current_line_indentation++ ;

				if (paragraph_info->paragraph_preserve_indentation) {
					/* preserving white space */
				}
				else {
					continue ;
				}
			}
		}
		else if (! isspace(this_char)) {
			if (paragraph_info->first_non_white_this_line == 0) {
				paragraph_info->first_non_white_this_line = this_char ;
			}
			paragraph_info->current_line_is_blank_or_comment = 0 ;
		}

		if (this_char == back_slash) {
			number_of_contiguous_back_slashes++ ;
		}
		else {
			number_of_contiguous_back_slashes = 0 ;
		}
		jr_AListNativeSetNewTail(result_listbuf, this_char, char) ;
	}

	if (paragraph_info->comment_character &&
		(paragraph_info->first_non_white_this_line == paragraph_info->comment_character)) {

		paragraph_info->current_line_is_blank_or_comment++ ;
	}
	if (paragraph_info->current_line_is_blank_or_comment) {
		paragraph_info->number_of_blank_lines++ ;
	}

	if (paragraph_info->heading_line_indentation < 0) {
		paragraph_info->heading_line_indentation = paragraph_info->current_line_indentation ;
	}

	jr_AListNativeSetNewTail(result_listbuf, 0, char) ;	/* null out the end of this string */

	paragraph_info->current_line_ptr = jr_ParagraphCurrentLinePtr(paragraph_info) ;

	if ((this_char == EOF) && (previous_char == 0)) {
		/*
		 * We didn't manage to read even one character before hitting EOF
		 */
		return(0) ;
	}
	else {
		return(1) ;
	}
}

jr_int jr_ParagraphCurrentLineIsBlankOrComment(paragraph_info)
		jr_ParagraphStateInfoType *		paragraph_info ;
{
	char *		start_of_text = 0;

	if (! paragraph_info->current_line_ptr) return(1) ;

	for (start_of_text = paragraph_info->current_line_ptr ; *start_of_text; start_of_text++) {
		if (*start_of_text == '\n') return(1) ;
		if (*start_of_text == paragraph_info->comment_character) return(1) ;
		if (isspace(*start_of_text)) continue ;

		break ;
	}

	return(0) ;
}

char *	jr_ParagraphCurrentLinePtr(paragraph_info)
			jr_ParagraphStateInfoType *		paragraph_info ;
{
	jr_AList *	result_listbuf		= paragraph_info->result_listbuf ;
	char *		current_line_ptr ;

	current_line_ptr = jr_AListElementPtr(
							result_listbuf,
							paragraph_info->current_line_starting_index
						) ;
	
	return(current_line_ptr) ;
}

char *	jr_ParagraphCurrentParagraphPtr(paragraph_info)
			jr_ParagraphStateInfoType *		paragraph_info ;
{
	jr_AList *	result_listbuf		= paragraph_info->result_listbuf ;
	char *		current_paragraph_ptr ;

	current_paragraph_ptr =	jr_AListHeadPtr(result_listbuf) ;
	
	return(current_paragraph_ptr) ;
}

void	jr_ParagraphSkipToNonBlankOrCommentLine(paragraph_info, input_buffer)
			jr_ParagraphStateInfoType *		paragraph_info ;
			jr_IB_InputBuffer *				input_buffer ;
{
	jr_AList *	result_listbuf	= paragraph_info->result_listbuf ;

	while(jr_ParagraphReadLine(paragraph_info, input_buffer)) {
		if (! jr_ParagraphCurrentLineIsBlankOrComment(paragraph_info)) {
			char *	push_back_string ;
			char *	start_of_paragraph ;
			jr_int	length_of_paragraph	;
			
			push_back_string	= jr_ParagraphCurrentLinePtr(paragraph_info) ;
			start_of_paragraph	= jr_ParagraphCurrentParagraphPtr(paragraph_info) ;
			length_of_paragraph	= push_back_string - start_of_paragraph ;

			jr_IB_ungetstring(input_buffer, push_back_string) ;

			*push_back_string = 0 ;
			jr_AListSetSize(result_listbuf, length_of_paragraph) ;
			break ;
		}
	}
}
