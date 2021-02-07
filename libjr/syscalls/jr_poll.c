#include "jr_poll.h"

void jr_poll_init (
	jr_poll_t *					poll_ptr)
{
	memset (poll_ptr, 0, sizeof (jr_poll_t));

	jr_apr_initialize();

	jr_HTableInit (
		poll_ptr->event_table, sizeof (jr_pollevent_t *), jr_pollevent_ptr_hash, jr_pollevent_ptr_cmp
	);
	jr_DListInit (poll_ptr->event_list, sizeof (jr_pollevent_t));

	jr_AListInit (poll_ptr->result_list, sizeof (jr_pollevent_t));
}
	

void jr_poll_undo (
	jr_poll_t *					poll_ptr)
{
	jr_pollevent_t *			event_ptr;
	char						error_buf[jr_ERROR_LEN];

	jr_HTableUndo (poll_ptr->event_table);

	jr_DListForEachElementPtr (poll_ptr->event_list, event_ptr) {

		event_ptr->io_style_ready	= jr_POLL_CLOSE;
		jr_poll_call_handler (poll_ptr, event_ptr, error_buf);

		jr_pollevent_undo (event_ptr);
	}
	jr_DListUndo (poll_ptr->event_list);

	jr_AListUndo (poll_ptr->result_list);

	if (poll_ptr->apr_pollset_ptr) {
		apr_pollset_destroy (poll_ptr->apr_pollset_ptr);
	}
	if (poll_ptr->apr_pool_ptr) {
		apr_pool_destroy (poll_ptr->apr_pool_ptr);
	}
}


jr_int jr_poll_add_generic (
	jr_poll_t *					poll_ptr,
	void *						generic_io_ptr,
	jr_int						poll_flags,
	jr_int						(*opt_handler_fn) (
									void *			data_ptr,
									jr_poll_t *		poll_ptr,
									void *			generic_io_ptr,
									jr_int			is_style_ready,
									char *			error_buf
								),
	void *						opt_handler_data_ptr,
	char *						error_buf)
{
	jr_pollevent_t *			event_ptr;
	jr_int						status;

	if (poll_ptr->apr_pool_ptr == 0) {
		status	= apr_pool_create ((apr_pool_t **) &poll_ptr->apr_pool_ptr, NULL);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create pool: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}
	}
	if (poll_ptr->apr_pollset_ptr == 0) {
		unsigned jr_int			pollset_flags		= 0;

		if (jr_poll_can_wakeup( poll_ptr)) {
			pollset_flags	|= APR_POLLSET_WAKEABLE;
		}

		status	= apr_pollset_create (
					(apr_pollset_t **) &poll_ptr->apr_pollset_ptr,
					MIN_APR_POLLSET_SIZE,
					poll_ptr->apr_pool_ptr,
					pollset_flags
				);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't create apr_pollset_t: %s",
				jr_apr_strerror( status, error_buf)
			);
			status	= jr_ConvertAprErrno (status);
			goto return_status;
		}

		poll_ptr->max_descriptors	= MIN_APR_POLLSET_SIZE;
	}

	if (poll_ptr->num_descriptors >= poll_ptr->max_descriptors) {
		status	= jr_poll_increase_capacity (poll_ptr, poll_ptr->num_descriptors + 1, error_buf);

		if (status != 0) {
			goto return_status;
		}
	}

	event_ptr	= jr_poll_get_new_event (
					poll_ptr, generic_io_ptr, poll_flags, opt_handler_fn, opt_handler_data_ptr
				);

	if (event_ptr == 0) {
		jr_esprintf (error_buf, "already exists");
		status	= jr_IS_FOUND_ERROR;
		goto return_status;
	}

	status		= jr_pollevent_add_to_pollset (
					event_ptr, poll_ptr->apr_pollset_ptr, error_buf
				);

	if (status != 0) {
		goto return_status;
	}

	poll_ptr->num_descriptors++;


	status = 0;

	return_status : {
	}
	return status;
}


jr_int jr_poll_add_socket (
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
	char *						error_buf)
{
	jr_int						status;

	status	= jr_poll_add_generic (
				poll_ptr, socket_ptr, poll_flags | jr_POLL_IS_SOCKET,
				(jr_poll_handler_fn_t) opt_handler_fn, opt_handler_data_ptr, error_buf
			);

	return status;
}

