#include "ezport.h"

#include <string.h>
#include <signal.h>

#include <apr-1/apr_pools.h>
#include <apr-1/apr_thread_proc.h>
#include <apr-1/apr_lib.h>

#include "jr/apr.h"
#include "jr/error.h"
#include "jr/malloc.h"

void jr_process_init (
	jr_process_t *				process_ptr)
{
	memset (process_ptr, 0, sizeof (*process_ptr));

	process_ptr->apr_proc_ptr	= jr_malloc (sizeof (apr_proc_t));

	memset( process_ptr->apr_proc_ptr, 0, sizeof( apr_proc_t));

	jr_file_init (process_ptr->input_wfi);
	jr_file_init (process_ptr->output_rfi);
	jr_file_init (process_ptr->error_rfi);
}

void jr_process_undo (
	jr_process_t *				process_ptr)
{
	if (process_ptr->apr_proc_ptr && process_ptr->apr_pool_ptr) {
		apr_pool_note_subprocess (
			process_ptr->apr_pool_ptr, process_ptr->apr_proc_ptr, APR_KILL_AFTER_TIMEOUT
		);
		/*
		** 2/22/07: first send SIGTERM, wait() 3 seconds, then send SIGKILL, then wait()
		*/
	}

	if (process_ptr->apr_pool_ptr) {
		apr_pool_destroy (process_ptr->apr_pool_ptr);
	}
	jr_free (process_ptr->apr_proc_ptr);

	jr_file_undo (process_ptr->input_wfi);
	jr_file_undo (process_ptr->output_rfi);
	jr_file_undo (process_ptr->error_rfi);
}


void jr_procattr_child_errfn (
	apr_pool_t *				apr_pool_ptr,
	jr_int						errno,
	const char *				error_str)
{
	fprintf (stderr, "%s\n", error_str);
}

