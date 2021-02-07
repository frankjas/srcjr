#include "jr_apr.h"

void jr_dir_init (
	jr_dir_t *					dir_info)
{
	memset (dir_info, 0, sizeof (jr_dir_t));

	jr_apr_initialize();
}

void jr_dir_undo (
	jr_dir_t *					dir_info)
{
	jr_dir_close (dir_info);
}

jr_int jr_dir_open (
	jr_dir_t *					dir_info,
	const char *				path,
	char *						error_buf)
{
	jr_int						status;

	if (dir_info->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &dir_info->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	status	= apr_dir_open ((apr_dir_t **) &dir_info->apr_dir_ptr, path, dir_info->apr_pool_ptr);

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

void jr_dir_close (
	jr_dir_t *			dir_info)
{
	if (dir_info->apr_dir_ptr) {
		apr_dir_close (dir_info->apr_dir_ptr);
		dir_info->apr_dir_ptr	= 0;
	}

	if (dir_info->apr_pool_ptr) {
		apr_pool_destroy (dir_info->apr_pool_ptr);
		dir_info->apr_pool_ptr	= 0;
	}
}

jr_int jr_dir_read (
	jr_dir_t *			dir_info,
	const char **		opt_entry_name_ref,
	struct jr_stat *	opt_stat_info,
	char *				error_buf)
{
	apr_finfo_t			apr_finfo[1];
	jr_int				info_bits				= 0;
	jr_int				status;
	unsigned			finfo_norm_bits			= APR_FINFO_NORM;

#	ifdef ostype_winnt
		finfo_norm_bits		&= ~(APR_FINFO_IDENT | APR_FINFO_OWNER);
#	endif

	if (opt_entry_name_ref) {
		info_bits		|= APR_FINFO_NAME;
	}
	if (opt_stat_info) {
		info_bits		|= APR_FINFO_NORM;
	}
	status	= apr_dir_read (apr_finfo, info_bits, dir_info->apr_dir_ptr);

	if (status != 0) {
		if (APR_STATUS_IS_INCOMPLETE (status)) {
			if (opt_entry_name_ref  &&  (apr_finfo->valid & APR_FINFO_NAME) != APR_FINFO_NAME) {
				jr_esprintf (error_buf, "can't get dir. entry name");
				status	= jr_INTERNAL_ERROR;
				goto return_status;
			}
			if (opt_stat_info  &&  (apr_finfo->valid & finfo_norm_bits) != finfo_norm_bits) {
				jr_esprintf (error_buf, "can't get dir. entry info (valid %#x, norm %#x",
					apr_finfo->valid, finfo_norm_bits
				);
				status	= jr_INTERNAL_ERROR;
				goto return_status;
			}
			/*
			** 1/1/07: else we got what we needed.
			*/
		}
		else {
			jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	if (opt_entry_name_ref) {
		*opt_entry_name_ref	= apr_finfo->name;
	}
	if (opt_stat_info) {

		jr_stat_init (opt_stat_info);

		jr_stat_asgn_apr_finfo (opt_stat_info, apr_finfo);
	}

	status = 0;

	return_status : {
	}
	return status;
}