jr_int jr_poll_add_file (
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
	char *						error_buf)
{
	jr_int						status;

	status	= jr_poll_add_generic (
				poll_ptr, file_ptr, poll_flags | jr_POLL_IS_FILE,
				(jr_poll_handler_fn_t) opt_handler_fn, opt_handler_data_ptr, error_buf
			);

	return status;
}

void jr_poll_delete_generic (
	jr_poll_t *					poll_ptr,
	void *						generic_io_ptr,
	jr_int						poll_flags)
{
	apr_pollfd_t				apr_pollfd;

	jr_poll_delete_event (poll_ptr, generic_io_ptr, poll_flags);

	if (poll_ptr->apr_pollset_ptr) {
		memset (&apr_pollfd, 0, sizeof (apr_pollfd_t));

		if (poll_flags & jr_POLL_IS_SOCKET) {
			apr_pollfd.desc_type	= APR_POLL_SOCKET;
			apr_pollfd.desc.s		= jr_socket_apr_ptr ((jr_socket_t *) generic_io_ptr);
		}
		if (poll_flags & jr_POLL_IS_FILE) {
			apr_pollfd.desc_type	= APR_POLL_FILE;
			apr_pollfd.desc.f		= jr_file_apr_ptr ((jr_file_t *) generic_io_ptr);
		}

		apr_pollset_remove (poll_ptr->apr_pollset_ptr, &apr_pollfd);
		/*
		** 2/27/07: Only returns success or not-found, doesn't distinguish between READ/WRITE
		*/
	}
}


void jr_poll_delete_socket (
	jr_poll_t *					poll_ptr,
	jr_socket_t *				socket_ptr)
{
	jr_poll_delete_generic (poll_ptr, socket_ptr, jr_POLL_IS_SOCKET);
}

void jr_poll_delete_file (
	jr_poll_t *					poll_ptr,
	jr_file_t *					file_ptr)
{
	jr_poll_delete_generic (poll_ptr, file_ptr, jr_POLL_IS_FILE);
}

jr_int jr_poll_increase_capacity (
	jr_poll_t *					poll_ptr,
	jr_int						new_size,
	char *						error_buf)
{
	apr_pollset_t *				new_pollset_ptr				= 0;
	unsigned jr_int				poll_flags		= 0;
	jr_pollevent_t *			event_ptr;
	jr_int						status;

	if (new_size < poll_ptr->max_descriptors) {
		return 0;
	}
	if (new_size < 2 * poll_ptr->max_descriptors) {
		new_size	= 2 * poll_ptr->max_descriptors;
	}

	if (jr_poll_can_wakeup( poll_ptr)) {
		poll_flags	|= APR_POLLSET_WAKEABLE;
	}

	status	= apr_pollset_create (
				(apr_pollset_t **) &new_pollset_ptr, new_size, poll_ptr->apr_pool_ptr, poll_flags
			);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't create apr_pollset_t: %s",
			jr_apr_strerror( status, error_buf)
		);
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	jr_DListForEachElementPtr (poll_ptr->event_list, event_ptr) {
		status		= jr_pollevent_add_to_pollset (
						event_ptr, new_pollset_ptr, error_buf
					);

		if (status != 0) {
			goto return_status;
		}
	}

	if (poll_ptr->apr_pollset_ptr) {
		apr_pollset_destroy (poll_ptr->apr_pollset_ptr);
	}

	poll_ptr->apr_pollset_ptr	= new_pollset_ptr;
	poll_ptr->max_descriptors	= new_size;

	new_pollset_ptr				= 0;

	status	= 0;

	return_status : {
		if (new_pollset_ptr) {
			apr_pollset_destroy (new_pollset_ptr);
		}
	}
	return status;
}

jr_pollevent_t *jr_poll_get_new_event (
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
	void *						opt_handler_data_ptr)
{
	jr_pollevent_t *			event_ptr;
	jr_int						status;

	event_ptr	= jr_DListNewTailPtr (poll_ptr->event_list);

	jr_pollevent_init (event_ptr, generic_io_ptr, poll_flags, opt_handler_fn, opt_handler_data_ptr);

	
	status		= jr_HTableSetNewElementIndex (poll_ptr->event_table, &event_ptr);

	if (status < 0) {
		jr_pollevent_undo (event_ptr);
		jr_DListDeleteElement (poll_ptr->event_list, event_ptr);
		
		event_ptr	= 0;
	}

	return event_ptr;
}

