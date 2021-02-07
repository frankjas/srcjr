#ifndef _JR_EPOLL_H___
#define _JR_EPOLL_H___

#include "ezport.h"

#ifndef missing_epoll_h
#include <sys/epoll.h>
#endif

/******** epoll helper functions ********/

extern void			jr_epoll_event_init_ptr (
						struct epoll_event *		event_info,
						jr_int						event_flags,
						void *						ptr);

#define				jr_epoll_event_ptr(event_info)			\
					((event_info)->data.ptr)

#define				jr_epoll_event_has_read(event_info)		\
					((event_info)->events & EPOLLIN)

#define				jr_epoll_event_has_write(event_info)		\
					((event_info)->events & EPOLLOUT)


/******** epoll replacement ********/

#ifdef missing_epoll_in_kernel

extern jr_int		jr_epoll_close (
						jr_int						epoll_fd);

extern jr_int		jr_epoll_create (
						jr_int						estimated_num_clients);

extern jr_int		jr_epoll_ctl (
						jr_int						epoll_fd,
						jr_int						epoll_op,
						jr_int						io_fd,
						struct epoll_event *		event_info);

extern jr_int		jr_epoll_wait (
						jr_int						epoll_fd,
						struct epoll_event *		event_array,
						jr_int						max_events,
						jr_int						timeout_seconds);

#else

#define				jr_epoll_create(num_clnts)			epoll_create (num_clnts)
#define				jr_epoll_ctl(efd, eop, ifd, eptr)	epoll_ctl (efd, eop, ifd, eptr)
#define				jr_epoll_wait(efd, ea, me, ts)		epoll_wait (efd, ea, me, ts)

#define				jr_epoll_close(epoll_fd)			close (epoll_fd)

#endif

#endif
