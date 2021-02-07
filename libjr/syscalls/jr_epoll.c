#include "ezport.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "jr/epoll.h"
#include "jr/alist.h"
#include "jr/dlist.h"
#include "jr/error.h"

void jr_epoll_event_init_ptr (
	struct epoll_event *		event_info,
	jr_int						event_flags,
	void *						ptr)
{
	memset (event_info, 0, sizeof (*event_info));

	event_info->events			= event_flags;
	event_info->data.ptr		= ptr;
}




typedef struct {
	jr_DList			event_list[1];
} jr_EPollType;

typedef struct {
	jr_int				io_fd;
	struct epoll_event	event_info[1];
} jr_EPollEventType;

extern void			jr_EPollInit (
						jr_EPollType *				epoll_info_ptr,
						jr_int						estimated_num_clients);

extern void			jr_EPollUndo (
						jr_EPollType *				epoll_info_ptr);

extern void			jr_EPollAddEvent (
						jr_EPollType *				epoll_info_ptr,
						jr_int						io_fd,
						struct epoll_event *		event_info);

extern void			jr_EPollEventInit (
						jr_EPollEventType *			epoll_el_ptr,
						jr_int						io_fd,
						struct epoll_event *		event_info);

extern void			jr_EPollEventUndo (
						jr_EPollEventType *			epoll_el_ptr);



void jr_EPollInit (
	jr_EPollType *				epoll_info_ptr,
	jr_int						estimated_num_clients)
{
	memset (epoll_info_ptr, 0, sizeof (*epoll_info_ptr));

	jr_DListInit (epoll_info_ptr->event_list, sizeof (jr_EPollEventType));
}

void jr_EPollUndo (
	jr_EPollType *				epoll_info_ptr)
{
	jr_EPollEventType *			epoll_el_ptr;

	jr_DListForEachElementPtr (epoll_info_ptr->event_list, epoll_el_ptr) {
		jr_EPollEventUndo (epoll_el_ptr);
	}
	jr_DListUndo (epoll_info_ptr->event_list);
}

void jr_EPollAddEvent (
	jr_EPollType *				epoll_info_ptr,
	jr_int						io_fd,
	struct epoll_event *		event_info)
{
	jr_EPollEventType *			epoll_el_ptr;

	jr_DListForEachElementPtr (epoll_info_ptr->event_list, epoll_el_ptr) {
		if (epoll_el_ptr->io_fd == io_fd) {
			jr_EPollEventUndo (epoll_el_ptr);
			jr_EPollEventInit (epoll_el_ptr, io_fd, event_info);
			return;
		}
	}

	epoll_el_ptr	= jr_DListNewTailPtr (epoll_info_ptr->event_list);

	jr_EPollEventInit (epoll_el_ptr, io_fd, event_info);
}

void jr_EPollDeleteEvent (
	jr_EPollType *				epoll_info_ptr,
	jr_int						io_fd)
{
	jr_EPollEventType *			epoll_el_ptr;

	jr_DListForEachElementPtr (epoll_info_ptr->event_list, epoll_el_ptr) {
		if (epoll_el_ptr->io_fd == io_fd) {
			jr_EPollEventUndo (epoll_el_ptr);
			jr_DListDeleteElement (epoll_info_ptr->event_list, epoll_el_ptr);
			return;
		}
	}
}

jr_int jr_EPollWait (
	jr_EPollType *				epoll_info_ptr,
	struct epoll_event *		event_array,
	jr_int						max_events,
	jr_int						timeout_seconds)
{
	jr_EPollEventType *			epoll_el_ptr;
	jr_int						max_fd;
	jr_int						num_fds;
	struct timeval				timeout [1];
	struct timeval *			timeout_ptr			= 0;
	fd_set						read_fd_set[1];
	fd_set						write_fd_set[1];
	jr_int						nready;
	jr_int						num_events;

	for (;;)  {

		FD_ZERO (read_fd_set);
		FD_ZERO (write_fd_set);

		timeout->tv_sec		= timeout_seconds;
		timeout->tv_usec	= 0;

		if (timeout_seconds >= 0) {
			timeout_ptr		= timeout;
		}
		else {
			timeout_ptr		= 0;
		}

		num_fds				= 0;
		max_fd				= -1;

		jr_DListForEachElementPtr (epoll_info_ptr->event_list, epoll_el_ptr) {
			if (epoll_el_ptr->event_info->events & EPOLLIN) {
				FD_SET	(epoll_el_ptr->io_fd, read_fd_set);
			}
			if (epoll_el_ptr->event_info->events & EPOLLOUT) {
				FD_SET	(epoll_el_ptr->io_fd, write_fd_set);
			}
			num_fds++;

			if (epoll_el_ptr->io_fd > max_fd) {
				max_fd	= epoll_el_ptr->io_fd;
			}
		}

		if (max_events < num_fds) {
			/*
			** 8/14/06: Since each file descriptor returned by select() must be examined,
			** the number of events must be >= number of watched fds.
			*/
			errno = EINVAL;
			return jr_INTERNAL_ERROR;
		}

		nready = select (max_fd + 1, read_fd_set, write_fd_set, 0, timeout_ptr);

		if (nready < 0) {
			return jr_INTERNAL_ERROR;
		}
		if (nready == 0) {
			return 0;
		}

		num_events	= 0;

		jr_DListForEachElementPtr (epoll_info_ptr->event_list, epoll_el_ptr) {
			if (	FD_ISSET (epoll_el_ptr->io_fd, read_fd_set)
				||	FD_ISSET (epoll_el_ptr->io_fd, write_fd_set)) {

				event_array[num_events].data	= epoll_el_ptr->event_info->data;
				event_array[num_events].events	= 0;

				if (FD_ISSET (epoll_el_ptr->io_fd, read_fd_set)) {
					event_array[num_events].events	|= EPOLLIN;
				}
				if (FD_ISSET (epoll_el_ptr->io_fd, write_fd_set)) {
					event_array[num_events].events	|= EPOLLOUT;
				}
				num_events++;
			}
		}
		if (num_events > 0) {
			return num_events;
		}
	}
}

