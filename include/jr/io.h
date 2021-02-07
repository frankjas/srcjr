#ifndef __JR_IO_H__
#define __JR_IO_H__

#include "ezport.h"

#include <stdio.h>
#include <sys/types.h>

#include "jr/error.h"

#ifndef jr_MAXNAMLEN
#	define jr_MAXNAMLEN			244
#endif

#ifndef jr_MAXPATHLEN
#	define jr_MAXPATHLEN		500
#endif

#ifndef jr_MAXARGS
#	define jr_MAXARGS			500
#endif


extern jr_int		jr_Timed_fgets			PROTO((
							char *			buffer,
							jr_int			length,
							FILE *  		rfp,
							jr_int    		seconds
					)) ;

extern jr_int		jr_GetWholeLine			PROTO ((
						char **				line_buf_ref,
						jr_int *			line_buf_size_ref,
						FILE *				rfp
					));



extern jr_int		jr_FilePtrCopy			PROTO ((
							FILE *			wfp,
							FILE *			rfp,
							char *			error_buf
					)) ;

extern int			jr_FileNameCopy			PROTO ((
						const char *		dest_file_name,
						const char *		src_file_name,
						char *				error_buf
					));

extern jr_int		jr_FirstLineOfFileName					PROTO((
							const char *	rfn,
							char *			result_buf,
							jr_int			result_buf_length
					)) ;


extern void			jr_PrintArbitraryData			PROTO ((
						FILE *						wfp,
						const void *				data_ptr,
						jr_int						data_length,
						jr_int						max_width
					));

extern void *		jr_FileNameReadIntoNewMemory	PROTO((
							const char *			rfn,
							unsigned jr_int *		length_ptr
					)) ;
extern void *		jr_FilePtrReadIntoNewMemory		PROTO((
							FILE *					rfp,
							unsigned jr_int *		length_ptr
					)) ;

/******** Path DB ********/

#define				jr_PATH_DB_CONF_FILE		"PathDB.cnf"
#define				jr_PATH_DB_CREATE_FLAG		0x0001
#define				jr_PATH_DB_FILE_FLAG		0x0002
#define				jr_PATH_DB_DIRECTORY_FLAG	0x0004


extern jr_int		jr_PathDbMakePath (
						const char *				base_path,
						jr_int						numeric_value,
						jr_int						value_base,
						jr_int						path_flags,
						const char **				balanced_path_ref,
						char *						error_buf);

/******** URL Library ********/

extern jr_int		jr_URL_VerifyExistence		PROTO ((
						char *					machine_name,
						jr_int					port_number,
						char *					url_string,
						char *					error_buf
					));


#define				jr_HTTP_CONTINUE_STATUS				100

#define				jr_HTTP_SUCCESS_STATUS				200
#define				jr_HTTP_CREATED_STATUS				201
#define				jr_HTTP_ACCEPTED_STATUS				202
#define				jr_HTTP_NO_CONTENT_STATUS			204

#define				jr_HTTP_MOVED_PERMANENTLY_STATUS	301
#define				jr_HTTP_FOUND_STATUS				302
#define				jr_HTTP_NOT_MODIFIED_STATUS			304

#define				jr_HTTP_BAD_REQUEST_STATUS			400
#define				jr_HTTP_UNAUTHORIZED_STATUS			401
#define				jr_HTTP_FORBIDDEN_STATUS			403
#define				jr_HTTP_NOT_FOUND_STATUS			404
#define				jr_HTTP_URI_TOO_LONG_STATUS			414

#define				jr_HTTP_SERVER_ERROR_STATUS			500
#define				jr_HTTP_NOT_IMPLEMENTED_STATUS		501
#define				jr_HTTP_BAD_GATEWAY_STATUS			502
#define				jr_HTTP_SERVICE_UNAVAILABLE_STATUS	503

extern char *		jr_HTTP_StatusString	PROTO ((
						jr_int				http_status
					));

#endif
