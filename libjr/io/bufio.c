#include "ezport.h"

#include <errno.h>

#include "jr/bufio.h"
#include "jr/error.h"
#include "jr/string.h"
#include "jr/malloc.h"

void jr_BufferedIoInit (
	jr_BufferedIoType *			buf_io)
{
	memset (buf_io, 0, sizeof (*buf_io));

	jr_AListInit (buf_io->data_buffer, sizeof (char));
}

void jr_BufferedIoUndo (
	jr_BufferedIoType *			buf_io)
{
	jr_AListUndo (buf_io->data_buffer);

	if (buf_io->read_buffer) {
		jr_free( buf_io->read_buffer);
	}
}

void jr_BufferedIoSetRawReadFn(
	jr_BufferedIoType *			buf_io,
	jr_BufferedIoRawReadFn *	raw_read_fn,
	void *						raw_read_arg)
{
	buf_io->raw_read_fn			= raw_read_fn;
	buf_io->raw_read_arg		= raw_read_arg;
}


jr_int jr_BufferedIoGetLine (
	jr_BufferedIoType *			buf_io,
	char **						line_buffer_ref,
	char *						error_buf)
{
	char *						line_buffer;
	jr_int						line_length;
	const char *				newline_ptr		= 0;
	jr_int						q;
	jr_int						status;


	/*
	** 12/27/06: data from the last read may still be referenced by buf_io->data_ptr
	*/
	if (jr_AListSize (buf_io->data_buffer) > 0) {
		jr_esprintf (error_buf, "internal buffer still has data");
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}

	while (1) {
		if (buf_io->data_length == 0) {
			status	= buf_io->raw_read_fn (
						buf_io->raw_read_arg, 0, &buf_io->data_ptr, &buf_io->data_length, error_buf
					);

			if (status != 0) {
				goto return_status;
			}
			if (buf_io->data_length == 0) {

				if (jr_AListSize (buf_io->data_buffer) == 0) {
					jr_esprintf (error_buf, "end of input");
					status	= jr_EOF_ERROR;
					goto return_status;
				}

				jr_AListNativeSetNewTail (buf_io->data_buffer, 0, char);

				line_length	= jr_AListSize (buf_io->data_buffer) - 1;
				line_buffer	= jr_AListExtractBuffer (buf_io->data_buffer);
				break;
			}
		}

		/*
		** 12/27/06: look for the newline, note: data_ptr is not a string
		*/
		newline_ptr	= jr_strnchr (buf_io->data_ptr, '\n', buf_io->data_length);

		if (newline_ptr) {
			line_length	= newline_ptr - buf_io->data_ptr;

			if (newline_ptr[-1] == '\r') {
				line_length --;
			}

			if (jr_AListSize (buf_io->data_buffer) > 0) {
				/*
				** 12/27/06: We've already buffered some of the data, add the rest to the buffer
				*/
				for (q=0;  q < line_length;  q++) {
					jr_AListNativeSetNewTail (buf_io->data_buffer, buf_io->data_ptr[q], char);
				}
				jr_AListNativeSetNewTail (buf_io->data_buffer, 0, char);

				line_length	= jr_AListSize (buf_io->data_buffer) - 1;
				line_buffer	= jr_AListExtractBuffer (buf_io->data_buffer);
			}
			else {
				line_buffer	= jr_malloc (line_length + 1);

				memcpy (line_buffer, buf_io->data_ptr, line_length);
				line_buffer[line_length] = 0;
			}
			buf_io->data_length	-= newline_ptr + 1 - buf_io->data_ptr;
			buf_io->data_ptr	= newline_ptr + 1;

			break;
		}
		else {
			/*
			** 12/27/06: No newline, add the current data to our buffer
			*/
			for (q=0;  q < (jr_int) buf_io->data_length;  q++) {
				jr_AListNativeSetNewTail (buf_io->data_buffer, buf_io->data_ptr[q], char);
			}
			buf_io->data_length	= 0;
		}
	}

	status				= line_length;
	*line_buffer_ref	= line_buffer;

	return_status : {
	}
	return status;
}


