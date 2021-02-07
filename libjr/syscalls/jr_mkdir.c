#include "ezport.h"

#include <string.h>
#include <errno.h>

#include <apr-1/apr_pools.h>
#include <apr-1/apr_file_io.h>

#include "jr/trace.h"
#include "jr/syscalls.h"
#include "jr/apr.h"
#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/vector.h"

jr_int jr_MakeDirectoryPath(orig_pathname, perm_flags, error_buf)
	const char *		orig_pathname ;
	jr_int				perm_flags;
	char *				error_buf ;
{
	apr_fileperms_t		apr_perms			= jr_PermsToUmaskedAprPerms (perm_flags);
	char *				new_pathname		= 0;
	char *				curr_end_ptr;
	apr_pool_t *		apr_pool_ptr		= 0;
	jr_int				status ;
	

	new_pathname	= jr_strdup(orig_pathname) ;
	curr_end_ptr	= new_pathname;

	while (curr_end_ptr) {
		curr_end_ptr	= strchr (curr_end_ptr + 1, '/');

		if (curr_end_ptr) {
			*curr_end_ptr	= 0;
		}

		status = jr_PathIsDirectory (new_pathname, error_buf);

		if (status == jr_NOT_FOUND_ERROR) {

			jr_apr_initialize();

			status	= apr_pool_create ((apr_pool_t **) &apr_pool_ptr, NULL);

			if (status != 0) {
				jr_esprintf (error_buf, "couldn't create pool: %s",
					jr_apr_strerror( status, error_buf)
				);
				status	= jr_ConvertAprErrno (status);
				goto return_status;
			}

			status = apr_dir_make (new_pathname, apr_perms, apr_pool_ptr) ;

			if (status != 0) {
				jr_esprintf(error_buf, "%s", jr_apr_strerror( status, error_buf)) ;
				status	= jr_ConvertAprErrno (status);
				goto return_status;
			}
		}
		else if (status < 0) {

			jr_esprintf (error_buf, "couldn't access '%.64s': %s", new_pathname, error_buf);
			goto return_status;
		}
		else if (status == 0) {
			jr_esprintf (error_buf, "'%.64s' is not a directory", new_pathname);
			status	= jr_INTERNAL_ERROR;
			goto return_status;
		}
		if (curr_end_ptr) {
			*curr_end_ptr	= '/';
		}
	}

	status = 0;

	return_status : {
		if (new_pathname) {
			jr_free (new_pathname);
		}
		if (apr_pool_ptr) {
			apr_pool_destroy (apr_pool_ptr);
		}
	}

	return(status) ;
}


#ifdef obsolete__

jr_int jr_MakeDirectoryPath(pathname, perm_flags, error_buf)
	const char *	pathname ;
	jr_int			perm_flags;
	char *			error_buf ;
{
	char *	target_dirname ;
	char *  pathlist[128] ;
	jr_int    max = sizeof(pathlist) / sizeof(char *) ;
	jr_int    n ;
	jr_int	i ;
	jr_int	status ;
	jr_int	first_char ;
	mode_t			unix_mode			= jr_PermsToUnixMode (perm_flags);
	
	target_dirname	= jr_strdup(pathname) ;
	first_char		= target_dirname[0] ;

	n = jr_VectorizeIntoWords(target_dirname, "/\\", pathlist, max) ;
	if (n == max) {
		jr_esprintf(error_buf,"too many levels of directories ");
		for (n--; n >= 0; n--) {
			jr_free(pathlist[n]) ;
		}
		return(-1) ;
	}

	status = 0 ;
	*target_dirname = '\0' ;
	for (i=0; i < n; i++) {
		if ((*target_dirname != '\0') || (first_char == '/')) {
			sprintf(strchr(target_dirname,0), "/%s", pathlist[i]) ;
			first_char = 0 ;
		}
		else {
			sprintf(strchr(target_dirname,0), "%s", pathlist[i]) ;
		}
		if (jr_PathIsDirectory(target_dirname)) {
			/* nothing needs to happen here */
		}
		else {
			/* the permissions start out -drwxrwxrwx- but will be reduced by the umask */
			status = mkdir(target_dirname, unix_mode) ;
			if (status != 0) {
				jr_esprintf(error_buf, "couldn't mkdir ('%.64s'): %s",
					target_dirname, strerror(errno)
				) ;
				break ;
			}
		}
	}

	jr_VectorUndo(pathlist) ;
	jr_free(target_dirname) ;

	if (status == 0) {
		if (! jr_PathIsDirectory(pathname)) {
			jr_esprintf(error_buf, "result not directory : %s", strerror(errno)) ;
			status = -1 ;
		}
	}

	return(status) ;
}

#endif

/*
void main()
{
	char error_buf[1024] ;
	char *	pathname = "users/claspac/tmp_ftp/t1/t2/t3" ;

	jr_set_trace(jr_malloc_stats) ;

	if (jr_MakeDirectoryPath(pathname, error_buf) != 0) {
		fprintf(stderr, "%s\n", error_buf) ;
		exit(-1) ;
	}
	else {
		fprintf(stdout, "%s : is now a directory.\n", pathname) ;
	}

	jr_malloc_stats(stdout, "After MakeDirectoryPath") ;

	exit(0) ;
}
*/
