#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>


/*| begin jr_FileNameCopy_decl |*/
#include "jr/io.h"
#include "jr/syscalls.h"

int jr_FileNameCopy (dest_file_name, src_file_name, error_buf)
	const char *		dest_file_name;
	const char *		src_file_name;
	char *				error_buf;
{
	FILE *				wfp;
	FILE *				rfp;
	int					status;

	if (strcmp (src_file_name, "-") == 0) {
		rfp	= stdin;
	}
	else {
		rfp = fopen (src_file_name, "rb");

		if (rfp == NULL) {
			jr_esprintf (error_buf, "couldn't open '%.64s' for reading: %s",
				src_file_name, strerror (errno)
			);
			return jr_ConvertErrno (errno);
		}
	}

	if (strcmp (dest_file_name, "-") == 0) {
		wfp	= stdout;
	}
	else {
		wfp = fopen (dest_file_name, "w+b");

		if (wfp == NULL) {
			jr_esprintf (error_buf, "couldn't open '%.64s' for writing: %s",
				dest_file_name, strerror (errno)
			);
			return jr_ConvertErrno (errno);
		}
	}

	status = jr_FilePtrCopy (wfp, rfp, error_buf);

	if (rfp != stdin) {
		fclose (rfp);
	}
	if (wfp != stdout) {
		fclose (wfp);
	}

	return status;
}
/* .bp */

int jr_FilePtrCopy (wfp, rfp, error_buf)
	FILE *			wfp;
	FILE *			rfp;
	char *			error_buf;
{
	int				c;

	errno = 0;
	while ((c = getc (rfp)) != EOF) {
		errno = 0;
		putc (c, wfp);
		if (errno != 0) {
			jr_esprintf (error_buf, "write failure: %s", strerror (errno));
			return jr_ConvertErrno (errno);
		}
	}
	if (errno != 0) {
		jr_esprintf (error_buf, "read failure: %s", strerror (errno));
		return jr_ConvertErrno (errno);
	}
	return 0;
}

/*| end jr_FileNameCopy_decl |*/
