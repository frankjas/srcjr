#define _POSIX_SOURCE

#include "jr/io.h"

char *jr_HTTP_StatusString (http_status)
	jr_int				http_status;
{
	switch (http_status) {
		case jr_HTTP_CONTINUE_STATUS			: return "Continue";

		case jr_HTTP_SUCCESS_STATUS				: return "Success";
		case jr_HTTP_CREATED_STATUS				: return "Created";
		case jr_HTTP_ACCEPTED_STATUS			: return "Accepted";
		case jr_HTTP_NO_CONTENT_STATUS			: return "No Content";

		case jr_HTTP_MOVED_PERMANENTLY_STATUS	: return "Moved Permanently";
		case jr_HTTP_FOUND_STATUS				: return "Found";
		case jr_HTTP_NOT_MODIFIED_STATUS		: return "Not Modified";

		case jr_HTTP_BAD_REQUEST_STATUS			: return "Bad Request";
		case jr_HTTP_UNAUTHORIZED_STATUS		: return "Unauthorized";
		case jr_HTTP_FORBIDDEN_STATUS			: return "Forbidden";
		case jr_HTTP_NOT_FOUND_STATUS			: return "Not Found";
		case jr_HTTP_URI_TOO_LONG_STATUS		: return "URI Too Long";

		case jr_HTTP_SERVER_ERROR_STATUS		: return "Internal Server Error";
		case jr_HTTP_NOT_IMPLEMENTED_STATUS		: return "Not Implemented";
		case jr_HTTP_BAD_GATEWAY_STATUS			: return "Bad Gateway";
		case jr_HTTP_SERVICE_UNAVAILABLE_STATUS	: return "Service Unavailable";
	}
	return "Unknown Status";
}

jr_int jr_ConvertHttpError(
	jr_int				http_status)
{
	switch (http_status) {
		case jr_HTTP_CONTINUE_STATUS			: return 0;

		case jr_HTTP_SUCCESS_STATUS				: return 0;
		case jr_HTTP_CREATED_STATUS				: return 0;
		case jr_HTTP_ACCEPTED_STATUS			: return 0;
		case jr_HTTP_NO_CONTENT_STATUS			: return 0;

		case jr_HTTP_MOVED_PERMANENTLY_STATUS	: return 0;
		case jr_HTTP_FOUND_STATUS				: return 0;
		case jr_HTTP_NOT_MODIFIED_STATUS		: return 0;

		case jr_HTTP_BAD_REQUEST_STATUS			: return jr_MISUSE_ERROR;
		case jr_HTTP_UNAUTHORIZED_STATUS		: return jr_PERMISSION_ERROR;
		case jr_HTTP_FORBIDDEN_STATUS			: return jr_PERMISSION_ERROR;
		case jr_HTTP_NOT_FOUND_STATUS			: return jr_NOT_FOUND_ERROR;
		case jr_HTTP_URI_TOO_LONG_STATUS		: return jr_MISUSE_ERROR;

		case jr_HTTP_SERVER_ERROR_STATUS		: return jr_INTERNAL_ERROR;
		case jr_HTTP_NOT_IMPLEMENTED_STATUS		: return jr_NOT_IMPLEMENTED_ERROR;
		case jr_HTTP_BAD_GATEWAY_STATUS			: return jr_INTERNAL_ERROR;
		case jr_HTTP_SERVICE_UNAVAILABLE_STATUS	: return jr_INTERNAL_ERROR;
	}
	return jr_INTERNAL_ERROR;
}

#if 0
/*
** 5-3-2013: obsoleted by jr_ConvertHttpError() which doesn't depend on jr/http/lib.h
*/
jr_int jr_InetConvertHttpError(
	jr_int				http_status)
{
	switch (http_status) {
		case jr_HTTP_CONTINUE_STATUS			: return 0;

		case jr_HTTP_SUCCESS_STATUS				: return 0;
		case jr_HTTP_CREATED_STATUS				: return 0;
		case jr_HTTP_ACCEPTED_STATUS			: return 0;
		case jr_HTTP_NO_CONTENT_STATUS			: return 0;

		case jr_HTTP_MOVED_PERMANENTLY_STATUS	: return 0;
		case jr_HTTP_FOUND_STATUS				: return 0;
		case jr_HTTP_NOT_MODIFIED_STATUS		: return 0;

		case jr_HTTP_BAD_REQUEST_STATUS			: return jr_HTTP_BAD_REQUEST_ERROR;
		case jr_HTTP_UNAUTHORIZED_STATUS		: return jr_PERMISSION_ERROR;
		case jr_HTTP_FORBIDDEN_STATUS			: return jr_INTERNAL_ERROR;
		case jr_HTTP_NOT_FOUND_STATUS			: return jr_NOT_FOUND_ERROR;
		case jr_HTTP_URI_TOO_LONG_STATUS		: return jr_HTTP_INPUT_TOO_LONG_ERROR;

		case jr_HTTP_SERVER_ERROR_STATUS		: return jr_INTERNAL_ERROR;
		case jr_HTTP_NOT_IMPLEMENTED_STATUS		: return jr_NOT_IMPLEMENTED_ERROR;
		case jr_HTTP_BAD_GATEWAY_STATUS			: return jr_INTERNAL_ERROR;
		case jr_HTTP_SERVICE_UNAVAILABLE_STATUS	: return jr_HTTP_CONNECTION_REFUSED;
	}
	return jr_INTERNAL_ERROR;
}
#endif