jr_int jr_process_init_procattr (
	jr_process_t *				process_ptr,
	char *						error_buf)
{
	jr_int						status;


	if (process_ptr->apr_procattr_ptr) {
		return 0;
	}

	if (process_ptr->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &process_ptr->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	status		= apr_procattr_create (
					(apr_procattr_t **) &process_ptr->apr_procattr_ptr, process_ptr->apr_pool_ptr
				);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	apr_procattr_cmdtype_set (process_ptr->apr_procattr_ptr, APR_PROGRAM_PATH);
	/*
	** 2/22/07: do a path search by default, use parents env.
	*/

	apr_procattr_io_set (process_ptr->apr_procattr_ptr, APR_NO_PIPE, APR_NO_PIPE, APR_NO_PIPE);
	/*
	** 2/22/07: no pipes back to the parent
	*/

	apr_procattr_child_errfn_set (
		process_ptr->apr_procattr_ptr, (apr_child_errfn_t *) jr_procattr_child_errfn
	);

#	ifdef ostype_winnt
	{
		if (0) {
			apr_procattr_detach_set (process_ptr->apr_procattr_ptr, TRUE);
			/*
			** 2/22/07: No command window under microsoft (no control terminal under Linux)
			** 2/23/07: Detaching under windows causes spawning problems when running from the
			** command line.
			*/
		}
	}
#	endif

	status	= 0;

	return_status : {
	}
	return status;
}

jr_int jr_process_set_input (
	jr_process_t *				process_ptr,
	jr_file_t *					child_rfi,
	jr_file_t *					opt_parent_pipe_wfi,
	char *						error_buf)
{
	jr_int						status;

	if (process_ptr->apr_procattr_ptr == 0) {
		status	= jr_process_init_procattr (process_ptr, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create process attributes: %s", error_buf);
			goto return_status;
		}
	}

	status	= apr_procattr_child_in_set (
				process_ptr->apr_procattr_ptr, 
				jr_file_apr_ptr (child_rfi),
				opt_parent_pipe_wfi ? jr_file_apr_ptr (opt_parent_pipe_wfi) : 0
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

jr_int jr_process_set_output (
	jr_process_t *				process_ptr,
	jr_file_t *					child_wfi,
	jr_file_t *					opt_parent_pipe_rfi,
	char *						error_buf)
{
	jr_int						status;

	if (process_ptr->apr_procattr_ptr == 0) {
		status	= jr_process_init_procattr (process_ptr, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create process attributes: %s", error_buf);
			goto return_status;
		}
	}
	status	= apr_procattr_child_out_set (
				process_ptr->apr_procattr_ptr, 
				jr_file_apr_ptr (child_wfi),
				opt_parent_pipe_rfi ? jr_file_apr_ptr (opt_parent_pipe_rfi) : 0
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

jr_int jr_process_set_error (
	jr_process_t *				process_ptr,
	jr_file_t *					child_wfi,
	jr_file_t *					opt_parent_pipe_rfi,
	char *						error_buf)
{
	jr_int						status;

	if (process_ptr->apr_procattr_ptr == 0) {
		status	= jr_process_init_procattr (process_ptr, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create process attributes: %s", error_buf);
			goto return_status;
		}
	}
	status	= apr_procattr_child_err_set (
				process_ptr->apr_procattr_ptr, 
				jr_file_apr_ptr (child_wfi),
				opt_parent_pipe_rfi ? jr_file_apr_ptr (opt_parent_pipe_rfi) : 0
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

jr_int jr_process_make_pipes (
	jr_process_t *				process_ptr,
	jr_int						input_kind,
	jr_int						output_kind,
	jr_int						error_kind,
	char *						error_buf)
{
	jr_int						apr_input_kind;
	jr_int						apr_output_kind;
	jr_int						apr_error_kind;
	jr_int						status;

	if (process_ptr->apr_procattr_ptr == 0) {
		status	= jr_process_init_procattr (process_ptr, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create process attributes: %s", error_buf);
			goto return_status;
		}
	}


	if (input_kind == jr_PROCESS_NO_PIPE) {
		apr_input_kind	= APR_NO_PIPE;
	}
	else if (input_kind == jr_PROCESS_NONBLOCKING_PIPE) {
		apr_input_kind	= APR_CHILD_BLOCK;
	}
	else {
		apr_input_kind	= APR_FULL_BLOCK;
	}

	if (output_kind == jr_PROCESS_NO_PIPE) {
		apr_output_kind	= APR_NO_PIPE;
	}
	else if (output_kind == jr_PROCESS_NONBLOCKING_PIPE) {
		apr_output_kind	= APR_CHILD_BLOCK;
	}
	else {
		apr_output_kind	= APR_FULL_BLOCK;
	}


	if (error_kind == jr_PROCESS_NO_PIPE) {
		apr_error_kind	= APR_NO_PIPE;
	}
	else if (error_kind == jr_PROCESS_NONBLOCKING_PIPE) {
		apr_error_kind	= APR_FULL_NONBLOCK;
	}
	else {
		apr_error_kind	= APR_FULL_BLOCK;
	}


	status	= apr_procattr_io_set(
				process_ptr->apr_procattr_ptr, apr_input_kind, apr_output_kind, apr_error_kind
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

jr_int jr_process_spawn (
	jr_process_t *				process_ptr,
	const char *				prog_name,
	const char **				opt_argv,
	const char **				opt_envp,
	char *						error_buf)
{
	const char *const*			argv				= opt_argv;
	const char *				default_argv[2];
	jr_int						status;

	default_argv[0]	= prog_name;
	default_argv[1]	= 0;

	if (argv == 0) {
		argv = default_argv;
	}

	if (process_ptr->apr_procattr_ptr == 0) {
		status	= jr_process_init_procattr (process_ptr, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create process attributes: %s", error_buf);
			goto return_status;
		}
	}

	if (opt_envp == 0) {
	}
	status	= apr_proc_create (
				process_ptr->apr_proc_ptr, prog_name, argv, opt_envp,
				process_ptr->apr_procattr_ptr, process_ptr->apr_pool_ptr
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	if (((apr_proc_t *) process_ptr->apr_proc_ptr)->in) {
		jr_file_set_apr_ptr (process_ptr->input_wfi, ((apr_proc_t *)process_ptr->apr_proc_ptr)->in);
	}
	if (((apr_proc_t *) process_ptr->apr_proc_ptr)->out) {
		jr_file_set_apr_ptr (process_ptr->output_rfi, ((apr_proc_t *)process_ptr->apr_proc_ptr)->out);
	}
	if (((apr_proc_t *) process_ptr->apr_proc_ptr)->err) {
		jr_file_set_apr_ptr (process_ptr->error_rfi, ((apr_proc_t *)process_ptr->apr_proc_ptr)->err);
	}

	status	= 0;

	return_status : {
	}
	return status;
}

jr_int jr_process_wait (
	jr_process_t *				process_ptr,
	jr_int						flags,
	jr_int *					exit_status_ref,
	jr_int *					exit_why_ref,
	char *						error_buf)
{
	jr_int						exit_status;
	apr_exit_why_e				exit_why;
	apr_wait_how_e				wait_how;
	jr_int						status;

	if (flags & jr_PROCESS_WAIT_NON_BLOCK) {
		wait_how	= APR_NOWAIT;
	}
	else {
		wait_how	= APR_WAIT;
	}

	status	= apr_proc_wait (
				process_ptr->apr_proc_ptr, &exit_status, &exit_why, wait_how
			);

	if (APR_STATUS_IS_CHILD_DONE (status)) {
		status = 0;
	}
	else if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	*exit_status_ref	= exit_status;
	*exit_why_ref		= 0;

	if (exit_why & APR_PROC_EXIT) {
		*exit_why_ref	|= jr_PROCESS_EXITTED;
	}
	if (exit_why & APR_PROC_SIGNAL) {
		*exit_why_ref	|= jr_PROCESS_WAS_SIGNALLED;
	}
	if (exit_why & APR_PROC_SIGNAL_CORE) {
		*exit_why_ref	|= jr_PROCESS_HAS_CORE;
	}
		
	status = 0;

	return_status : {
	}
	return status;
}

jr_int jr_process_kill (
	jr_process_t *				process_ptr,
	jr_int						signum_status,
	char *						error_buf)
{
	jr_int						status;

	/*
	** 11-29-2011: in MS the signal is used as the exit status
	** argument for TerminateProcess()
	*/
	if (signum_status < 0) {
#		ifdef ostype_winnt
			signum_status	= 1;
#		else
			signum_status	= SIGKILL;
#		endif
	}

	status	= apr_proc_kill ( process_ptr->apr_proc_ptr, signum_status);

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

jr_int jr_process_id (
	jr_process_t *				process_ptr)
{
	return ((apr_proc_t *) process_ptr->apr_proc_ptr)->pid;
}
