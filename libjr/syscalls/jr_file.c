#include "ezport.h"

#include <string.h>

#include <apr-1/apr_pools.h>
#include <apr-1/apr_file_io.h>
#include <apr-1/apr_errno.h>
#include <apr-1/apr_portable.h>

#ifdef ostype_winnt
#	include <io.h>
#endif

#include "jr/apr.h"
#include "jr/error.h"
#include "jr/misc.h"
#include "jr/syscalls.h"

#include "jr_apr.h"

void jr_file_init (
	jr_file_t *					file_info)
{
	memset (file_info, 0, sizeof (jr_file_t));

	jr_apr_initialize();
}

void jr_file_undo (
	jr_file_t *					file_info)
{
	if (file_info->apr_pool_ptr) {
		/*
		** 5/13/08: If there's no pool, there's either no apr_file_ptr or it's the write
		** of a pipe, which will be closed when the parent pipe is closed.
		*/
		if (file_info->apr_file_ptr) {
			apr_file_close (file_info->apr_file_ptr);
		}
		apr_pool_destroy (file_info->apr_pool_ptr);
	}
	if (file_info->aprbuf_ptr) {
		jr_aprbuf_destroy (file_info->aprbuf_ptr);
	}
}

jr_int jr_file_cmp (
	const void *				void_ptr_1,
	const void *				void_ptr_2)
{
	const jr_file_t *			file_ptr_1			= void_ptr_1;
	const jr_file_t *			file_ptr_2			= void_ptr_2;

	return jr_ptrcmp (file_ptr_1->apr_file_ptr, file_ptr_2->apr_file_ptr);
}

jr_int jr_file_hash (
	const void *				void_ptr)
{
	const jr_file_t *			file_ptr			= void_ptr;

	return jr_ptrhash (file_ptr->apr_file_ptr);
}