void jr_poll_delete_event (
	jr_poll_t *					poll_ptr,
	void *						generic_io_ptr,
	jr_int						poll_flags)
{
	jr_pollevent_t				event_info[1];
	jr_pollevent_t *			event_ptr			= event_info;
	jr_pollevent_t **			event_ref;
	jr_int						index;

	jr_pollevent_init (event_info, generic_io_ptr, poll_flags, 0, 0);

	index	= jr_HTableFindElementIndex (poll_ptr->event_table, &event_ptr);

	if (index >= 0) {

		event_ref	= jr_HTableElementPtr (poll_ptr->event_table, index);
		jr_HTableDeleteIndex (poll_ptr->event_table,  index);
		jr_DListDeleteElement (poll_ptr->event_list, *event_ref);
		
		event_ptr	= 0;
	}
	jr_pollevent_undo (event_info);
}


jr_int jr_poll_wait (
	jr_poll_t *					poll_ptr,
	jr_useconds_t *				opt_interval_ptr,
	char *						error_buf)
{
	const apr_pollfd_t *		apr_pollfd_array;
	apr_interval_time_t			apr_interval;
	jr_pollevent_t *				event_ptr;
	jr_int						num_ready;
	jr_int						q;
	jr_int						status;


	if (poll_ptr->apr_pollset_ptr == 0) {
		jr_esprintf( error_buf, "no items to poll");
		status = jr_MISUSE_ERROR;
		goto return_status;
	}

	if (opt_interval_ptr) {
		apr_interval	= jr_useconds_to_64 (opt_interval_ptr);
	}
	else {
		apr_interval	= -1;
	}

	status	= apr_pollset_poll (
				poll_ptr->apr_pollset_ptr, apr_interval, &num_ready, &apr_pollfd_array
			);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	jr_AListEmpty (poll_ptr->result_list);

	for (q=0;  q < num_ready;  q++) {
		event_ptr	= apr_pollfd_array[q].client_data;

		event_ptr->io_style_ready	= 0;

		if (apr_pollfd_array[q].rtnevents & APR_POLLIN) {
			event_ptr->io_style_ready	|= jr_POLL_READ;
		}
		if (apr_pollfd_array[q].rtnevents & APR_POLLOUT) {
			event_ptr->io_style_ready	|= jr_POLL_WRITE;
		}
		/*
		** 3/28/09: May not need the HUP events, since under Linux
		** a shutdown of one side generates a READ event on the other
		** with 0 data.
		*/
		if (apr_pollfd_array[q].rtnevents & APR_POLLHUP) {
			event_ptr->io_style_ready	|= jr_POLL_HUP;
		}
		/*
		** 3-11-2011: APR_POLLRDHUP is added by the epoll-rdhup.patch.
		*/
		if (apr_pollfd_array[q].rtnevents & APR_POLLRDHUP) {
			event_ptr->io_style_ready	|= jr_POLL_HUP;
		}
		jr_AListSetNewTail (poll_ptr->result_list, event_ptr);
	}

	status	= jr_AListSize (poll_ptr->result_list);

	return_status : {
	}
	return status;
}

void jr_poll_wakeup (
	jr_poll_t *					poll_ptr)
{
	if (poll_ptr->apr_pollset_ptr) {
		apr_pollset_wakeup ( poll_ptr->apr_pollset_ptr);
	}
}

void jr_pollevent_init (
	jr_pollevent_t *			event_ptr,
	void *						generic_io_ptr,
	jr_int						poll_flags,
	jr_int						(*opt_handler_fn) (
									void *			data_ptr,
									jr_poll_t *		poll_ptr,
									void *			generic_io_ptr,
									jr_int			io_style_ready,
									char *			error_buf
								),
	void *						opt_handler_data_ptr)
{
	memset (event_ptr, 0, sizeof (*event_ptr));

	event_ptr->generic_io_ptr		= generic_io_ptr;
	event_ptr->poll_flags			= poll_flags;
	event_ptr->opt_handler_fn		= opt_handler_fn;
	event_ptr->opt_handler_data_ptr	= opt_handler_data_ptr;
}

void jr_pollevent_undo (
	jr_pollevent_t *				event_ptr)
{
}

