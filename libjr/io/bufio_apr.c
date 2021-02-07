#include "ezport.h"

#include "jr/bufio.h"
#include "jr/error.h"
#include "jr/malloc.h"


void jr_BufferedIoSetFileInfo(
	jr_BufferedIoType *			buf_io,
	jr_file_t *					file_info)
{
	buf_io->file_info			= file_info;

	if (buf_io->read_buffer) {
		jr_free( buf_io->read_buffer);
	}
	buf_io->read_buffer_length	= 8192;
	buf_io->read_buffer			= jr_malloc( buf_io->read_buffer_length);

	jr_BufferedIoSetRawReadFn( buf_io, jr_BufferedIoFileRead, buf_io);
}

jr_int jr_BufferedIoFileRead(
	void *						raw_read_arg,
	unsigned jr_int				max_data_length,
	const char **				data_ptr_ref,
	unsigned jr_int *			data_length_ref,
	char *						error_buf)
{
	jr_BufferedIoType *			buf_io			= raw_read_arg;
	jr_int						num_bytes;
	jr_int						status;

	/*
	** 11-29-2011: use jr_file_read() since fread() buffers, i.e. won't return
	** until the buffer is full or EOF.
	*/
	status	= jr_file_read(
					buf_io->file_info, (char *) buf_io->read_buffer, buf_io->read_buffer_length, error_buf
				);

	if (status < 0) {
		jr_esprintf( error_buf, "read error: %s", error_buf);
		goto return_status;
	}
	else {
		num_bytes		= status;
	}

	*data_ptr_ref		= buf_io->read_buffer;
	*data_length_ref	= num_bytes;

	status	 = 0;

	return_status : {
	}
	return status;
}


void jr_BufferedIoSetSocket(
	jr_BufferedIoType *			buf_io,
	jr_socket_t *				sock_info)
{
	buf_io->sock_info			= sock_info;

	if (buf_io->read_buffer) {
		jr_free( buf_io->read_buffer);
	}
	buf_io->read_buffer_length	= 8192;
	buf_io->read_buffer			= jr_malloc( buf_io->read_buffer_length);

	jr_BufferedIoSetRawReadFn( buf_io, jr_BufferedIoSocketRead, buf_io);
}

jr_int jr_BufferedIoSocketRead(
	void *						raw_read_arg,
	unsigned jr_int				max_data_length,
	const char **				data_ptr_ref,
	unsigned jr_int *			data_length_ref,
	char *						error_buf)
{
	jr_BufferedIoType *			buf_io			= raw_read_arg;
	jr_int						num_bytes;
	jr_int						status;

	status	= jr_socket_recv(
				buf_io->sock_info, (char *) buf_io->read_buffer, buf_io->read_buffer_length, error_buf
			);

	if (status < 0) {
		goto return_status;
	}
	else {
		num_bytes		= status;
	}

	*data_ptr_ref		= buf_io->read_buffer;
	*data_length_ref	= num_bytes;

	status	 = 0;

	return_status : {
	}
	return status;
}


