#ifndef __JR_PARAGRPH_H__
#define __JR_PARAGRPH_H__

#include <stdio.h>

#include "jr/inputbuf.h"
#include "jr/alist.h"

#define jr_PARAGRAPH_ENDS_WITH_EOF						0x00000000
#define jr_PARAGRAPH_ENDS_WITH_BLANK_LINE				0x00000002
#define jr_PARAGRAPH_ENDS_WITH_UNESCAPED_NEWLINE		0x00000004

#define jr_PARAGRAPH_STARTS_WITH_INDENT_LESS_THAN		0x00000008
#define jr_PARAGRAPH_STARTS_WITH_INDENT_GREATER_THAN	0x00000010
#define jr_PARAGRAPH_STARTS_WITH_REGULAR_EXPRESSION		0x00000020

#define jr_PARAGRAPH_PRESERVE_INDENTATION				0x00000040

typedef struct {
	jr_int		number_of_unescaped_newlines ;

	jr_AList	result_listbuf[1] ;

	jr_int		current_line_starting_index ;
	char *		current_line_ptr ;

	jr_int		previous_line_starting_index ;
	char *		previous_line_ptr ;

	jr_int		heading_line_indentation ;
	jr_int		current_line_indentation ;
	jr_int		previous_line_indentation ;

	jr_int		number_of_blank_lines ;
	jr_int		current_line_is_blank_or_comment ;
	jr_int		first_non_white_this_line ;

	jr_int		comment_character ;
	jr_int		end_indentation ;
	char *		end_expression ;

	unsigned 		paragraph_ends_with_blank_line : 1 ;
	unsigned		paragraph_ends_with_unescaped_newline : 1 ;

	unsigned		paragraph_starts_with_indent_less_than : 1 ;
	unsigned		paragraph_starts_with_indent_greater_than : 1 ;
	unsigned		paragraph_starts_with_regular_expression : 1 ;

	unsigned		paragraph_preserve_indentation : 1 ;

} jr_ParagraphStateInfoType ;

extern void						jr_ParagraphStateInfoInit					PROTO((
										jr_ParagraphStateInfoType *		paragraph_info,
										jr_int							end_indentation,
										const char *					end_expression,
										jr_int							comment_character,
										jr_int							flags
								)) ;

extern void						jr_ParagraphStateInfoUndo					PROTO((
										jr_ParagraphStateInfoType *		paragraph_info
								)) ;

extern char *					jr_ParagraphReadNewString					PROTO((
										jr_ParagraphStateInfoType *		paragraph_info,
										jr_IB_InputBuffer *				input_buffer
								)) ;

extern void						jr_ParagraphStart							PROTO((
										jr_ParagraphStateInfoType *		paragraph_info
								)) ;

extern jr_int					jr_ParagraphReadLine						PROTO((
										jr_ParagraphStateInfoType *		paragraph_info,
										jr_IB_InputBuffer *				input_buffer
								)) ;

extern jr_int					jr_ParagraphCurrentLineIsBlankOrComment		PROTO((
										jr_ParagraphStateInfoType *		paragraph_info
								)) ;

extern char *					jr_ParagraphCurrentLinePtr					PROTO((
										jr_ParagraphStateInfoType *		paragraph_info
								)) ;
											
extern char *					jr_ParagraphCurrentParagraphPtr				PROTO((
										jr_ParagraphStateInfoType *		paragraph_info
								)) ;

extern void						jr_ParagraphSkipToNonBlankOrCommentLine		PROTO((
										jr_ParagraphStateInfoType *		paragraph_info,
										jr_IB_InputBuffer *				input_buffer
								)) ;
#endif
