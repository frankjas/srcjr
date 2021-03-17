#ifndef _jr_memfile_h___
#define _jr_memfile_h___

#include "ezport.h"

#include <stdio.h>

#include "jr/alist.h"

#define jr_MEM_FILE_TMP_BUF_LENGTH			32

typedef struct {
	jr_AList		char_buffer [1];
	jr_int			max_buffer_size;

	jr_int			rfd;
	jr_int			wfd;

	FILE *			fp;

	void *			old_mask_ptr;
	jr_int			num_string_opens;

	char			tmp_file_name [jr_MEM_FILE_TMP_BUF_LENGTH];

	unsigned		saw_eof					: 1;
	unsigned		created_tmp_file		: 1;
} jr_MemoryFileType;


extern jr_MemoryFileType *jr_MemoryFileCreateForWriting	PROTO ((
						jr_int						max_buffer_size,
						char *						error_buf
					));

extern void			jr_MemoryFileDestroy			PROTO ((
						jr_MemoryFileType *			mem_file_ptr
					));

extern const char *	jr_MemoryFileOpenString			PROTO ((
						jr_MemoryFileType *			mem_file_ptr
					));

extern void			jr_MemoryFileCloseString		PROTO ((
						jr_MemoryFileType *			mem_file_ptr,
						const char *				string
					));


#define				jr_MemoryFilePtr(mem_file_ptr)	((mem_file_ptr)->fp)


#endif
