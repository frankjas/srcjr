#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <stdlib.h>

#ifdef ostype_winnt
#include <crtdefs.h>
#include <io.h>
#include <fcntl.h>
#include <apr-1/apr_file_io.h>
#include <apr-1/apr_errno.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#endif


#include "jr/syscalls.h"
#include "jr/error.h"
#include "jr/apr.h"



FILE *jr_TempFileOpen (opt_template, error_buf)
	char *			opt_template;
	char *			error_buf;
{
	char *			template				= opt_template;
	FILE *			fp						= 0;
	jr_int			fd						= -1;

	char			tmp_dir[32];
	char			template_buf[sizeof (tmp_dir) + 10];

	jr_int			status;

	/*
	** 7/6/2004: returns a FILE * to a temporary file which disappears when closed.
	**
	** 2/8/07: apr_file_mktemp() provides for exclusive open and delete-on-close,
	** however, getting a file pointer from it is impossible w/o keeping the apr_file_t
	** around as well, so this API won't work.
	** For win32, you need to use _open_osfhandle() then _fdopen(),
	** see http://support.microsoft.com/kb/139640
	*/ 


	if (opt_template == 0) {
		
		status		= jr_GetTempDir (tmp_dir, sizeof (tmp_dir), error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't get tmp. directory: %s", error_buf);
			fp		= NULL;
			goto return_fp;
		}

		sprintf (template_buf, "%s/jtXXXXXX", tmp_dir);

		template	= template_buf;
	}


#	ifdef ostype_winnt
	{
		HANDLE			handle;
		DWORD			createflags			= CREATE_NEW;
		DWORD			oflags				= GENERIC_READ | GENERIC_WRITE;
		DWORD			attributes			= FILE_FLAG_DELETE_ON_CLOSE;
		DWORD			sharemode			= FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
		jr_int			crt_handle;

		jr_MkTemp (template);

        handle			= CreateFileA (
							template, oflags, sharemode, NULL, createflags, attributes, 0
						);

		if (handle == INVALID_HANDLE_VALUE) {
			jr_esprintf (error_buf, "couldn't CreateFile(): %s",
				jr_apr_strerror (apr_get_os_error(), error_buf)
			);
			fp = NULL;
			goto return_fp;
		}

		crt_handle		= _open_osfhandle ((intptr_t) handle, _O_RDWR);

		if (crt_handle < 0) {
			CloseHandle (handle);
			jr_esprintf (error_buf, "couldn't _open_osfhandle(): %s", strerror (errno));
			fp = NULL;
			goto return_fp;
		}

		fp	= _fdopen (crt_handle, "w+b");

		if (fp == NULL) {
			_close (crt_handle);			/* 2/8/07: closes 'handle' implicitly */
			jr_esprintf (error_buf, "couldn't fdopen(): %s", strerror (errno));
			goto return_fp;
		}
	}
#	else
	{
		fd			= mkstemp (template);

		if (fd < 0) {
			if (errno == EINVAL) {
				jr_esprintf (error_buf, "no trailing 'XXXXXX' in temp. file name");
			}
			else if (errno == EEXIST) {
				jr_esprintf (error_buf, "couldn't create temp. name");
			}
			else {
				jr_esprintf (error_buf, "couldn't mkstemp(): %s", strerror (errno));
			}
			fp	= NULL;
			goto return_fp;
		}

		jr_Unlink (template, 0);


		fp	= fdopen (fd, "w+b");

		if (fp == NULL) {
			jr_esprintf (error_buf, "couldn't fdopen(): %s", strerror (errno));
			goto return_fp;
		}
		fd	= -1;
	}
#	endif

	return_fp : {
		if (fd >= 0) {
			close (fd);
		}
	}

	return fp;
}


