#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef ostype_winnt
#	include <winerror.h>
#else
#	include <netdb.h>
#endif

#include <errno.h>

#include "jr/error.h"

jr_int jr_ConvertErrno (
	jr_int						errno_value)
{
	switch (errno_value) {
		case EACCES		: return jr_PERMISSION_ERROR;
		case ESRCH		:
		case ENOENT		: return jr_NOT_FOUND_ERROR;
		case EIO		: return jr_IO_ERROR;
		case EFBIG		:							/* file too big */
		case ENOSPC		: return jr_NO_ROOM_ERROR;	/* no more space on file system */
	}
	return jr_INTERNAL_ERROR;
}

#ifdef ostype_winnt

jr_int jr_ConvertLastError (
	jr_int						last_error)
{
	switch (last_error) {
		case ERROR_ACCESS_DENIED	: return jr_PERMISSION_ERROR;

		case ERROR_FILE_NOT_FOUND	:
		case ERROR_PATH_NOT_FOUND	: return jr_NOT_FOUND_ERROR;

		case ERROR_CRC				:
		case ERROR_READ_FAULT		:
		case ERROR_WRITE_FAULT		:
		case ERROR_GEN_FAILURE		: return jr_IO_ERROR;

		case ERROR_OUTOFMEMORY		:
		case ERROR_NOT_ENOUGH_MEMORY:
		case ERROR_HANDLE_DISK_FULL	: return jr_NO_ROOM_ERROR;
	}
	return jr_INTERNAL_ERROR;
}

jr_int jr_ConvertGetHostErrno (gethost_errno)
	jr_int			gethost_errno;
{
	switch (gethost_errno) {
		case WSAHOST_NOT_FOUND		: return jr_NOT_FOUND_ERROR;
		case WSATRY_AGAIN			: return jr_TIME_OUT_ERROR;
		case WSAENETDOWN			:
		case WSANO_RECOVERY			: return jr_IO_ERROR;
		case WSANO_DATA				: return jr_NOT_FOUND_ERROR;
	}

	return jr_MISUSE_ERROR;
}

char *jr_GetHostErrnoString (gethost_errno)
	jr_int			gethost_errno;
{
	switch (gethost_errno) {
		case WSAHOST_NOT_FOUND		: return "no such host";
		case WSATRY_AGAIN			: return "try again";
		case WSAENETDOWN			: return "network unavailable";
		case WSANO_RECOVERY			: return "unrecoverable name server error";
		case WSANO_DATA				: return "valid hostname, but no address listed";
	}
	return "internal gethost...() error";
}

#else
jr_int jr_ConvertGetHostErrno (gethost_errno)
	jr_int			gethost_errno;
{
	switch (gethost_errno) {
		case HOST_NOT_FOUND		: return jr_NOT_FOUND_ERROR;
		case TRY_AGAIN			: return jr_TIME_OUT_ERROR;
		case NO_RECOVERY		: return jr_IO_ERROR;
		case NO_DATA			: return jr_NOT_FOUND_ERROR;
	}

	return jr_MISUSE_ERROR;
}

char *jr_GetHostErrnoString (gethost_errno)
	jr_int			gethost_errno;
{
	switch (gethost_errno) {
		case HOST_NOT_FOUND		: return "no such host";
		case TRY_AGAIN			: return "try again";
		case NO_RECOVERY		: return "unrecoverable name server error";
		case NO_DATA			: return "valid hostname, but no address listed";
	}

	return "invalid gethost...() error number";
}


#endif
