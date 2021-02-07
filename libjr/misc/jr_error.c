#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "jr/trace.h"
#include "jr/misc.h"
#include "jr/malloc.h"
#include "jr/error.h"

static jr_int		jr_ErrorBufMaxLength			= jr_ERROR_LEN;

static jr_int		jr_ErrorWasSimulated			= 0;
static const char *	jr_ErrorSimulatedFileName		= "";
static jr_int		jr_ErrorSimulatedLineNumber		= 0;


jr_int jr_esprintf (
	char *					opt_error_buf,
	const char *			control_string, ...)
{
	char *					tmp_error_buf;
	va_list					arg_list;
	jr_int					status;

	if (opt_error_buf == 0) {
		return 0;
	}

	tmp_error_buf		= jr_malloc (jr_ErrorBufMaxLength);

	if (jr_ErrorWasSimulated) {
		status	= snprintf (opt_error_buf, jr_ErrorBufMaxLength, "simulated error %.64s:%d",
					jr_ErrorSimulatedFileName, jr_ErrorSimulatedLineNumber
				);

		jr_ErrorWasSimulated = 0;
	}
	else {

		/*
		 * Could do optimization for control strings without format specifiers.
		 * Then we wouldn't need the local_error_buf because there is no race condition.
		 * But the optimization may require as much work than the copy (linear time).
		 */
		va_start (arg_list, control_string);

		status	= vsnprintf (tmp_error_buf, jr_ErrorBufMaxLength, control_string, arg_list);

		if (status >= jr_ErrorBufMaxLength) {
			tmp_error_buf[ jr_ErrorBufMaxLength-1 ]	= 0;
		}

		strcpy (opt_error_buf, tmp_error_buf);

		va_end (arg_list);
	}

	if (status > jr_ErrorBufMaxLength - 1) {
		status = jr_ErrorBufMaxLength - 1;
	}
	jr_free (tmp_error_buf);
	return status;
}

jr_int jr_ensprintf (
	char *					opt_error_buf,
	jr_int					error_buf_len,
	const char *			control_string, ...)
{
	va_list					arg_list;

	char *					tmp_error_buf;
	jr_int					status;


	if (opt_error_buf == 0) {
		return 0;
	}

	tmp_error_buf		= jr_malloc (error_buf_len);

	if (jr_ErrorWasSimulated) {
		status	= snprintf (tmp_error_buf, error_buf_len, "simulated error %.64s:%d",
					jr_ErrorSimulatedFileName, jr_ErrorSimulatedLineNumber
				);

		jr_ErrorWasSimulated = 0;
	}
	else {

		va_start (arg_list, control_string);

		status	= vsnprintf (tmp_error_buf, error_buf_len, control_string, arg_list);

		if (status >= error_buf_len) {
			tmp_error_buf[ error_buf_len - 1 ]  = 0;
		}
		strcpy (opt_error_buf, tmp_error_buf);

		va_end (arg_list);
	}

	if (status > error_buf_len - 1) {
		status = error_buf_len - 1;
	}

	jr_free (tmp_error_buf);

	return status;
}


jr_int jr_error_set_max_buf_length (
	jr_int					max_buf_length)
{
	if (max_buf_length <= jr_ERROR_LEN) {
		jr_ErrorBufMaxLength	= max_buf_length;
	}

	return jr_ErrorBufMaxLength;
}

jr_int jr_error_max_buf_length (void)
{
	return jr_ErrorBufMaxLength;
}


void jr_error_msg_simulate (
	const char *			file_name,
	jr_int					line_number)
{
	jr_ErrorWasSimulated		= 1;
	jr_ErrorSimulatedFileName	= file_name;
	jr_ErrorSimulatedLineNumber	= line_number;
}