jr_int jr_pollevent_ptr_hash (
	const void *				void_ptr)
{
	const jr_pollevent_t *		event_ptr			= *(jr_pollevent_t **)void_ptr;
	jr_int						hash_value			= 0;

	if (event_ptr->poll_flags && jr_POLL_IS_SOCKET) {
		hash_value	+= jr_socket_hash (event_ptr->generic_io_ptr);
	}
	if (event_ptr->poll_flags && jr_POLL_IS_FILE) {
		hash_value	+= jr_file_hash (event_ptr->generic_io_ptr);
	}

	return hash_value;
}

jr_int jr_pollevent_ptr_cmp (
	const void *				void_ptr_1,
	const void *				void_ptr_2)
{
	const jr_pollevent_t *		event_ptr_1			= *(jr_pollevent_t **)void_ptr_1;
	const jr_pollevent_t *		event_ptr_2			= *(jr_pollevent_t **)void_ptr_2;
	jr_int						diff;

	diff	= (event_ptr_1->poll_flags & jr_POLL_IS_MASK)  -  (event_ptr_2->poll_flags & jr_POLL_IS_MASK);

	if (diff != 0) {
		return diff;
	}

	
	if ((event_ptr_1->poll_flags & jr_POLL_IS_SOCKET)  &&  (event_ptr_2->poll_flags & jr_POLL_IS_SOCKET)) {
		diff	= jr_socket_cmp (event_ptr_1->generic_io_ptr, event_ptr_2->generic_io_ptr);

		if (diff != 0) {
			return diff;
		}
	}

	if ((event_ptr_1->poll_flags & jr_POLL_IS_FILE)  &&  (event_ptr_2->poll_flags & jr_POLL_IS_FILE)) {
		diff	= jr_file_cmp (event_ptr_1->generic_io_ptr, event_ptr_2->generic_io_ptr);

		if (diff != 0) {
			return diff;
		}
	}

	return 0;
}

jr_int jr_pollevent_add_to_pollset (
	jr_pollevent_t *			event_ptr,
	apr_pollset_t *				apr_pollset_ptr,
	char *						error_buf)
{
	apr_pollfd_t				apr_pollfd;
	jr_int						status;

	memset (&apr_pollfd, 0, sizeof (apr_pollfd_t));

	if (event_ptr->poll_flags & jr_POLL_IS_SOCKET) {
		apr_pollfd.desc_type	= APR_POLL_SOCKET;
		apr_pollfd.desc.s		= jr_socket_apr_ptr ((jr_socket_t *) event_ptr->generic_io_ptr);
	}
	if (event_ptr->poll_flags & jr_POLL_IS_FILE) {
		apr_pollfd.desc_type	= APR_POLL_FILE;
		apr_pollfd.desc.f		= jr_file_apr_ptr ((jr_file_t *) event_ptr->generic_io_ptr);
	}

	if (event_ptr->poll_flags & jr_POLL_READ) {
		apr_pollfd.reqevents	|= APR_POLLIN;
	}
	if (event_ptr->poll_flags & jr_POLL_WRITE) {
		apr_pollfd.reqevents	|= APR_POLLOUT;
	}
	if (event_ptr->poll_flags & jr_POLL_HUP) {
		apr_pollfd.reqevents	|= APR_POLLHUP;

		if (event_ptr->poll_flags & jr_POLL_IS_SOCKET) {
			apr_pollfd.reqevents	|= APR_POLLRDHUP;
		}
	}

	apr_pollfd.client_data		= event_ptr;

	status	= apr_pollset_add (apr_pollset_ptr, &apr_pollfd);

	if (status != 0) {
		jr_esprintf (error_buf, "%s", jr_apr_strerror( status, error_buf));
		status	= jr_ConvertAprErrno (status);
		goto return_status;
	}

	status	= 0;

	return_status : {
	}
	return status;
}

jr_int jr_poll_call_handler (
	jr_poll_t *					poll_ptr,
	jr_pollevent_t *			event_ptr,
	char *						error_buf)
{
	jr_int						status;


	if (event_ptr->opt_handler_fn) {
		status	= (*event_ptr->opt_handler_fn) (
					event_ptr->opt_handler_data_ptr,
					poll_ptr,
					event_ptr->generic_io_ptr,
					event_ptr->io_style_ready,
					error_buf
				);
	}
	else {
		status = 0;
	}

	return status;
}
