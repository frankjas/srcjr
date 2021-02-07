#ifndef ___apr_h__
#define ___apr_h__

#include "ezport.h"

#include <apr-1/apr_general.h>
#include <apr-1/apr_pools.h>
#include <apr-1/apr_errno.h>
#include <apr-1/apr_file_io.h>

#include "jr/apr.h"
#include "jr/syscalls.h"
#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/trace.h"

extern void					jr_stat_asgn_apr_finfo (
								struct jr_stat *			jr_stat_ptr,
								const apr_finfo_t *			apr_finfo_ptr);

extern jr_int				jr_stat_prepare_return (
								struct jr_stat *			opt_jr_stat_ptr,
								jr_int						flags,
								const char **				opt_user_name_ref,
								const char **				opt_group_name_ref,
								apr_finfo_t *				apr_finfo,
								apr_pool_t *				apr_pool_ptr,
								char *						error_buf);

#endif
