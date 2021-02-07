#ifndef __jr_poll_h___
#define __jr_poll_h___

#include "ezport.h"

#include "jr/htable.h"
#include "jr/dlist.h"
#include "jr/time.h"
#include "jr/apr.h"

/*
** 2/22/07: Separate from jr/apr.h due to the additional data structures.
*/

/******** Polled I/O ********/

typedef struct jr_poll {
	void *				apr_pollset_ptr;
	void *				apr_pool_ptr;

	jr_int				num_descriptors;
	jr_int				max_descriptors;

	jr_DList			event_list[1];
	jr_HTable			event_table[1];

	jr_AList			result_list[1];

	unsigned			can_wakeup			: 1;
} jr_poll_t;

#define					jr_POLL_READ					0x0001
#define					jr_POLL_WRITE					0x0002
#define					jr_POLL_HUP						0x0004
#define					jr_POLL_CLOSE					0x0008

#define					jr_POLL_IS_SOCKET				0x0100
#define					jr_POLL_IS_FILE					0x0200

#define					jr_POLL_IS_MASK					0x0f00


typedef	jr_int			(*jr_poll_handler_fn_t) (
							void *			data_ptr,
							jr_poll_t *		poll_ptr,
							void *			generic_io_ptr,
							jr_int			io_style_ready,
							char *			error_buf
						);


typedef struct jr_pollevent {
	void *				generic_io_ptr;
	jr_int				poll_flags;
	jr_int				io_style_ready;
	jr_int				(*opt_handler_fn) (
							void *			data_ptr,
							jr_poll_t *		poll_ptr,
							void *			generic_io_ptr,
							jr_int			io_style_ready,
							char *			error_buf
						);
	void *				opt_handler_data_ptr;
} jr_pollevent_t;



extern void				jr_poll_init (
							jr_poll_t *					poll_ptr);

extern void				jr_poll_undo (
							jr_poll_t *					poll_ptr);

extern jr_int			jr_poll_wait (
							jr_poll_t *					poll_ptr,
							jr_useconds_t *				opt_interval_ptr,
							char *						error_buf);

extern void				jr_poll_wakeup (
							jr_poll_t *					poll_ptr);

extern jr_int			jr_poll_add_socket (
							jr_poll_t *					poll_ptr,
							jr_socket_t *				socket_ptr,
							jr_int						poll_flags,
							jr_int						(*opt_handler_fn) (
															void *			data_ptr,
															jr_poll_t *		poll_ptr,
															jr_socket_t *	socket_ptr,
															jr_int			is_style_ready,
															char *			error_buf
														),
							void *						opt_handler_data_ptr,
							char *						error_buf);

extern jr_int			jr_poll_add_file (
							jr_poll_t *					poll_ptr,
							jr_file_t *					file_ptr,
							jr_int						poll_flags,
							jr_int						(*opt_handler_fn) (
															void *			data_ptr,
															jr_poll_t *		poll_ptr,
															jr_file_t *		file_ptr,
															jr_int			is_style_ready,
															char *			error_buf
														),
							void *						opt_handler_data_ptr,
							char *						error_buf);


extern void				jr_poll_delete_socket (
							jr_poll_t *					poll_ptr,
							jr_socket_t *				socket_ptr);

extern void				jr_poll_delete_file (
							jr_poll_t *					poll_ptr,
							jr_file_t *					file_ptr);

#define					jr_poll_set_can_wakeup( poll_ptr, v)					\
						((poll_ptr)->can_wakeup = (v) != 0)

#define					jr_poll_can_wakeup( poll_ptr)							\
						((poll_ptr)->can_wakeup)

#define					jr_poll_foreach_event_ptr(poll_ptr, event_ptr)			\
						jr_AListForEachElementPtr ((poll_ptr)->result_list, event_ptr)

#define					jr_poll_num_events( poll_ptr)							\
						jr_AListSize( (poll_ptr)->result_list)

#define					jr_poll_get_event_ptr( poll_ptr, index)					\
						jr_AListElementPtr( (poll_ptr)->result_list, index)

extern jr_int			jr_poll_call_handler (
							jr_poll_t *					poll_ptr,
							jr_pollevent_t *			event_ptr,
							char *						error_buf);

extern jr_int			jr_poll_increase_capacity (
							jr_poll_t *					poll_ptr,
							jr_int						new_size,
							char *						error_buf);

#define					jr_pollevent_socket_ptr( event_ptr)						\
						((jr_socket_t *) (event_ptr)->generic_io_ptr)

#define					jr_pollevent_file_ptr( event_ptr)						\
						((jr_file_t *) (event_ptr)->generic_io_ptr)

#define					jr_pollevent_handler_fn( event_ptr)						\
						((event_ptr)->opt_handler_fn)

#define					jr_pollevent_handler_data_ptr( event_ptr)				\
						((event_ptr)->opt_handler_data_ptr)

#endif