jr_int jr_file_open (
	jr_file_t *					file_info,
	const char *				file_name,
	jr_int						open_flags,
	jr_int						file_perms,
	char *						error_buf)
{
	apr_int32_t					apr_flags			= 0;
	apr_fileperms_t				apr_perms			= 0;
	jr_int						status;


	if (file_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &file_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}
	else {
		if (file_info->apr_file_ptr) {
			apr_file_close (file_info->apr_file_ptr);
		}
	}


	if (open_flags & jr_FILE_OPEN_READ) {
		apr_flags		|= APR_FOPEN_READ;
	}
	if (open_flags & jr_FILE_OPEN_WRITE) {
		apr_flags		|= APR_FOPEN_WRITE;
	}
	if (open_flags & jr_FILE_OPEN_APPEND) {
		apr_flags		|= APR_FOPEN_APPEND;
	}
	if (open_flags & jr_FILE_OPEN_CREATE) {
		apr_flags		|= APR_FOPEN_CREATE;
	}
	if (open_flags & jr_FILE_OPEN_TRUNCATE) {
		apr_flags		|= APR_FOPEN_TRUNCATE;
	}
	if (open_flags & jr_FILE_OPEN_BUFFERED) {
		apr_flags		|= APR_FOPEN_BUFFERED;
	}
	if (open_flags & jr_FILE_OPEN_DEL_ON_CLOSE) {
		apr_flags		|= APR_FOPEN_DELONCLOSE;
	}
	if (open_flags & jr_FILE_OPEN_EXCLUSIVE) {
		apr_flags		|= APR_FOPEN_EXCL;
	}

	apr_perms	= jr_PermsToUmaskedAprPerms(
					file_perms & ~(jr_PERM_X_OWNER | jr_PERM_X_GROUP | jr_PERM_X_OTHER)
				);
				/*
				** 2-25-2012: to mimic Unix behavior, always mask the execute bit
				** when creating files.
				*/

	status	= apr_file_open (
				(apr_file_t **) &file_info->apr_file_ptr, 
				file_name,
				apr_flags,
				apr_perms,
				file_info->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}


jr_int jr_file_close (
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_int						status;

	if (file_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (file_info->aprbuf_ptr)) {
		status	= jr_file_flush (file_info, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}
	if (file_info->apr_file_ptr) {
		status	= apr_file_close (file_info->apr_file_ptr);

		if (status != 0) {
			jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	status	= 0;

	return_status : {
		/*
		** 2/13/07: this should behave as though the close actually worked
		*/
		if (file_info->apr_pool_ptr) {
			apr_pool_destroy (file_info->apr_pool_ptr);
		}

		file_info->apr_file_ptr		= 0;
		file_info->apr_pool_ptr		= 0;
	}
	return status;
}


jr_int jr_file_open_stdin (
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_int						status;


	if (file_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &file_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}


	status	= apr_file_open_stdin (
				(apr_file_t **) &file_info->apr_file_ptr, 
				file_info->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}


jr_int jr_file_open_stdout (
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_int						status;


	if (file_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &file_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}


	status	= apr_file_open_stdout (
				(apr_file_t **) &file_info->apr_file_ptr, 
				file_info->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}


jr_int jr_file_open_stderr (
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_int						status;


	if (file_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &file_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}


	status	= apr_file_open_stderr (
				(apr_file_t **) &file_info->apr_file_ptr, 
				file_info->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}


jr_int jr_file_open_pipe (
	jr_file_t *					read_file_info,
	jr_file_t *					write_file_info,
	char *						error_buf)
{
	jr_int						status;

	if (read_file_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &read_file_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}


	/*
	** 5/9/08: Note that the APR documentation is misleading. The actual
	** test code reads from the first pipe argument.
	*/
	status	= apr_file_pipe_create (
				(apr_file_t **) &read_file_info->apr_file_ptr, 
				(apr_file_t **) &write_file_info->apr_file_ptr, 
				read_file_info->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}

jr_int jr_file_open_tmp (
	jr_file_t *					file_info,
	char *						file_name,
	jr_int						open_flags,
	jr_int						file_perms,
	char *						error_buf)
{
	jr_int						status;

	open_flags			|= jr_FILE_OPEN_WRITING | jr_FILE_OPEN_EXCLUSIVE;

	jr_MkTemp( file_name );

	status		= jr_file_open( file_info, file_name, open_flags, file_perms, error_buf );

	return status;
}

int jr_file_setbufsize( 
	jr_file_t *					file_info,
	jr_int						buf_size,
	char *						error_buf)
{
	char *						buffer;
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	buffer	= apr_palloc( file_info->apr_pool_ptr, buf_size );

	status	= apr_file_buffer_set( file_info->apr_file_ptr, buffer, buf_size );

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status = 0;

	return_status : {
	}
	return status;
}

FILE *jr_file_stdio_open_dont_use(
	jr_file_t *					file_info,
	const char *				how,
	char *						error_buf)
{
	FILE *						fp;
	apr_os_file_t				handle;
	jr_int						status;

	/*
	** 3/26/09: NOTE: the problem with this API is that you can't close the FILE*
	** without closing the underlying jr_file_t, rendering the jr_file_t useless.
	*/

	status	= apr_os_file_get( &handle, file_info->apr_file_ptr );

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		fp = NULL;
		goto return_fp;
	}

#	ifdef ostype_winnt
	{
		/*
		** 3/26/09: the apr_os_file_t is equivalent to a HANDLE
		*/
		jr_int			crt_handle;

		crt_handle		= _open_osfhandle ((intptr_t) handle, _O_RDWR);

		if (crt_handle < 0) {
			CloseHandle (handle);
			jr_esprintf (error_buf, "couldn't _open_osfhandle(): %s", strerror (errno));
			fp = NULL;
			goto return_fp;
		}

		fp	= _fdopen( crt_handle, how );

		if (fp == NULL) {
			_close (crt_handle);			/* 2/8/07: closes 'handle' implicitly */
			jr_esprintf (error_buf, "couldn't fdopen(): %s", strerror (errno));
			goto return_fp;
		}
	}
#	else
	{
		fp	= fdopen (handle, how );

		if (fp == NULL) {
			jr_esprintf (error_buf, "couldn't fdopen(): %s", strerror (errno));
			goto return_fp;
		}
	}
#	endif

	return_fp : {
	}

	return fp;
}

jr_int jr_file_read (
	jr_file_t *					file_info,
	void *						data_buf,
	unsigned jr_int				data_buf_len,
	char *						error_buf)
{
	apr_size_t					apr_length			= data_buf_len;
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	if (data_buf_len > jr_INT_MAX) {
		jr_esprintf (error_buf, "read length %d too large, max is %d", data_buf_len, jr_INT_MAX);
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}
	status	= apr_file_read (file_info->apr_file_ptr, data_buf, &apr_length);

	if (status == APR_EOF) {
		/*
		** 3-12-2013: changing this to jr_EOF_ERROR will ripple through fcache, OTF, OTN,
		** OTC, OTA, libcw, where the 0 return value on EOF is used to stop a loop.
		** In libcw, look for ar_BytesObjectReadData().
		*/
		apr_length	= 0;
	}
	else if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));

		/*
		** 2/13/07: apr_file_read() returns apr_get_os_error()
		*/
#		ifdef ostype_winnt
		{
			status	= jr_ConvertLastError (APR_TO_OS_ERROR(status));
		}
#		else
		{
			status	= jr_ConvertErrno (APR_TO_OS_ERROR(status));
		}
#		endif

		goto return_status;
	}

	status	= (jr_int) apr_length;

	return_status : {
	}

	return status;
}

jr_int jr_file_write (
	jr_file_t *					file_info,
	const void *				data_buf,
	unsigned jr_int				data_buf_len,
	char *						error_buf)
{
	apr_size_t					apr_length			= data_buf_len;
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	if (data_buf_len > jr_INT_MAX) {
		jr_esprintf (error_buf, "write length %d too large, max is %d", data_buf_len, jr_INT_MAX);
		status	= jr_MISUSE_ERROR;
		goto return_status;
	}
	if (file_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (file_info->aprbuf_ptr)) {
		/*
		** 3/29/08: allow abitrary combinations of jr_file_write() and jr_file_printf()
		*/
		status	= jr_file_flush (file_info, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	status	= apr_file_write (file_info->apr_file_ptr, data_buf, &apr_length);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= (jr_int) apr_length;

	return_status : {
	}

	return status;
}

jr_int jr_file_printf (
	jr_file_t *					file_info,
	const char *				control_string,
								...)
{
	va_list						arg_list;
	jr_int						nbytes;
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	va_start (arg_list, control_string);

	if (file_info->aprbuf_ptr == 0) {
		file_info->aprbuf_ptr	= jr_aprbuf_create( file_info, (jr_aprbuf_writefn_t) jr_file_write, 8 * 1024 );
	}

	jr_aprbuf_set_error_buf (file_info->aprbuf_ptr, 0);

	status	= jr_aprbuf_vformatter ( file_info->aprbuf_ptr, control_string, arg_list );

	if (status < 0) {
		/*
		** 2/7/07: only fails if flush() fails, so use that status, error_buf already filled in
		*/
		status	= jr_ConvertAprErrno (jr_aprbuf_status (file_info->aprbuf_ptr));
		goto return_status;
	}
	else {
		nbytes	= status;
	}

	/*
	** 3/29/08: Not all data may be written out, need to call jr_file_flush()
	** to flush any buffered data.
	*/
	
	status	= nbytes;

	return_status : {
		va_end (arg_list);
	}

	return status;
}


jr_int jr_file_flush (
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	if (file_info->aprbuf_ptr  &&  jr_aprbuf_needs_flush (file_info->aprbuf_ptr)) {

		jr_aprbuf_set_error_buf (file_info->aprbuf_ptr, error_buf);

		status	= jr_aprbuf_flush (file_info->aprbuf_ptr);

		if (status != 0) {
			goto return_status;
		}
	}


	status	= apr_file_flush (file_info->apr_file_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}

	return status;
}

jr_int jr_file_sync (
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	status	= apr_file_sync (file_info->apr_file_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}

	return status;
}

jr_int jr_file_seek (
	jr_file_t *					file_info,
	jr_ULong					offset,
	jr_int						whence,
	char *						error_buf)
{
	apr_off_t					apr_offset			= jr_ULongToHost64 (offset);
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	if (whence == jr_FILE_SEEK_CUR) {
		whence	= APR_CUR;
	}
	else if (whence == jr_FILE_SEEK_END) {
		whence	= APR_END;
	}
	else {
		whence	= APR_SET;
	}

	status	= apr_file_seek (file_info->apr_file_ptr, whence, &apr_offset);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}
	/*
	** 1/29/07: apr_offset contains the new offset
	*/

	status	= 0;

	return_status : {
	}

	return status;
}

jr_int jr_file_truncate (
	jr_file_t *					file_info,
	jr_ULong					offset,
	char *						error_buf)
{
	apr_off_t					apr_offset			= jr_ULongToHost64 (offset);
	apr_int32_t					apr_flags;
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	apr_flags	= apr_file_flags_get( file_info->apr_file_ptr);

	if (apr_flags & APR_FOPEN_BUFFERED) {
		status	= jr_file_flush( file_info, error_buf);

		if (status != 0) {
			jr_esprintf( error_buf, "couldn't flush before truncate: %s", error_buf);
			goto return_status;
		}
	}
	status	= apr_file_trunc (file_info->apr_file_ptr, apr_offset);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}

	return status;
}

jr_int jr_file_get_offset (
	jr_file_t *					file_info,
	jr_ULong *					offset_ref,
	char *						error_buf)
{
	apr_off_t					apr_offset			= 0;
	jr_int						whence;
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	whence	= APR_CUR;

	status	= apr_file_seek (file_info->apr_file_ptr, whence, &apr_offset);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}
	/*
	** 1/29/07: apr_offset contains the new offset
	*/
	jr_ULongAsgn64 (offset_ref, apr_offset);

	status	= 0;

	return_status : {
	}

	return status;
}

jr_int jr_file_gets (
	jr_file_t *					file_info,
	char *						buffer,
	jr_int						buffer_length,
	char *						error_buf)
{
	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	status	= apr_file_gets (buffer, buffer_length, file_info->apr_file_ptr);

	if (status == APR_EOF) {
		jr_esprintf (error_buf, "end of input");
		status	= jr_EOF_ERROR;
		goto return_status;
	}
	else if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));

		/*
		** 2/13/07: apr_file_read() returns apr_get_os_error()
		*/
#		ifdef ostype_winnt
		{
			status	= jr_ConvertLastError (APR_TO_OS_ERROR(status));
		}
#		else
		{
			status	= jr_ConvertErrno (APR_TO_OS_ERROR(status));
		}
#		endif

		goto return_status;
	}

	status	= 0;

	return_status : {
	}

	return status;
}


