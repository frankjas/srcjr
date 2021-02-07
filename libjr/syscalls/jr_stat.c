#include "jr_apr.h"

void jr_stat_init (
	jr_stat_t *					jr_stat_ptr)
{
	memset (jr_stat_ptr, 0, sizeof (*jr_stat_ptr));
}

void jr_stat_asgn_apr_finfo (
	jr_stat_t *					jr_stat_ptr,
	const apr_finfo_t *			apr_finfo_ptr)
{

	jr_stat_init (jr_stat_ptr);

	if (sizeof (apr_finfo_ptr->size) <= sizeof (jr_int)) {
		jr_ULongAsgnUInt (&jr_stat_ptr->file_size, apr_finfo_ptr->size);
	}
	else {
		jr_ULongAsgn64 (&jr_stat_ptr->file_size, apr_finfo_ptr->size);
	}

	jr_stat_ptr->mtime	= apr_time_sec (apr_finfo_ptr->mtime);
	jr_stat_ptr->atime	= apr_time_sec (apr_finfo_ptr->atime);
	jr_stat_ptr->ctime	= apr_time_sec (apr_finfo_ptr->ctime);

	jr_stat_ptr->perms	= jr_AprPermsToPerms (apr_finfo_ptr->protection);

	switch (apr_finfo_ptr->filetype) {
		case APR_REG :
			jr_stat_ptr->is_file		= 1;
			break;
		case APR_DIR :
			jr_stat_ptr->is_directory	= 1;
			break;
		case APR_LNK :
			jr_stat_ptr->is_link		= 1;
			break;
		case APR_CHR :
			jr_stat_ptr->is_char_dev	= 1;
			break;
		default :
			break;
	}
}

jr_int jr_stat_common (
	const char *				file_name,
	jr_stat_t *					opt_jr_stat_ptr,
	jr_int						flags,
	const char **				opt_user_name_ref,
	const char **				opt_group_name_ref,
	char *						error_buf)
{
	apr_finfo_t					apr_finfo[1];
	apr_pool_t *				apr_pool_ptr			= 0;
	jr_int						info_bits				= APR_FINFO_NORM;

	jr_int						status;

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	if (flags & jr_STAT_GET_LINK_INFO) {
		info_bits	|= APR_FINFO_LINK;
	}

	status = apr_stat (apr_finfo, file_name, info_bits, apr_pool_ptr);

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


jr_int jr_stat_prepare_return (
	jr_stat_t *					opt_jr_stat_ptr,
	jr_int						flags,
	const char **				opt_user_name_ref,
	const char **				opt_group_name_ref,
	apr_finfo_t *				apr_finfo,
	apr_pool_t *				apr_pool_ptr,
	char *						error_buf)
{
	char *						user_name		= 0;
	char *						group_name		= 0;
	jr_int						status;

	if (opt_user_name_ref) {
		status	= apr_uid_name_get (&user_name, apr_finfo->user, apr_pool_ptr);

		if (status != 0  &&  !(flags & jr_STAT_IGNORE_UID_ERRORS)) {
			jr_esprintf (error_buf, "couldn't get user name: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}
	if (opt_group_name_ref) {
		status	= apr_gid_name_get (&group_name, apr_finfo->group, apr_pool_ptr);

		if (status != 0  &&  !(flags & jr_STAT_IGNORE_UID_ERRORS)) {
			jr_esprintf (error_buf, "couldn't get group name: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}

	status	= 0;

	if (opt_jr_stat_ptr) {
		jr_stat_asgn_apr_finfo (opt_jr_stat_ptr, apr_finfo);
	}
	if (opt_user_name_ref) {
		if (user_name) {
			*opt_user_name_ref	= jr_strdup (user_name);
		}
		else {
			*opt_user_name_ref	= 0;
		}
	}
	if (opt_group_name_ref) {
		if (group_name) {
			*opt_group_name_ref	= jr_strdup (group_name);
		}
		else {
			*opt_group_name_ref	= 0;
		}
	}

	return_status : {
	}

	return status;
}

jr_int jr_stat_mtime_cmp (
	const void *				void_ptr_1,
	const void *				void_ptr_2)
{
	const jr_stat_t *			jr_stat_ptr_1				= void_ptr_1;
	const jr_stat_t *			jr_stat_ptr_2				= void_ptr_2;

	if (jr_stat_ptr_1->mtime > jr_stat_ptr_2->mtime) {
		return 1;
	}
	if (jr_stat_ptr_1->mtime < jr_stat_ptr_2->mtime) {
		return -1;
	}

	return 0;
}

/*
** 3-15-2013: apr_file_mtime_set() is part of filestat.c in APR
*/
jr_int jr_FileNameSetMtime (
	const char *			file_path,
	jr_seconds_t			time_sec,
	char *					error_buf)
{
	apr_pool_t *			apr_pool_ptr;
	apr_time_t				apr_time_sec		= time_sec * 1000000LL;
	jr_int					status;

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	
	status	= apr_file_mtime_set (file_path, apr_time_sec, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
		apr_pool_destroy (apr_pool_ptr);
	}

	return status;
}

jr_int jr_FileNameGetMtime (
	const char *			file_path,
	jr_seconds_t *			time_sec_ref,
	char *					error_buf)
{
	jr_stat_t				stat_info[1];
	jr_int					status;

	jr_stat_init( stat_info);

	status = jr_stat( file_path, stat_info, error_buf);

	if (status != 0) {
		goto return_status;
	}

	*time_sec_ref	= jr_stat_mtime( stat_info);

	status	= 0;

	return_status : {
	}

	return status;
}
