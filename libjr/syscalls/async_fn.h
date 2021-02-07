#ifndef __async_fn_h___
#define __async_fn_h___

#define _POSIX_SOURCE		1

#include "ezport.h"

#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "jr/error.h"
#include "jr/errsim.h"
#include "jr/syscalls.h"
#include "jr/avl_tree.h"

typedef struct {
	jr_int						call_time;
	void						(*handler_fn) ();
	void *						handler_param;
	jr_int						num_seconds;
	unsigned					do_repeat				: 1;
} jr_AsyncFunctionStruct;

extern void				jr_AsyncFunctionHandler (
							jr_int						signum);

extern void				jr_AsyncFunctionInit (
							jr_AsyncFunctionStruct *	handler_info_ptr,
							void						(*handler_fn) (),
							void *						handler_param,
							jr_int						num_seconds,
							jr_int						do_repeat);

extern void				jr_AsyncFunctionUndo (
							jr_AsyncFunctionStruct *	handler_info_ptr);

extern jr_int			jr_AsyncFunctionCmp (
							const void *				void_arg_1,
							const void *				void_arg_2);

#define					jr_AsyncFunctionPtr(handler_info_ptr)				\
						((handler_info_ptr)->handler_fn)

#define					jr_AsyncFunctionParam(handler_info_ptr)				\
						((handler_info_ptr)->handler_param)

#define					jr_AsyncFunctionCallTime(handler_info_ptr)			\
						((handler_info_ptr)->call_time)

#define					jr_AsyncFunctionNumSeconds(handler_info_ptr)		\
						((handler_info_ptr)->num_seconds)

#define					jr_AsyncFunctionDoRepeat(handler_info_ptr)			\
						((handler_info_ptr)->do_repeat)

#define					jr_AsyncFunctionSetCallTime(handler_info_ptr, v)	\
						((handler_info_ptr)->call_time	= (v))


#endif