jr_int jr_file_is_tape(
	jr_file_t *					file_info,
	char *						error_buf)
{
	jr_stat_t					stat_info[1];
	jr_int						status;


	status	= jr_file_stat (file_info, stat_info, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't 'stat' file pointer: %s", error_buf);
		return -1;
	}

	return jr_stat_is_char_dev( stat_info );
}

jr_int jr_file_stat_common (
	jr_file_t *					file_info,
	jr_int						flags,
	jr_stat_t *					opt_jr_stat_ptr,
	const char **				opt_user_name_ref,
	const char **				opt_group_name_ref,
	char *						error_buf)
{
	apr_finfo_t					apr_finfo[1];
	apr_pool_t *				apr_pool_ptr			= 0;
	jr_int						info_bits				= APR_FINFO_NORM;

	jr_int						status;

	if (file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	status = apr_file_info_get (apr_finfo, info_bits, file_info->apr_file_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= jr_stat_prepare_return(
				opt_jr_stat_ptr, flags, opt_user_name_ref, opt_group_name_ref,
				apr_finfo, apr_pool_ptr, error_buf
			);

	if (status != 0) {
		goto return_status;
	}

	status	= 0;

	return_status : {
		apr_pool_destroy (apr_pool_ptr);
	}

	return status;
}


jr_int jr_file_copy(
	jr_file_t *					dest_file_info,
	jr_file_t *					src_file_info,
	char *						error_buf)
{
	apr_pool_t *				apr_pool_ptr		= 0;
	apr_file_t *				apr_dest_ptr;
	apr_file_t *				apr_src_ptr;

	apr_size_t					dest_buf_size;
	apr_size_t					src_buf_size;
	apr_off_t					src_pos;

	char *						data_buf;
	apr_size_t					data_buf_len;
	apr_size_t					apr_length;
	apr_size_t					bytes_written;
	apr_size_t					bytes_read;

	jr_int						status;

	/*
	** 3/28/09: Dup the original files so we can turn off buffering.
	*/
	if (src_file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "source file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}
	if (dest_file_info->apr_file_ptr == 0) {
		jr_esprintf (error_buf, "destination file not open");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}



	status	= apr_pool_create( &apr_pool_ptr, NULL );

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't create pool: %s",
			jr_apr_strerror( status, error_buf)
		);
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	dest_buf_size		= apr_file_buffer_size_get( dest_file_info->apr_file_ptr );
	src_buf_size		= apr_file_buffer_size_get( src_file_info->apr_file_ptr );

	if (dest_buf_size > src_buf_size) {
		data_buf_len	= dest_buf_size;
	}
	else {
		data_buf_len	= src_buf_size;
	}
	if (data_buf_len <= 0) {
		data_buf_len	= 8192;
	}

	data_buf	= apr_palloc( apr_pool_ptr, data_buf_len );

	if (data_buf == 0) {
		jr_esprintf (error_buf, "couldn't allocate data buf");
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	src_pos	= 0;
	status	= apr_file_seek( src_file_info->apr_file_ptr, APR_CUR, &src_pos );

	if (status != 0) {
		jr_esprintf( error_buf, "couldn't get src. initial position: %s",
			jr_apr_strerror( status, error_buf)
		);
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= apr_file_dup( &apr_dest_ptr, dest_file_info->apr_file_ptr, apr_pool_ptr );

	if (status != 0) {
		jr_esprintf( error_buf, "couldn't dup. dest. file: %s",
			jr_apr_strerror( status, error_buf)
		);
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= apr_file_dup( &apr_src_ptr, src_file_info->apr_file_ptr, apr_pool_ptr );

	if (status != 0) {
		jr_esprintf( error_buf, "couldn't dup. dest. file: %s",
			jr_apr_strerror( status, error_buf)
		);
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	/*
	** 3/28/09: turn off buffering
	*/
	status	= apr_file_buffer_set( apr_dest_ptr, 0, 0 );

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= apr_file_buffer_set( apr_src_ptr, 0, 0 );

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	while (1) {
		apr_length	= data_buf_len;

		status	= apr_file_read (apr_src_ptr, data_buf, &apr_length);

		if (status == APR_EOF) {
			break;
		}
		else if (status != 0) {
			jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));

			/*
			** 2/13/07: apr_file_read() returns apr_get_os_error()
			*/
#			ifdef ostype_winnt
			{
				status	= jr_ConvertLastError (APR_TO_OS_ERROR(status));
			}
#			else
			{
				status	= jr_ConvertErrno (APR_TO_OS_ERROR(status));
			}
#			endif

			goto return_status;
		}

		bytes_written	= 0;
		bytes_read		= apr_length;


		while (bytes_written < bytes_read) {
			apr_length	= bytes_read - bytes_written;

			status	= apr_file_write (apr_dest_ptr, data_buf + bytes_written, &apr_length);

			if (apr_length > 0) {
				bytes_written	+= apr_length;
			}
			else if (status != 0) {
				jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
				status	= jr_ConvertAprErrno (status);
				goto return_status;
			}
		}
	}

	/*
	** 3/29/09: the following seek() is to overcome a "bug" in APR, where non-dup()ed
	** src file's offset is not maintained properly. The apr_file_t apparently
	** caches the file offset (in filePtr).  The dup()ed src changes the actual
	** system file offset but calls to apr_file_seek() on the original have no effect
	** because APR doesn't take this into account.
	*/
	status	= apr_file_seek( apr_src_ptr, APR_SET, &src_pos );

	if (status != 0) {
		jr_esprintf( error_buf, "couldn't reset src. position: %s",
			jr_apr_strerror( status, error_buf)
		);
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
		apr_pool_destroy (apr_pool_ptr);
	}

	return status;
}
