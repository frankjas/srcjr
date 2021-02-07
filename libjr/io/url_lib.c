#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "jr/io.h"
#include "jr/inetlib.h"
#include "jr/error.h"


jr_int jr_URL_VerifyExistence (machine_name, port_number, url_string, error_buf)
	char *					machine_name;
	jr_int					port_number;
	char *					url_string;
	char *					error_buf;
{
	jr_int					socket_fd;
	FILE *					wfp;
	FILE *					rfp;
	char					line_buf [256];
	char					version_string [256];
	jr_int					request_status;

	jr_int					status;


	socket_fd = jr_InetConnectToService (machine_name, port_number, error_buf);

	if (socket_fd < 0) {
		jr_esprintf (error_buf, "couldn't connect to server: %s", error_buf);
		return -1;
	}

	wfp = fdopen (socket_fd, "w");

	if (wfp == NULL) {
		jr_esprintf (error_buf, "couldn't get network connection: fdopen() failed");
		shutdown (socket_fd, jr_ALL_DISALLOW);
		close (socket_fd);
		return -1;
	}

	rfp = fdopen (socket_fd, "r");

	if (rfp == NULL) {
		jr_esprintf (error_buf, "couldn't get network connection: fdopen() failed");
		shutdown (socket_fd, jr_ALL_DISALLOW);
		close (socket_fd);
		fclose (wfp);
		return -1;
	}

	status = fprintf (wfp, "GET %s HTTP/1.0\n\n", url_string);

	if (status < 0) {
		jr_esprintf (error_buf, "couldn't write remote request: %s",
			strerror (errno)
		);
		status = -1;
		goto return_status;
	}

	status = fflush (wfp);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't write remote request: %s",
			strerror (errno)
		);
		status = -1;
		goto return_status;
	}

	errno = 0;

	if (fgets (line_buf, sizeof (line_buf), rfp)  ==  NULL) {
		if (errno) {
			jr_esprintf (error_buf, "no reply from server: %s",
				strerror (errno)
			);
		}
		else {
			jr_esprintf (error_buf, "no reply from server: %s",
				"unexpected end-of-file"
			);
		}
		status = -1;
		goto return_status;
	}

	status = sscanf (line_buf, "%s %d", version_string, &request_status);

	if (status != 2) {
		jr_esprintf (error_buf, "bad reply format: '%s'",
			line_buf
		);
		status = -1;
		goto return_status;
	}

	status	= request_status;

	return_status : {

		shutdown (socket_fd, jr_ALL_DISALLOW);
		close (socket_fd);
		/*
		 * Without the close, we may get a SIGPIPE, if the
		 * fclose() needs to flush (which it shouldn't).
		 */

		fclose (wfp);
		fclose (rfp);
	}

	return status;
}

