#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "jr/io.h"
#include "jr/syscalls.h"
#include "jr/error.h"
#include "jr/malloc.h"
#include "jr/error.h"

/*| begin jr_FileNameSafeCopy_decl |*/
jr_int jr_FileNameSafeCopy (
	const char *		replaced_file,
	const char *		new_data_file,
	FILE *				opt_new_data_file_ptr,
	char *				error_buf)
{
	sigset_t			new_mask;
	sigset_t			old_mask;
	char *				tmp_file_name				= 0;
	FILE *				wfp							= 0;
	FILE *				rfp							= 0;
	jr_int				status;

	sigemptyset (&new_mask);

	sigaddset (&new_mask, SIGHUP);		/* terminal gets hung up */
	sigaddset (&new_mask, SIGINT);		/* cntrl-C */
	sigaddset (&new_mask, SIGQUIT);		/* cntrl-backslash */
	sigaddset (&new_mask, SIGALRM);		/* software alarm */
	sigaddset (&new_mask, SIGTERM);		/* software kill */
	sigaddset (&new_mask, SIGTSTP);		/* cntrl-Z */

	/*\f8*/sigprocmask (SIG_BLOCK, &new_mask, &old_mask);/*\fP*/

	/* begin critical section */

	if (access (replaced_file, F_OK)  ==  0) {

		tmp_file_name		= jr_malloc (strlen (replaced_file) + 9 + 1);

		if (strchr (replaced_file, '/')) {
			strcpy (tmp_file_name, replaced_file);
			strcpy (strrchr (tmp_file_name, '/') + 1, "tmpXXXXXX");
		}
		else {
			strcpy (tmp_file_name, "tmpXXXXXX");	/*@ string_constant_copy @*/
		}

		jr_MkTemp (tmp_file_name);				/* 3/11/2004: for classes, #define of mktemp() */

		status = rename (replaced_file, tmp_file_name);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't rename '%.64s' to temp file '%.64s': %s",
				replaced_file, tmp_file_name, strerror (errno)
			);
			status	= jr_ConvertErrno (errno);
			goto return_status;
		}
	}
	/* .bp */

	wfp		= fopen (replaced_file, "w+b");

	if (wfp == NULL) {
		jr_esprintf (error_buf, "couldn't open '%.64s' for writing: %s",
			replaced_file, strerror (errno)
		);
		status	= jr_ConvertErrno (errno);
		goto return_status;
	}

	if (opt_new_data_file_ptr) {
		rfp		= opt_new_data_file_ptr;
	}
	else {
		rfp		= fopen (new_data_file, "rb");

		if (rfp == NULL) {
			jr_esprintf (error_buf, "couldn't open '%.64s' for reading: %s",
				new_data_file, strerror (errno)
			);
			status	= jr_ConvertErrno (errno);
			goto return_status;
		}
	}

	/* .bp */

	status = jr_FilePtrCopy (wfp, rfp, error_buf);

	if (status != 0) {
		const char *		first_rename_strerror;
		jr_int				tmp_status;

		first_rename_strerror		= strerror (errno);

		if (tmp_file_name) {
			unlink (replaced_file);		/* it may have been created above */

			tmp_status	= rename (tmp_file_name, replaced_file);	
		}
		else {
			tmp_status = 0;
		}

		if (tmp_status != 0) {
			jr_esprintf (error_buf,
				"couldn't copy '%.32s' to '%.32s' (%.32s), '%.32s' is now in '%.32s' (%.32s)",
				new_data_file, replaced_file, error_buf,
				replaced_file, tmp_file_name, first_rename_strerror
			);
			status	= jr_ConvertErrno (errno);
		}
		else {
			jr_esprintf (error_buf, "couldn't copy '%.64s' to '%.64s': %s",
				new_data_file, replaced_file, error_buf
			);
		}
		goto return_status;
	}

	unlink (tmp_file_name);
	status = 0;

	return_status: {
		if (tmp_file_name) {
			jr_free (tmp_file_name);
		}
		if (wfp) {
			fclose (wfp);
		}
		if (rfp) {
			fclose (rfp);
		}
	}
	/*\f8*/sigprocmask (SIG_SETMASK, &old_mask, 0);/*\fP*/
	/* end critical section */

	return status;
}
/*| end jr_FileNameSafeCopy_decl |*/