jr_int jr_BufferedIoGetString (
	jr_BufferedIoType *			buf_io,
	unsigned jr_int				desired_string_length,
	char **						string_ref,
	char *						error_buf)
{
	char *						string;
	jr_int						leftover_length;
	jr_int						q;
	jr_int						status;


	/*
	** 12/27/06: data from the last read may still be referenced by buf_io->data_ptr
	*/

	while (1) {

		/*
		** 9/16/07: The following cases only occur if GetKnownData() call can't read enough data
		** then is called again?
		*/
		if ((unsigned jr_int) jr_AListSize (buf_io->data_buffer) == desired_string_length) {
			jr_AListNativeSetNewTail (buf_io->data_buffer, 0, char);
			string	= jr_AListExtractBuffer (buf_io->data_buffer);
		}
		else if ((unsigned jr_int) jr_AListSize (buf_io->data_buffer) > desired_string_length) {
			jr_int				original_size;

			original_size	= jr_AListSize (buf_io->data_buffer);

			string		= jr_AListExtractBuffer (buf_io->data_buffer);

			for (q=desired_string_length;  q < original_size;  q++) {
				jr_AListNativeSetNewTail (buf_io->data_buffer, string[q], char);
			}

			string[desired_string_length]	 = 0;
			break;
		}

		if (buf_io->data_length == 0) {
			status	= buf_io->raw_read_fn(
						buf_io->raw_read_arg, 0, &buf_io->data_ptr, &buf_io->data_length, error_buf
					);

			if (status != 0) {
				goto return_status;
			}
			if (buf_io->data_length == 0) {

				if (jr_AListSize (buf_io->data_buffer) > 0) {
					jr_esprintf (error_buf, "only found %d of %d bytes",
						jr_AListSize (buf_io->data_buffer), desired_string_length
					);
					status	= jr_MISUSE_ERROR;
				}
				else {
					jr_esprintf (error_buf, "end of input");
					status	= jr_EOF_ERROR;
				}
				goto return_status;
			}
		}

		if (jr_AListSize (buf_io->data_buffer) + buf_io->data_length  <  desired_string_length) {
			/*
			** 12/27/06: Not enough data, add the current data to our buffer
			*/
			for (q=0;  q < (jr_int) buf_io->data_length;  q++) {
				jr_AListNativeSetNewTail (buf_io->data_buffer, buf_io->data_ptr[q], char);
			}
			buf_io->data_length	= 0;
		}
		else {
			if (jr_AListSize (buf_io->data_buffer) > 0) {
				/*
				** 12/27/06: We've already buffered some of the data, add the rest to the buffer
				*/

				leftover_length		= desired_string_length - jr_AListSize (buf_io->data_buffer);

				for (q=0;  q < leftover_length;  q++) {
					jr_AListNativeSetNewTail (buf_io->data_buffer, buf_io->data_ptr[q], char);
				}
				jr_AListNativeSetNewTail (buf_io->data_buffer, 0, char);

				buf_io->data_length	-= leftover_length;
				buf_io->data_ptr		+= leftover_length;

				string	= jr_AListExtractBuffer (buf_io->data_buffer);
			}
			else {
				string	= jr_malloc (desired_string_length + 1);

				memcpy (string, buf_io->data_ptr, desired_string_length);
				string[desired_string_length] = 0;

				buf_io->data_length	-= desired_string_length;
				buf_io->data_ptr		+= desired_string_length;
			}
			break;
		}
	}

	status		= 0;
	*string_ref	= string;

	return_status : {
	}
	return status;
}


jr_int jr_BufferedIoRead (
	jr_BufferedIoType *			buf_io,
	unsigned jr_int				max_data_length,
	const char **				data_buffer_ref,
	unsigned jr_int *			data_length_ref,
	char *						error_buf)
{
	const char *				data_buffer;
	unsigned jr_int				data_length;
	jr_int						status;

	if (jr_AListSize (buf_io->data_buffer) > 0) {
		/*
		** 9/16/07: Only occurs if GetKnownData() call can't read enough data?
		*/
		data_buffer	= jr_AListHeadPtr (buf_io->data_buffer);
		data_length	= jr_AListSize (buf_io->data_buffer);

		jr_AListSetSize (buf_io->data_buffer, 0);
	}
	else if (buf_io->data_length > 0) {

		data_buffer	= buf_io->data_ptr;

		if (max_data_length > 0  &&  buf_io->data_length > max_data_length) {
			data_length				= max_data_length;
			buf_io->data_ptr		+= max_data_length;
			buf_io->data_length		-= max_data_length;
		}
		else {
			data_length				= buf_io->data_length;
			buf_io->data_ptr		+= buf_io->data_length;
			buf_io->data_length		= 0;
		}
	}
	else if (buf_io->data_length == 0) {

		status	= buf_io->raw_read_fn(
					buf_io->raw_read_arg, max_data_length, &data_buffer, &data_length, error_buf
				);

		if (status != 0) {
			goto return_status;
		}
		if (max_data_length > 0  &&  data_length > max_data_length) {
			buf_io->data_ptr		= data_buffer + max_data_length;
			buf_io->data_length		= data_length - max_data_length;;

			data_length				= max_data_length;
		}
	}
	else {
		jr_esprintf( error_buf, "illegal negative data length %d", buf_io->data_length);
		status			= jr_INTERNAL_ERROR;
		goto return_status;
	}

	status				= 0;

	*data_buffer_ref	= data_buffer;
	*data_length_ref	= data_length;

	return_status : {
	}
	return status;
}


void jr_BufferedIoSetFilePtr(
	jr_BufferedIoType *			buf_io,
	FILE *						rfp)
{
	buf_io->rfp					= rfp;

	if (buf_io->read_buffer) {
		jr_free( buf_io->read_buffer);
	}
	buf_io->read_buffer_length	= 8192;
	buf_io->read_buffer			= jr_malloc( buf_io->read_buffer_length);

	jr_BufferedIoSetRawReadFn( buf_io, jr_BufferedIoFread, buf_io);
}

jr_int jr_BufferedIoFread(
	void *						raw_read_arg,
	unsigned jr_int				max_data_length,
	const char **				data_ptr_ref,
	unsigned jr_int *			data_length_ref,
	char *						error_buf)
{
	jr_BufferedIoType *			buf_io			= raw_read_arg;
	size_t						num_bytes;
	jr_int						status;

	/*
	** 11-29-2011: use read(2) since fread() buffers, i.e. won't return
	** until the buffer is full or EOF.
	*/
	num_bytes	= fread( (char *) buf_io->read_buffer, 1, buf_io->read_buffer_length, buf_io->rfp);

	if (num_bytes == 0  &&  ferror( buf_io->rfp)) {
		jr_esprintf( error_buf, "read error: %s", strerror( errno));
		status				= jr_INTERNAL_ERROR;

		goto return_status;
	}

	*data_ptr_ref		= buf_io->read_buffer;
	*data_length_ref	= (jr_int) num_bytes;


	status	 = 0;

	return_status : {
	}
	return status;
}

