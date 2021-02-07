#include "jr_apr.h"

#include <apr-1/apr_thread_proc.h>

void jr_thread_init (
	jr_thread_t *				thread_ptr)
{
	memset (thread_ptr, 0, sizeof (jr_thread_t));

	jr_apr_initialize();
}

void jr_thread_undo (
	jr_thread_t *				thread_ptr)
{
	if (thread_ptr->apr_thread_ptr) {
		jr_thread_detach( thread_ptr, 0);
	}
	else if (thread_ptr->apr_pool_ptr) {
		apr_pool_destroy (thread_ptr->apr_pool_ptr);
		thread_ptr->apr_pool_ptr	= 0;
	}
}

static void * APR_THREAD_FUNC jr_thread_fn(
	apr_thread_t *			apr_thread_ptr,
	void *					data_ptr)
{
	jr_thread_t *			thread_ptr		= data_ptr;

	thread_ptr->thread_fn( thread_ptr, thread_ptr->arg_ptr);

	return 0;
}

jr_int jr_thread_spawn (
	jr_thread_t *				thread_ptr,
	void						(*thread_fn)(
									jr_thread_t *	thread_ptr,
									void *			arg_ptr
								),
	void *						arg_ptr,
	char *						error_buf)
{
	jr_int						status;

	if (thread_ptr->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &thread_ptr->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	thread_ptr->thread_fn	= thread_fn;
	thread_ptr->arg_ptr		= arg_ptr;

	status	= apr_thread_create (
				(apr_thread_t **) &thread_ptr->apr_thread_ptr, 0,
				jr_thread_fn, thread_ptr,
				(apr_pool_t *) thread_ptr->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status: {
	}

	return status;
}

jr_int jr_thread_detach (
	jr_thread_t *				thread_ptr,
	char *						error_buf)
{
	apr_status_t				status;

	status = apr_thread_detach( (apr_thread_t *) thread_ptr->apr_thread_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status: {
	}

	return status;
}

void jr_thread_exit( 
	jr_thread_t *				thread_ptr,
	jr_int						exit_status)
{
	apr_thread_exit( thread_ptr->apr_thread_ptr, exit_status);
	thread_ptr->apr_thread_ptr	= 0;
}

jr_int jr_thread_wait( 
	jr_thread_t *				thread_ptr,
	jr_int *					exit_status_ref,
	char *						error_buf)
{
	apr_status_t				exit_status;
	apr_status_t				status;

	status = apr_thread_join( &exit_status, (apr_thread_t *) thread_ptr->apr_thread_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	thread_ptr->apr_thread_ptr	= 0;

	status				= 0;
	*exit_status_ref	= exit_status;

	return_status: {
	}

	return status;
}
