#ifndef _jr_poll_proj_h___
#define _jr_poll_proj_h___

#include "ezport.h"

#include <string.h>
#include <apr-1/apr_pools.h>
#include <apr-1/apr_poll.h>
#include <apr-1/apr_errno.h>

#include "jr/poll.h"
#include "jr/apr.h"
#include "jr/error.h"
#include "jr/syscalls.h"
#include "jr/misc.h"

#define MIN_APR_POLLSET_SIZE	8


extern jr_int			jr_poll_add_generic (
							jr_poll_t *					poll_ptr,
							void *						generic_io_ptr,
							jr_int						poll_flags,
							jr_int						(*opt_handler_fn) (
															void *			data_ptr,
															jr_poll_t *		poll_ptr,
															void *			generic_io_ptr,
															jr_int			io_style_ready,
															char *			error_buf
														),
							void *						opt_handler_data_ptr,
							char *						error_buf);

extern void				jr_poll_delete_generic (
							jr_poll_t *					poll_ptr,
							void *						generic_io_ptr,
							jr_int						poll_flags);

extern jr_pollevent_t *	jr_poll_get_new_event (
							jr_poll_t *					poll_ptr,
							void *						generic_io_ptr,
							jr_int						io_style,
							jr_int						(*handler_fn) (
															void *			data_ptr,
															jr_poll_t *		poll_ptr,
															void *			generic_io_ptr,
															jr_int			io_style,
															char *			error_buf
														),
							void *						handler_data_ptr);

extern void				jr_poll_delete_event (
							jr_poll_t *					poll_ptr,
							void *						generic_io_ptr,
							jr_int						poll_flags);

extern void				jr_pollevent_init (
							jr_pollevent_t *			event_ptr,
							void *						generic_io_ptr,
							jr_int						io_style,
							jr_int						(*handler_fn) (
															void *			data_ptr,
															jr_poll_t *		poll_ptr,
															void *			generic_io_ptr,
															jr_int			io_style,
															char *			error_buf
														),
							void *						handler_data_ptr);

extern void				jr_pollevent_undo (
							jr_pollevent_t *			event_ptr);

extern jr_int			jr_pollevent_ptr_cmp (
							const void *				void_ptr_1,
							const void *				void_ptr_2);

extern jr_int			jr_pollevent_ptr_hash (
							const void *				void_ptr);

extern jr_int			jr_pollevent_add_to_pollset (
							jr_pollevent_t *			event_ptr,
							apr_pollset_t *				apr_pollset_ptr,
							char *						error_buf);


#endif
