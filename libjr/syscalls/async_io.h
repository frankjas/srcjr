#ifndef _jr_async_io_h___
#define _jr_async_io_h___

#define _POSIX_SOURCE		1

#include "ezport.h"

#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/types.h>

#include "jr/error.h"
#include "jr/io.h"
#include "jr/htable.h"


extern jr_int		jr_AddAsyncIOHandler			PROTO ((
						jr_int						fd,
						FILE *						fp,
						void						(*handler_fn) (),
						void *						handler_param,
						jr_int						is_for_reading,
						char *						error_buf
					));

extern void			jr_RemoveAsyncIOHandler			PROTO ((
						jr_int						fd,
						jr_int						is_for_reading
					));



extern void			jr_AsyncIOHandler			PROTO ((
						jr_int					sig_number
					));


typedef struct {
	jr_int			fd;
	FILE *			fp;
	void			(*handler_fn) ();
	void *			handler_param;
	jr_int			is_for_reading;
} jr_AsyncInfoStruct;

extern void			jr_AsyncInfoInit				PROTO ((
						jr_AsyncInfoStruct *		handler_info,
						jr_int						fd,
						FILE *						fp,
						void						(*handler_fn) (),
						void *						handler_param,
						jr_int						is_for_reading
					));

extern void			jr_AsyncInfoUndo				PROTO ((
						jr_AsyncInfoStruct *		handler_info
					));

extern jr_int		jr_AsyncInfoHash				PROTO ((
						const void *				void_arg_1
					));

extern jr_int		jr_AsyncInfoCmp					PROTO ((
						const void *				void_arg_1,
						const void *				void_arg_2
					));



#endif
