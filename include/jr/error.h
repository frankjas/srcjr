#ifndef __jr_error_h___
#define __jr_error_h___

#include "ezport.h"

#define					jr_INTERNAL_ERROR				-1
#define					jr_MISUSE_ERROR					-2
#define					jr_PERMISSION_ERROR				-3
#define					jr_NOT_FOUND_ERROR				-4
#define					jr_IS_FOUND_ERROR				-5
#define					jr_TIME_OUT_ERROR				-6
#define					jr_NO_ROOM_ERROR				-7
#define					jr_IO_ERROR						-8
#define					jr_WOULD_BLOCK_ERROR			-9
#define					jr_EOF_ERROR					-10
#define					jr_INTERRUPTED_ERROR			-11
#define					jr_NOT_IMPLEMENTED_ERROR		-12

#define					jr_MIN_ERROR					-100


extern jr_int			jr_ConvertErrno (
							jr_int						errno_value);

extern jr_int			jr_ConvertAprErrno (
							jr_int						errno_value);

extern jr_int			jr_ConvertLastError (
							jr_int						last_error);

extern jr_int			jr_ConvertHttpError (
							jr_int						http_status);

extern jr_int			jr_ConvertGetHostErrno (
							jr_int						gethost_errno);

extern char *			jr_GetHostErrnoString (
							jr_int						gethost_errno);

#define					jr_ERROR_LEN					512

extern jr_int			jr_esprintf (
							char *						opt_error_buf,
							const char *				control_string, ...)
							__attribute__((format(printf,2,3)));
							/*
							** 11/16/06: helps gcc generate format string warnings?
							*/

extern jr_int			jr_ensprintf (
							char *						opt_error_buf,
							jr_int						error_buf_len,
							const char *				control_string, ...)
							__attribute__((format(printf,3,4)));

extern jr_int			jr_error_set_max_buf_length (
							jr_int						max_buf_length);

extern jr_int			jr_error_max_buf_length		(void);


extern void				jr_error_msg_simulate (
							const char *				file_name,
							jr_int						line_number);

#endif
