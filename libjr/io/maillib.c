#include "mailproj.h"


jr_int jr_MailStringMessage (
								smtp_server_name, sender_address, recipient_address,
								opt_header_vector, message_string,
								error_buf
							)
	char *					smtp_server_name;
	char *					sender_address;
	char *					recipient_address;
	char **					opt_header_vector;
	char *					message_string;
	char *					error_buf;
{
	jr_int					smtp_fd				= -1;
	FILE *					smtp_wfp			= 0;
	char *					smtp_command;
	jr_int					smtp_status;

	char					local_hostname [256];
	char					line_buffer [SMTP_MAX_RESPONSE_LENGTH];

	jr_int					status;


	status		= gethostname (local_hostname, sizeof (local_hostname) - 1);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get local host name: %s", strerror (errno));
		status = jr_INET_HOST_NOT_FOUND;
		goto return_status;
	}


	smtp_fd		= jr_InetConnectToService (
					smtp_server_name, SMTP_PORT, error_buf
				);

	if (smtp_fd < 0) {
		jr_esprintf (error_buf, "couldn't connect to '%.64s': %s",
			smtp_server_name, error_buf
		);
		return smtp_fd;
	}


	smtp_wfp	= fdopen (smtp_fd, "w");

	if (smtp_wfp == 0) {
		jr_esprintf (error_buf, "couldn't get connection wfp: %s", strerror (errno));
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	status = setvbuf (smtp_wfp, 0, _IOLBF, 0);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get setvbuf(wfp): %s", strerror (errno));
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after connection");
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_INIT_STATUS) {
		jr_esprintf (error_buf, "smtp init status %d != %d", smtp_status, SMTP_INIT_STATUS);
		status = jr_INET_CONNECTION_REFUSED;
		goto return_status;
	}


	smtp_command	= "HELO";
	fprintf (smtp_wfp, "%s %s\r\n", smtp_command, local_hostname);

	/*
	 * Some SMTP servers send multiple 220 init messages.
	 */

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_OK_STATUS  &&  smtp_status != SMTP_INIT_STATUS) {
		jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_OK_STATUS);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	smtp_command	= "MAIL";

	if (sender_address[0] == '<') {
		fprintf (smtp_wfp, "%s From:%s\r\n", smtp_command, sender_address);
	}
	else {
		fprintf (smtp_wfp, "%s From:<%s>\r\n", smtp_command, sender_address);
	}


	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_OK_STATUS) {
		jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_OK_STATUS);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	smtp_command	= "RCPT";

	if (recipient_address[0] == '<') {
		fprintf (smtp_wfp, "%s To:%s\r\n", smtp_command, recipient_address);
	}
	else {
		fprintf (smtp_wfp, "%s To:<%s>\r\n", smtp_command, recipient_address);
	}


	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_OK_STATUS) {
		jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_OK_STATUS);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	smtp_command	= "DATA";
	fprintf (smtp_wfp, "%s\r\n", smtp_command);

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	/*
	 * The RCPT command could have two status responses?
	 */

	switch (smtp_status) {
		case SMTP_OK_STATUS				:
		case SMTP_DATA_OK_STATUS		:
		case SMTP_WILL_FORWARD_STATUS	: {
			break;
		}

		default							: {
			jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_DATA_OK_STATUS);
			status = jr_INET_SYSTEM_ERROR;
			goto return_status;

			break;
		}
	}


	if (opt_header_vector) {
		jr_int			i;

		for (i=0;  opt_header_vector[i] != 0;  i++) {
			fprintf (smtp_wfp, "%s\r\n", opt_header_vector[i]);
		}
		fprintf (smtp_wfp, "\r\n");
	}

	fprintf (smtp_wfp, "%s\r\n.\r\n", message_string);

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_OK_STATUS) {
		jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_OK_STATUS);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	smtp_command	= "QUIT";
	fprintf (smtp_wfp, "%s\r\n", smtp_command);

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_QUIT_OK_STATUS) {
		jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_QUIT_OK_STATUS);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}



	status = 0;

	return_status : {
		if (smtp_fd >= 0) {
			shutdown (smtp_fd, jr_ALL_DISALLOW);
			close (smtp_fd);
		}

		if (smtp_wfp) {
			fclose (smtp_wfp);
		}
	}
			
	return status;
}


