#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>

#include "jr/string.h"

const char *jr_PathTail (file_path)
	const char *			file_path;
{
	const char *			path_tail;

	path_tail	= strrchr (file_path, '/');

	if (path_tail) {
		path_tail ++;
	}
	else {
		path_tail	= file_path;
	}

	return path_tail;
}

const char *jr_GetTrailingPath (path_string, trailing_path_buf, trailing_path_buf_size)
	const char *				path_string;
	char *						trailing_path_buf;
	jr_int						trailing_path_buf_size;
{
	jr_int						path_length		= (jr_int) strlen (path_string);

	if (path_length < trailing_path_buf_size) {
		strcpy (trailing_path_buf, path_string);
	}
	else {
		strcpy (trailing_path_buf, "...");
		strcat (
			trailing_path_buf,
			path_string + path_length - (trailing_path_buf_size - 3 - 1)
		);
	}

	return trailing_path_buf;
}


void jr_NormalizePath (
	char *					file_path)
{
	jr_int					q;
	char *					second_last_separator		= 0;

	for (q=0; file_path[q]; q++) {

		if (	q >= 1
			&&  file_path[q-1] == '/'
			&&  file_path[q] 	== '/') {
			/*
			** 11/24/06
			** transform:	"/home/frank/src/cw//libcw/cw_mod.c"
			** into:		"/home/frank/src/cw/libcw/cw_mod.c"
			*/ 
			memmove (file_path + q, file_path + q+1, strlen (file_path + q+1) + 1);
			q -= 1;
		}
		else if (	q >= 2
				&&	file_path[q-2] == '/'
				&&  file_path[q-1] == '.'
				&&  file_path[q] 	== '/') {
			/*
			** 11/24/06
			** transform:	"/home/frank/src/cw/./libcw/cw_mod.c"
			** into:		"/home/frank/src/cw/libcw/cw_mod.c"
			*/ 
			memmove (file_path + q-1, file_path + q+1, strlen (file_path + q+1) + 1);
			q -= 2;
		}
		else if (	q >= 3  &&  second_last_separator
				&&	file_path[q-3] == '/'
				&&  file_path[q-2] == '.'
				&&  file_path[q-1] == '.'
				&&  file_path[q] 	== '/') {
			/*
			** 12/31/06
			** transform:	"/home/frank/src/cw/../libjr/alist/alist.c"
			** into:		"/home/frank/src/libjr/alist/alist.c"
			*/ 

			memmove (second_last_separator + 1, file_path + q+1, strlen (file_path + q+1) + 1);
			q = second_last_separator - file_path;
		}
		if (file_path[q] == '/') {
			second_last_separator	= jr_rstrchr (file_path + q - 1, file_path, '/');
		}
	}
	if (	q >= 2
		&&	file_path[q-2]	== '/'
		&&	file_path[q-1]	== '.') {
		/*
		** 11/24/06
		** transform:	"/home/frank/src/cw/."
		** into:		"/home/frank/src/cw/"
		*/
		file_path[q-1]	= 0;
		q -= 1;
	}
	if (	q >= 3
		&&	file_path[q-3]	== '/'
		&&	file_path[q-2]	== '.'
		&&	file_path[q-1]	== '.') {
		/*
		** 11/24/06
		** transform:	"/home/frank/src/cw/.."
		** into:		"/home/frank/src/"
		*/
		file_path[second_last_separator + 1 - file_path]	= 0;
		q = second_last_separator + 1 - file_path;
	}
	if (	q >= 1
		&&	file_path[q-1]	== '/') {
		/*
		** 11/24/06
		** transform:	"/home/frank/src/cw/"
		** into:		"/home/frank/src/cw"
		*/
		file_path[q-1]	= 0;
		q -= 1;
	}
}
