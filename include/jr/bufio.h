#ifndef _jr_buf_io_h___
#define _jr_buf_io_h___

#include "ezport.h"

#include <stdio.h>

#include "jr/alist.h"
#include "jr/apr.h"

typedef struct jr_buffered_io jr_BufferedIoType;

typedef jr_int (jr_BufferedIoRawReadFn) (
							void *						raw_read_arg,
							unsigned jr_int				max_data_length,
							const char **				data_ptr_ref,
							unsigned jr_int *			data_length_ref,
							char *						error_buf);

struct jr_buffered_io {
	jr_BufferedIoRawReadFn *	raw_read_fn;
	void *						raw_read_arg;
	
	const char *				data_ptr;
	unsigned jr_int				data_length;

	jr_AList					data_buffer[1];

	const void *				read_buffer;
	jr_int						read_buffer_length;

	FILE *						rfp;
	jr_file_t *					file_info;
	jr_socket_t *				sock_info;
};


extern void				jr_BufferedIoInit (
							jr_BufferedIoType *			buf_io);

extern void				jr_BufferedIoUndo (
							jr_BufferedIoType *			buf_io);

extern void				jr_BufferedIoSetRawReadFn(
							jr_BufferedIoType *			buf_io,
							jr_BufferedIoRawReadFn *	raw_read_fn,
							void *						raw_read_arg);

extern jr_int			jr_BufferedIoGetLine (
							jr_BufferedIoType *			buf_io,
							char **						line_buffer_ref,
							char *						error_buf);

extern jr_int			jr_BufferedIoGetString (
							jr_BufferedIoType *			buf_io,
							unsigned jr_int				desired_string_length,
							char **						string_ref,
							char *						error_buf);

extern jr_int			jr_BufferedIoRead (
							jr_BufferedIoType *			buf_io,
							unsigned jr_int				max_data_length,
							const char **				data_buffer_ref,
							unsigned jr_int *			data_length_ref,
							char *						error_buf);

extern void				jr_BufferedIoSetFilePtr(
							jr_BufferedIoType *			buf_io,
							FILE *						rfp);

extern jr_int			jr_BufferedIoFread(
							void *						raw_read_arg,
							unsigned jr_int				max_data_length,
							const char **				data_ptr_ref,
							unsigned jr_int *			data_length_ref,
							char *						error_buf);

extern void				jr_BufferedIoSetFileInfo(
							jr_BufferedIoType *			buf_io,
							jr_file_t *					file_info);

extern jr_int			jr_BufferedIoFileRead(
							void *						raw_read_arg,
							unsigned jr_int				max_data_length,
							const char **				data_ptr_ref,
							unsigned jr_int *			data_length_ref,
							char *						error_buf);

extern void				jr_BufferedIoSetSocket(
							jr_BufferedIoType *			buf_io,
							jr_socket_t *				sock_info);

extern jr_int			jr_BufferedIoSocketRead(
							void *						raw_read_arg,
							unsigned jr_int				max_data_length,
							const char **				data_ptr_ref,
							unsigned jr_int *			data_length_ref,
							char *						error_buf);

#endif
