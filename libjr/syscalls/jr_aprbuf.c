#include "ezport.h"

#include <string.h>

#include <apr-1/apr_pools.h>
#include <apr-1/apr_lib.h>

#include "jr/apr.h"
#include "jr/malloc.h"

/*
** 3/29/08: jr_aprbuf_t defined here to avoid exposing APR headers
** jr_aprbuf_t must by type compatible with apr_vformatter_buff_t, which
** must be the first field.
*/

struct jr_aprbuf {
	apr_vformatter_buff_t		vbuff[1];
	char *						output_buffer;
	jr_int						buf_size;
	jr_int						error_status;
	char *						error_buf;

	void *						io_info;
	int							(*write_fn)(
									void *		io_info,
									void *		buf,
									int			buf_len,
									char *		error_buf);

	unsigned					is_in_flush				: 1;
};

#define					jr_aprbuf_is_in_flush(aprbuf_ptr)					\
						((aprbuf_ptr)->is_in_flush)

#define					jr_aprbuf_set_is_in_flush(aprbuf_ptr, v)			\
						((aprbuf_ptr)->is_in_flush = (v) != 0)

void jr_aprbuf_init (
	jr_aprbuf_t *				aprbuf_ptr,
	void *						io_info,
	int							(*write_fn)( void *io_info, void *buf, int buf_len, char *error_buf ),
	jr_int						buf_size)
{
	memset (aprbuf_ptr, 0, sizeof (*aprbuf_ptr));

	aprbuf_ptr->io_info				= io_info;
	aprbuf_ptr->write_fn			= write_fn;

	aprbuf_ptr->output_buffer		= jr_malloc (buf_size);
	aprbuf_ptr->buf_size			= buf_size;

	aprbuf_ptr->vbuff->curpos		= aprbuf_ptr->output_buffer;
	aprbuf_ptr->vbuff->endpos		= aprbuf_ptr->vbuff->curpos + aprbuf_ptr->buf_size;
}

void jr_aprbuf_undo (
	jr_aprbuf_t *		aprbuf_ptr)
{
	jr_free (aprbuf_ptr->output_buffer);
}

jr_aprbuf_t *jr_aprbuf_create (
	void *						io_info,
	int							(*write_fn)( void *io_info, void *buf, int buf_len, char *error_buf ),
	jr_int						buf_size)
{
	jr_aprbuf_t *				aprbuf_ptr	= jr_malloc (sizeof (jr_aprbuf_t));

	jr_aprbuf_init (aprbuf_ptr, io_info, write_fn, buf_size);

	return aprbuf_ptr;
}

void jr_aprbuf_destroy (
	jr_aprbuf_t *				aprbuf_ptr)
{
	jr_aprbuf_undo (aprbuf_ptr);
	jr_free (aprbuf_ptr);
}


jr_int jr_aprbuf_flush (
	jr_aprbuf_t *				aprbuf_ptr)
{
	jr_int						num_bytes;
	jr_int						status;
	char *						error_buf			= aprbuf_ptr->error_buf;

	jr_aprbuf_set_is_in_flush (aprbuf_ptr, 1);

	num_bytes	= aprbuf_ptr->vbuff->curpos - aprbuf_ptr->output_buffer;

	if (num_bytes >= 0) {
		status	= (*aprbuf_ptr->write_fn) (
					aprbuf_ptr->io_info,
					aprbuf_ptr->output_buffer, num_bytes,
					error_buf
				);

		if (status < 0) {
			aprbuf_ptr->error_status	= status;
			goto return_status;
		}
		else if (status < num_bytes) {
			/*
			** 2/7/07: didn't write everything, shift the leftover to the front of the buffer
			*/
			memmove (
				aprbuf_ptr->output_buffer,
				aprbuf_ptr->output_buffer + status,
				num_bytes - status 
			);
			aprbuf_ptr->vbuff->curpos		= aprbuf_ptr->output_buffer + num_bytes - status;
		}
		else {
			aprbuf_ptr->vbuff->curpos		= aprbuf_ptr->output_buffer;
		}
	}

	status	= 0;

	return_status : {
		jr_aprbuf_set_is_in_flush (aprbuf_ptr, 0);
	}
	return status;
}

typedef jr_int (*jr_aprbuf_flushfn_t)( apr_vformatter_buff_t *		apr_buff_ptr);

jr_int jr_aprbuf_vformatter(
	jr_aprbuf_t *				aprbuf_ptr,
	const char *				control_string,
	va_list						arg_list)
{
	jr_int						status;

	/*
	** 3/25/09: aprbuf_ptr and apr_vformatter_buff_t need to be type compatible
	** because of this call.
	*/
	status	= apr_vformatter (
				(jr_aprbuf_flushfn_t) jr_aprbuf_flush, (apr_vformatter_buff_t *) aprbuf_ptr,
				control_string,
				arg_list
			);

	return status;
}

void jr_aprbuf_set_error_buf(
	jr_aprbuf_t *				aprbuf_ptr,
	char *						error_buf)
{
	aprbuf_ptr->error_buf	= error_buf;
}

int jr_aprbuf_status(
	jr_aprbuf_t *				aprbuf_ptr)
{
	return aprbuf_ptr->error_status;
}

int jr_aprbuf_needs_flush(
	jr_aprbuf_t *				aprbuf_ptr)
{
	if (	!jr_aprbuf_is_in_flush( aprbuf_ptr )
		&&	(aprbuf_ptr)->vbuff->curpos - (aprbuf_ptr)->output_buffer) {
		
		return 1;
	}
	return 0;
}
