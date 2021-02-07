#include "ezport.h"

#include <apr-1/apr_user.h>

#include "jr/apr.h"
#include "jr/error.h"
#include "jr/malloc.h"

void jr_uid_init (
	jr_uid_t *					uid_ptr)
{
	memset (uid_ptr, 0, sizeof (*uid_ptr));
}

void jr_uid_undo (
	jr_uid_t *					uid_ptr)
{
	apr_pool_t *				apr_pool_ptr			= uid_ptr->apr_pool_ptr;

	if (apr_pool_ptr) {
		apr_pool_destroy (apr_pool_ptr);
	}
}

void jr_gid_init (
	jr_gid_t *					gid_ptr)
{
	memset (gid_ptr, 0, sizeof (*gid_ptr));
}

void jr_gid_undo (
	jr_gid_t *					gid_ptr)
{
	apr_pool_t *				apr_pool_ptr			= gid_ptr->apr_pool_ptr;

	if (apr_pool_ptr) {
		apr_pool_destroy (apr_pool_ptr);
	}
}


jr_int jr_uid_init_current (
	jr_uid_t *					uid_ptr,
	char *						error_buf)
{
	apr_uid_t					apr_uid_val;
	apr_gid_t					apr_gid_val;
	apr_pool_t *				apr_pool_ptr			= 0;
	jr_int						status;

	jr_uid_init (uid_ptr);

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	status = apr_uid_current (&apr_uid_val, &apr_gid_val, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	
	uid_ptr->id_value		= apr_uid_val;
	uid_ptr->apr_pool_ptr	= apr_pool_ptr;

	apr_pool_ptr			= 0;

	status		= 0;

	return_status : {
		if (apr_pool_ptr) {
			apr_pool_destroy (apr_pool_ptr);
		}
	}

	return status;
}

jr_int jr_uid_init_from_name (
	jr_uid_t *					uid_ptr,
	const char *				user_name,
	char *						error_buf)
{
	apr_uid_t					apr_uid_val;
	apr_gid_t					apr_gid_val;
	apr_pool_t *				apr_pool_ptr			= 0;
	jr_int						status;

	jr_uid_init (uid_ptr);

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	status	= apr_uid_get (&apr_uid_val, &apr_gid_val, user_name, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	
	uid_ptr->id_value		= apr_uid_val;
	uid_ptr->apr_pool_ptr	= apr_pool_ptr;

	apr_pool_ptr			= 0;


	status		= 0;

	return_status : {
		if (apr_pool_ptr) {
			apr_pool_destroy (apr_pool_ptr);
		}
	}

	return status;
}

jr_int jr_gid_init_current (
	jr_gid_t *					gid_ptr,
	char *						error_buf)
{
	apr_uid_t					apr_uid_val;
	apr_gid_t					apr_gid_val;
	apr_pool_t *				apr_pool_ptr			= 0;
	jr_int						status;

	jr_gid_init (gid_ptr);

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	status = apr_uid_current (&apr_uid_val, &apr_gid_val, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	
	gid_ptr->id_value		= apr_gid_val;
	gid_ptr->apr_pool_ptr	= apr_pool_ptr;

	apr_pool_ptr			= 0;

	status		= 0;

	return_status : {
		if (apr_pool_ptr) {
			apr_pool_destroy (apr_pool_ptr);
		}
	}

	return status;
}

jr_int jr_gid_init_from_name (
	jr_gid_t *					gid_ptr,
	const char *				group_name,
	char *						error_buf)
{
	apr_gid_t					apr_gid_val;
	apr_pool_t *				apr_pool_ptr			= 0;
	jr_int						status;

	jr_gid_init (gid_ptr);

	jr_apr_initialize();

	apr_pool_create (&apr_pool_ptr, NULL);

	status	= apr_gid_get (&apr_gid_val, group_name, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	gid_ptr->id_value		= apr_gid_val;
	gid_ptr->apr_pool_ptr	= apr_pool_ptr;

	status		= 0;

	return_status : {
		if (apr_pool_ptr) {
			apr_pool_destroy (apr_pool_ptr);
		}
	}

	return status;
}


jr_int jr_uid_get_name (
	jr_uid_t *					uid_ptr,
	const char **				user_name_ref,
	char *						error_buf)
{
	apr_uid_t					apr_uid_val				= uid_ptr->id_value;
	apr_pool_t *				apr_pool_ptr			= uid_ptr->apr_pool_ptr;
	char *						user_name;
	jr_int						status;

	status	= apr_uid_name_get (&user_name, apr_uid_val, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	*user_name_ref	= jr_strdup (user_name);
	status			= 0;

	return_status : {
	}

	return status;
}

jr_int jr_gid_get_name (
	jr_gid_t *					gid_ptr,
	const char **				group_name_ref,
	char *						error_buf)
{
	apr_gid_t					apr_gid_val				= gid_ptr->id_value;
	apr_pool_t *				apr_pool_ptr			= gid_ptr->apr_pool_ptr;
	char *						group_name;
	jr_int						status;

	status	= apr_gid_name_get (&group_name, apr_gid_val, apr_pool_ptr);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	*group_name_ref	= jr_strdup (group_name);
	status			= 0;

	return_status : {
	}

	return status;
}

jr_int jr_uid_cmp (
	const void *				void_ptr_1,
	const void *				void_ptr_2)
{
	const jr_uid_t *			uid_ptr_1					= void_ptr_1;
	const jr_uid_t *			uid_ptr_2					= void_ptr_2;

	apr_uid_t					apr_uid_val_1				= uid_ptr_1->id_value;
	apr_uid_t					apr_uid_val_2				= uid_ptr_2->id_value;
	jr_int						diff;

	diff	= apr_uid_compare (apr_uid_val_1, apr_uid_val_2);

	if (diff != 0) {
		return diff;
	}

	return 0;
}

jr_int jr_gid_cmp (
	const void *				void_ptr_1,
	const void *				void_ptr_2)
{
	const jr_gid_t *			gid_ptr_1					= void_ptr_1;
	const jr_gid_t *			gid_ptr_2					= void_ptr_2;

	apr_gid_t					apr_gid_val_1				= gid_ptr_1->id_value;
	apr_gid_t					apr_gid_val_2				= gid_ptr_2->id_value;
	jr_int						diff;

	diff	= apr_gid_compare (apr_gid_val_1, apr_gid_val_2);

	if (diff != 0) {
		return diff;
	}

	return 0;
}