void jr_EPollEventInit (
	jr_EPollEventType *			epoll_el_ptr,
	jr_int						io_fd,
	struct epoll_event *		event_info)
{
	memset (epoll_el_ptr, 0, sizeof (*epoll_el_ptr));

	epoll_el_ptr->io_fd			= io_fd;
	memcpy (epoll_el_ptr->event_info, event_info, sizeof (*event_info));
}

void jr_EPollEventUndo (
	jr_EPollEventType *			epoll_el_ptr)
{
}

#ifdef missing_epoll_in_kernel

jr_AList		EPollList[1];
static jr_int	EPollListIsInitialized		= 0;
static jr_int	EPollNumCreates				= 0;

jr_int jr_epoll_create (
	jr_int						estimated_num_clients)
{
	jr_EPollType *				epoll_info_ptr;

	if (!EPollListIsInitialized) {
		jr_AListInit (EPollList, sizeof (jr_EPollType));
		EPollListIsInitialized = 1;
	}

	epoll_info_ptr	= jr_AListNewTailPtr (EPollList);

	jr_EPollInit (epoll_info_ptr, estimated_num_clients);

	EPollNumCreates ++;

	return 0;
}

jr_int jr_epoll_close (
	jr_int						epoll_fd)
{
	jr_EPollType *				epoll_info_ptr;

	if (!EPollListIsInitialized  ||  !jr_AListIsValidIndex (EPollList, epoll_fd)) {
		errno = EBADF;
		return jr_INTERNAL_ERROR;
	}

	epoll_info_ptr	= jr_AListElementPtr (EPollList, epoll_fd);

	jr_EPollUndo (epoll_info_ptr);

	EPollNumCreates --;

	if (EPollNumCreates == 0) {
		jr_AListUndo (EPollList);
		EPollListIsInitialized = 0;
	}
	return 0;
}

jr_int jr_epoll_ctl (
	jr_int						epoll_fd,
	jr_int						epoll_op,
	jr_int						io_fd,
	struct epoll_event *		event_info)
{
	jr_EPollType *				epoll_info_ptr;

	if (!EPollListIsInitialized  ||  !jr_AListIsValidIndex (EPollList, epoll_fd)) {
		errno = EBADF;
		return jr_INTERNAL_ERROR;
	}

	epoll_info_ptr	= jr_AListElementPtr (EPollList, epoll_fd);

	if (epoll_op == EPOLL_CTL_ADD) {
		jr_EPollAddEvent (epoll_info_ptr, io_fd, event_info);
	}
	else if (epoll_op == EPOLL_CTL_MOD) {
		jr_EPollAddEvent (epoll_info_ptr, io_fd, event_info);
	}
	else if (epoll_op == EPOLL_CTL_DEL) {
		jr_EPollDeleteEvent (epoll_info_ptr, io_fd);
	}
	else {
		errno = ENOENT;
		return jr_INTERNAL_ERROR;
	}

	return 0;
}

jr_int jr_epoll_wait (
	jr_int						epoll_fd,
	struct epoll_event *		event_array,
	jr_int						max_events,
	jr_int						timeout_seconds)
{
	jr_EPollType *				epoll_info_ptr;
	jr_int						status;

	if (!EPollListIsInitialized  ||  !jr_AListIsValidIndex (EPollList, epoll_fd)) {
		errno = EBADF;
		return jr_INTERNAL_ERROR;
	}

	epoll_info_ptr	= jr_AListElementPtr (EPollList, epoll_fd);

	status	= jr_EPollWait (epoll_info_ptr, event_array, max_events, timeout_seconds);

	return status;
}

#endif