jr_int jr_MailStringMessageWithSubjectOnly (
								smtp_server_name, sender_address, recipient_address,
								opt_subject_buf, message_string,
								error_buf
							)
	char *					smtp_server_name;
	char *					sender_address;
	char *					recipient_address;
	char *					opt_subject_buf;
	char *					message_string;
	char *					error_buf;
{
	char *					header_vector[32];
	char					subject_buf[256];
	jr_int					i						= 0;

	jr_int					status;


	if (opt_subject_buf) {
		sprintf (subject_buf, "Subject: %.210s", opt_subject_buf);

		header_vector[i++]		= subject_buf;
		header_vector[i++]		= 0;
	}

	status	= jr_MailStringMessage (
				smtp_server_name, sender_address, recipient_address,
				opt_subject_buf ? header_vector : 0,
				message_string,
				error_buf
			);

	return status;
}


jr_int jr_MailServerValidate (smtp_server_name, opt_sender_address, error_buf)
	const char *			smtp_server_name;
	const char *			opt_sender_address;
	char *					error_buf;
{
	jr_int					smtp_fd				= -1;
	FILE *					smtp_wfp			= 0;
	char *					smtp_command;
	jr_int					smtp_status;

	jr_int					server_is_valid		= 0;

	char					local_hostname [256];
	char					line_buffer [SMTP_MAX_RESPONSE_LENGTH];

	jr_int					status;



	status		= gethostname (local_hostname, sizeof (local_hostname) - 1);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get local host name: %s", strerror (errno));
		status = jr_INET_HOST_NOT_FOUND;
		goto return_status;
	}

	smtp_fd		= jr_InetConnectToService (
					smtp_server_name, SMTP_PORT, error_buf
				);

	if (smtp_fd < 0) {
		jr_esprintf (error_buf, "couldn't connect to '%.64s': %s",
			smtp_server_name, error_buf
		);
		return smtp_fd;
	}


	smtp_wfp	= fdopen (smtp_fd, "w");

	if (smtp_wfp == 0) {
		jr_esprintf (error_buf, "couldn't get connection wfp: %s", strerror (errno));
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	status = setvbuf (smtp_wfp, 0, _IOLBF, 0);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get setvbuf(wfp): %s", strerror (errno));
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after connection");
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_INIT_STATUS) {
		jr_esprintf (error_buf, "smtp init status %d != %d", smtp_status, SMTP_INIT_STATUS);
		status = jr_INET_CONNECTION_REFUSED;
		goto return_status;
	}


	smtp_command	= "HELO";
	fprintf (smtp_wfp, "%s %s\r\n", smtp_command, local_hostname);

	/*
	 * Some SMTP servers send multiple 220 init messages.
	 */

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);


	if (opt_sender_address) {

		if (smtp_status != SMTP_OK_STATUS  &&  smtp_status != SMTP_INIT_STATUS) {
			jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_OK_STATUS);
			status = jr_INET_SYSTEM_ERROR;
			goto return_status;
		}


		smtp_command	= "MAIL";

		if (opt_sender_address[0] == '<') {
			fprintf (smtp_wfp, "%s From:%s\r\n", smtp_command, opt_sender_address);
		}
		else {
			fprintf (smtp_wfp, "%s From:<%s>\r\n", smtp_command, opt_sender_address);
		}

		status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

		if (status < 0) {
			jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
			status = jr_INET_SYSTEM_ERROR;
			goto return_status;
		}

		smtp_status	= atoi (line_buffer);
	}


	if (smtp_status == SMTP_OK_STATUS) {
		server_is_valid	= 1;
	}
	else {
		jr_esprintf (error_buf, "%s", line_buffer);
		server_is_valid	= 0;
	}


	smtp_command	= "QUIT";
	fprintf (smtp_wfp, "%s\r\n", smtp_command);

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		status = server_is_valid;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_QUIT_OK_STATUS) {
		status = server_is_valid;
		goto return_status;
	}


	status = server_is_valid;

	return_status : {
		if (smtp_fd >= 0) {
			shutdown (smtp_fd, jr_ALL_DISALLOW);
			close (smtp_fd);
		}

		if (smtp_wfp) {
			fclose (smtp_wfp);
		}
	}
			
	return status;
}


