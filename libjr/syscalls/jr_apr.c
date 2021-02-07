#include "jr_apr.h"

static jr_int IsInitialized		= 0;
static jr_int InitCount			= 0;

jr_int jr_apr_is_initialized ()
{
	return IsInitialized;
}

jr_int jr_apr_initialize_count ()
{
	return InitCount;
}

#define jr_APR_MAX_NUM_FREE_BLOCKS			32

void jr_apr_initialize ()
{
	if (!IsInitialized) {
		apr_pool_t *			tmp_pool_ptr;
		apr_allocator_t *		tmp_allocator_ptr;
		jr_int					status;

		apr_initialize ();

		status	= apr_pool_create ((apr_pool_t **) &tmp_pool_ptr, NULL);

		if (status == 0) {

			tmp_allocator_ptr = apr_pool_allocator_get(tmp_pool_ptr);

			if (tmp_allocator_ptr) {
				/*
				** 2/27/07: In debug mode, try to force intermediate freeing of memory
				** This may check for bounds errors earlier. Otherwise pool memory will
				** be reused without bounds checks.  But this may not have the intended effect,
				** since only whole blocks will be freed. Fragmentation may prevent actual
				** freeing.
				*/
				if (	jr_do_trace (jr_malloc_calls)
					||	jr_do_trace (jr_malloc_trap)
					||	jr_do_trace (jr_malloc_bounds)) {

					apr_allocator_max_free_set (tmp_allocator_ptr, 1);
				}
				else {
					apr_allocator_max_free_set (tmp_allocator_ptr, jr_APR_MAX_NUM_FREE_BLOCKS);
				}
			}
		}
		IsInitialized = 1;
		InitCount++;
	}
}

void jr_apr_terminate ()
{
	if (IsInitialized) {
		apr_terminate ();
		IsInitialized = 0;
	}
}

const char *jr_apr_strerror(
	jr_int						status,
	char *						error_buf)
{
	if (error_buf == 0) {
		return "";
	}
	return apr_strerror( status, error_buf, jr_ERROR_LEN);
}

jr_int jr_ConvertAprErrno (
	jr_int						errno_value)
{
	/*
	** 2/9/07: Under windows, each APR error condition is associated with multiple values
	*/
	if (APR_STATUS_IS_EACCES (errno_value)) {
		return jr_PERMISSION_ERROR;
	}
	if (APR_STATUS_IS_ENOENT (errno_value)) {
		return jr_NOT_FOUND_ERROR;
	}
	if (APR_STATUS_IS_EAGAIN (errno_value)) {
		return jr_WOULD_BLOCK_ERROR;
	}
	if (APR_STATUS_IS_EINTR (errno_value)) {
		return jr_INTERRUPTED_ERROR;
	}
	if (APR_STATUS_IS_TIMEUP (errno_value)) {
		return jr_TIME_OUT_ERROR;
	}
	if (APR_STATUS_IS_CHILD_NOTDONE (errno_value)) {
		return jr_WOULD_BLOCK_ERROR;
	}
	if (APR_STATUS_IS_ECONNREFUSED (errno_value)) {
		return jr_NOT_FOUND_ERROR;
	}
	if (APR_STATUS_IS_ENFILE (errno_value)) {
		return jr_NO_ROOM_ERROR;
	}
	if (APR_STATUS_IS_EMFILE (errno_value)) {
		return jr_NO_ROOM_ERROR;
	}
	if (APR_STATUS_IS_ENOBUFS (errno_value)) {
		return jr_NO_ROOM_ERROR;
	}
	if (APR_STATUS_IS_ENOMEM (errno_value)) {
		return jr_NO_ROOM_ERROR;
	}
	if (APR_STATUS_IS_EPROC_UNKNOWN (errno_value)) {
		return jr_NOT_FOUND_ERROR;
	}
	if (APR_STATUS_IS_ENOTIMPL (errno_value)) {
		return jr_NOT_IMPLEMENTED_ERROR;
	}
	return jr_INTERNAL_ERROR;
}

