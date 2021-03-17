#ifndef __jr_mime_h___
#define __jr_mime_h___

#include "jr/htable.h"

/******** Mime Table ********/

#define					jr_MIME_FILE_LINE_COMMENT_CHAR		'#'
#define					jr_MIME_FILE_LINE_COMMENT_STR		"//"
#define					jr_MIME_FILE_WORD_DELIMITERS		"\t \n"

#define					jr_MIME_TABLE_FLAG_REDEFINITIONS	0x0001
#define					jr_MIME_TABLE_EXIT_ON_ERROR			0x0002


typedef struct {
	jr_HTable		str_table [1];
} jr_MimeTableType;

typedef struct {
	char *			file_ext_str;
	char *			mime_type_str;

	char *			file_name;
	jr_int			line_number;
} jr_MimeInfoType;


extern void				jr_MimeTableInit				PROTO ((
							jr_MimeTableType *			mime_table_ptr
						));

extern void				jr_MimeTableUndo				PROTO ((
							jr_MimeTableType *			mime_table_ptr
						));

extern jr_int			jr_MimeTableAddNewElement		PROTO ((
							jr_MimeTableType *			mime_table_ptr,
							const char *				file_ext_str,
							const char *				mime_type_str,
							const char *				file_name,
							jr_int						line_number,
							char *						error_buf
						));

extern const char *		jr_MimeTableFindMimeType(
							jr_MimeTableType *			mime_table_ptr,
							const char *				file_ext_str);

extern jr_int			jr_MimeTableLoadFromFileName	PROTO ((
							jr_MimeTableType *			mime_table_ptr,
							char *						rfn,
							jr_int						mode,
							char *						error_buf
						));

extern jr_int			jr_MimeTableLoadFromFilePtr		PROTO ((
							jr_MimeTableType *			mime_table_ptr,
							FILE *						rfp,
							char *						rfn,
							jr_int						mode,
							char *						error_buf
						));

extern void				jr_MimeTablePrintForPhp			PROTO ((
							jr_MimeTableType *			mime_table_ptr,
							const char *				var_name,
							const char *				indent_str,
							FILE *						wfp
						));

#define					jr_MimeTableForEachMimeInfoPtr(mime_table_ptr, mime_info_ptr)		\
						jr_HTableForEachElementPtr ((mime_table_ptr)->str_table, mime_info_ptr)

extern void				jr_MimeInfoInit					PROTO ((
							jr_MimeInfoType *			mime_info_ptr,
							const char *				file_ext_str,
							const char *				opt_mime_type_str,
							const char *				opt_file_name,
							jr_int						opt_line_number
						));

extern void				jr_MimeInfoUndo					PROTO ((
							jr_MimeInfoType *			mime_info_ptr
						));

extern jr_int			jr_MimeInfoCmp					PROTO ((
							const void *				void_ptr_1,
							const void *				void_ptr_2
						));

extern jr_int			jr_MimeInfoHash					PROTO ((
							const void *				void_ptr_1
						));

#define					jr_MimeInfoFileExtString(mime_info_ptr)			\
						((mime_info_ptr)->file_ext_str)

#define					jr_MimeInfoMimeTypeString(mime_info_ptr)		\
						((mime_info_ptr)->mime_type_str)

#define					jr_MimeInfoFileName(mime_info_ptr)				\
						((mime_info_ptr)->file_name)

#define					jr_MimeInfoLineNumber(mime_info_ptr)			\
						((mime_info_ptr)->line_number)

#endif