jr_int jr_MailAddressValidate (
								recipient_address, smtp_server_name, sender_address,
								error_buf
							)
	const char *			recipient_address;
	const char *			smtp_server_name;
	const char *			sender_address;
	char *					error_buf;
{
	jr_int					smtp_fd				= -1;
	FILE *					smtp_wfp			= 0;
	char *					smtp_command;
	jr_int					smtp_status;

	jr_int					address_is_valid	= 0;

	char					local_hostname [256];
	char					line_buffer [SMTP_MAX_RESPONSE_LENGTH];

	jr_int					status;


	status		= gethostname (local_hostname, sizeof (local_hostname) - 1);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get local host name: %s", strerror (errno));
		status = jr_INET_HOST_NOT_FOUND;
		goto return_status;
	}


	smtp_fd		= jr_InetConnectToService (
					smtp_server_name, SMTP_PORT, error_buf
				);

	if (smtp_fd < 0) {
		jr_esprintf (error_buf, "couldn't connect to '%.64s': %s",
			smtp_server_name, error_buf
		);
		return smtp_fd;
	}


	smtp_wfp	= fdopen (smtp_fd, "w");

	if (smtp_wfp == 0) {
		jr_esprintf (error_buf, "couldn't get connection wfp: %s", strerror (errno));
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	status = setvbuf (smtp_wfp, 0, _IOLBF, 0);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get setvbuf(wfp): %s", strerror (errno));
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after connection");
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_INIT_STATUS) {
		jr_esprintf (error_buf, "smtp init status %d != %d", smtp_status, SMTP_INIT_STATUS);
		status = jr_INET_CONNECTION_REFUSED;
		goto return_status;
	}


	smtp_command	= "HELO";
	fprintf (smtp_wfp, "%s %s\r\n", smtp_command, local_hostname);

	/*
	 * Some SMTP servers send multiple 220 init messages.
	 */

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_OK_STATUS  &&  smtp_status != SMTP_INIT_STATUS) {
		jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_OK_STATUS);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	smtp_command	= "MAIL";

	if (sender_address[0] == '<') {
		fprintf (smtp_wfp, "%s From:%s\r\n", smtp_command, sender_address);
	}
	else {
		fprintf (smtp_wfp, "%s From:<%s>\r\n", smtp_command, sender_address);
	}


	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_OK_STATUS) {
		jr_esprintf (error_buf, "%s status %d != %d", smtp_command, smtp_status, SMTP_OK_STATUS);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}


	smtp_command	= "RCPT";

	if (recipient_address[0] == '<') {
		fprintf (smtp_wfp, "%s To:%s\r\n", smtp_command, recipient_address);
	}
	else {
		fprintf (smtp_wfp, "%s To:<%s>\r\n", smtp_command, recipient_address);
	}


	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		jr_esprintf (error_buf, "unexpected EOF after %s", smtp_command);
		status = jr_INET_SYSTEM_ERROR;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status == SMTP_OK_STATUS  ||  smtp_status == SMTP_WILL_FORWARD_STATUS) {
		address_is_valid	= 1;
	}
	else {
		jr_esprintf (error_buf, "%s", line_buffer);
		address_is_valid	= 0;
	}


	smtp_command	= "QUIT";
	fprintf (smtp_wfp, "%s\r\n", smtp_command);

	status	= jr_socket_readlines (smtp_fd, line_buffer, sizeof (line_buffer));

	if (status < 0) {
		status = address_is_valid;
		goto return_status;
	}

	smtp_status	= atoi (line_buffer);

	if (smtp_status != SMTP_QUIT_OK_STATUS) {
		status = address_is_valid;
		goto return_status;
	}


	status = address_is_valid;

	return_status : {
		if (smtp_fd >= 0) {
			shutdown (smtp_fd, jr_ALL_DISALLOW);
			close (smtp_fd);
		}

		if (smtp_wfp) {
			fclose (smtp_wfp);
		}
	}
			
	return status;
}