unsigned jr_int jr_PermsToAprPerms (
	unsigned jr_int				perm_flags)
{
	apr_fileperms_t		apr_mode			= 0;

	if (perm_flags & jr_PERM_S_OWNER) {
		apr_mode		|= APR_FPROT_USETID;
	}
	if (perm_flags & jr_PERM_S_GROUP) {
		apr_mode		|= APR_FPROT_GSETID;
	}

	if (perm_flags & jr_PERM_R_OWNER) {
		apr_mode		|= APR_FPROT_UREAD;
	}
	if (perm_flags & jr_PERM_W_OWNER) {
		apr_mode		|= APR_FPROT_UWRITE;
	}
	if (perm_flags & jr_PERM_X_OWNER) {
		apr_mode		|= APR_FPROT_UEXECUTE;
	}

	if (perm_flags & jr_PERM_R_GROUP) {
		apr_mode		|= APR_FPROT_GREAD;
	}
	if (perm_flags & jr_PERM_W_GROUP) {
		apr_mode		|= APR_FPROT_GWRITE;
	}
	if (perm_flags & jr_PERM_X_GROUP) {
		apr_mode		|= APR_FPROT_GEXECUTE;
	}

	if (perm_flags & jr_PERM_R_OTHER) {
		apr_mode		|= APR_FPROT_WREAD;
	}
	if (perm_flags & jr_PERM_W_OTHER) {
		apr_mode		|= APR_FPROT_WWRITE;
	}
	if (perm_flags & jr_PERM_X_OTHER) {
		apr_mode		|= APR_FPROT_WEXECUTE;
	}

	return apr_mode;
}

unsigned jr_int jr_AprPermsToPerms (
	unsigned jr_int				apr_mode)
{
	unsigned jr_int				perm_flags	= 0;

	if (apr_mode & APR_FPROT_USETID) {
		perm_flags		|= jr_PERM_S_OWNER;
	}
	if (apr_mode & APR_FPROT_GSETID) {
		perm_flags		|= jr_PERM_S_GROUP;
	}

	if (apr_mode & APR_FPROT_UREAD) {
		perm_flags		|= jr_PERM_R_OWNER;
	}
	if (apr_mode & APR_FPROT_UWRITE) {
		perm_flags		|= jr_PERM_W_OWNER;
	}
	if (apr_mode & APR_FPROT_UEXECUTE) {
		perm_flags		|= jr_PERM_X_OWNER;
	}

	if (apr_mode & APR_FPROT_GREAD) {
		perm_flags		|= jr_PERM_R_GROUP;
	}
	if (apr_mode & APR_FPROT_GWRITE) {
		perm_flags		|= jr_PERM_W_GROUP;
	}
	if (apr_mode & APR_FPROT_GEXECUTE) {
		perm_flags		|= jr_PERM_X_GROUP;
	}

	if (apr_mode & APR_FPROT_WREAD) {
		perm_flags		|= jr_PERM_R_OTHER;
	}
	if (apr_mode & APR_FPROT_WWRITE) {
		perm_flags		|= jr_PERM_W_OTHER;
	}
	if (apr_mode & APR_FPROT_WEXECUTE) {
		perm_flags		|= jr_PERM_X_OTHER;
	}

	return perm_flags;
}

#	ifdef ostype_winnt

unsigned jr_int	jr_WinNtUmask	= jr_PERM_RW_GROUP | jr_PERM_RW_OTHER;

unsigned jr_int jr_umask_winnt(
	unsigned jr_int				perm_flags)
{
	unsigned jr_int				old_umask	= jr_WinNtUmask;

	jr_WinNtUmask	= perm_flags;

	return old_umask;
}
#else

unsigned jr_int jr_umask_winnt(
	unsigned jr_int				perm_flags)
{
	/*
	** 2-25-2012: noop to allow for changes to the default WinNT umask w/o affecting Unix builds,
	*/
	return perm_flags;
}

#endif

