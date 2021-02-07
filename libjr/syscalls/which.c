#define _POSIX_SOURCE 1
#include "ezport.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"
#include "jr/vector.h"
#include "jr/misc.h"

#include "jr/syscalls.h"

#define					jr_STAT_STRUCT_PTR_IS_FROM_DIRECTORY(stat_ptr)  \
						(((stat_ptr)->st_mode & S_IFMT) == S_IFDIR)

jr_int jr_FileNameFindInPathString(file_name, path_string, result_buf, result_buf_length)
	const char *	path_string ;
	char *			result_buf ;
	jr_int			result_buf_length ;
	const char *	file_name ;
{
	char **			path_vector ;
	jr_int			max_vector_size ;
	jr_int			status ;
	jr_int			n ;

	if (path_string  ==  0) {
		return -1;
	}

	max_vector_size	=	strlen(path_string) ;
	path_vector	=	(char **) alloca(max_vector_size * sizeof(char *)) ;

	n = jr_VectorizeIntoWords(path_string, ":", path_vector, max_vector_size) ;

	if (n == max_vector_size) {
		path_vector[n-1] = 0;
	}

	status =	jr_FileNameFindInPathVector(
					file_name,
					path_vector,
					result_buf,
					result_buf_length
				) ;
	
	jr_VectorUndo(path_vector) ;

	return(status) ;
}

jr_int	jr_FileNameFindInPathVector(file_name, path_vector, result_buf, result_buf_length)
	char **			path_vector ;
	char *			result_buf ;
	jr_int			result_buf_length ;
	const char *	file_name ;
{
	jr_int			search_file_name_length ;
	jr_int			status ;

	struct stat		stat_info[1] ;
	char **			vp ;
	jr_int			nbytes ;

	char *			best_file_name_so_far ;
	char *			full_path_name_buf ;

	jr_int			best_file_mode_so_far ;
	jr_int			current_mode ;


	search_file_name_length = strlen(file_name) ;
	best_file_mode_so_far	= 0 ;
	best_file_name_so_far	= 0 ;

	for (vp = path_vector ; *vp ; vp++) {
		nbytes = strlen(*vp) + search_file_name_length + 2 ;	/* one null one '/' */
		full_path_name_buf	= (char *) jr_malloc(nbytes) ;
		strcpy(full_path_name_buf, *vp) ;
		strcat(full_path_name_buf, "/") ;
		strcat(full_path_name_buf, file_name) ;

		status = stat(full_path_name_buf, stat_info) ;
		if (status == 0) {
			if (jr_STAT_STRUCT_PTR_IS_FROM_DIRECTORY(stat_info)) {
				current_mode = -2 ;
			}
			else {
				current_mode = access(full_path_name_buf, X_OK) ;
			}

			if (best_file_name_so_far == 0) {
				best_file_name_so_far	= full_path_name_buf ;
				full_path_name_buf		= 0 ;

				best_file_mode_so_far	= current_mode ;
			}
			else if (current_mode > best_file_mode_so_far) {
				if (best_file_name_so_far) jr_free(best_file_name_so_far) ;

				best_file_name_so_far	= full_path_name_buf ;
				full_path_name_buf		= 0 ;

				best_file_mode_so_far	= current_mode ;
			}
		}

		if (full_path_name_buf) {
			jr_free(full_path_name_buf) ;
		}
	}
	if (best_file_name_so_far) {
		strncpy(result_buf, best_file_name_so_far, result_buf_length - 1) ;
		result_buf[result_buf_length - 1] = 0 ;
		jr_free(best_file_name_so_far) ;
		return(0) ;
	}

	return(-1) ;
}
