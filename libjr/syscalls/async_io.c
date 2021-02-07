#include "async_io.h"

static jr_HTable			HandlerTable[1];
static jr_int				InitializedTable					= 0;

struct sigaction			OldAction[1];


jr_int jr_AddAsyncIOHandler (fd, fp, handler_fn, handler_param, is_for_reading, error_buf)
	jr_int					fd;
	FILE *					fp;
	void					(*handler_fn) ();
	void *					handler_param;
	jr_int					is_for_reading;
	char *					error_buf;
{
	jr_AsyncInfoStruct		tmp_handler_info[1];
	jr_AsyncInfoStruct *	handler_info;

	struct sigaction		new_action[1];
	sigset_t				new_mask;
	sigset_t				old_mask;
	jr_int					sig_num;


#	ifdef has_streams_async_io
		sig_num				= SIGPOLL;
#	else
		sig_num				= SIGIO;
#	endif

	sigemptyset (&new_mask);

	sigaddset (&new_mask, sig_num);
	sigprocmask (SIG_BLOCK, &new_mask, &old_mask);

	{
		if (!InitializedTable) {
			InitializedTable = 1;
			jr_HTableInit (
				HandlerTable, sizeof (jr_AsyncInfoStruct),
				jr_AsyncInfoHash, jr_AsyncInfoCmp
			);

			new_action->sa_handler = jr_AsyncIOHandler;
			sigemptyset (&new_action->sa_mask);
			new_action->sa_flags   = SA_RESTART;

			sigaction (sig_num, new_action, OldAction);
		}


		jr_AsyncInfoInit (tmp_handler_info, fd, fp, handler_fn, handler_param, is_for_reading);

		handler_info = jr_HTableFindElementPtr (HandlerTable, tmp_handler_info);

		if (handler_info) {
			jr_esprintf (error_buf, "file descriptor %d already has handler", fd);
			jr_AsyncInfoUndo (tmp_handler_info);
			sigprocmask (SIG_SETMASK, &old_mask, 0);
			return -1;
		}

		jr_HTableSetNewElement (HandlerTable, tmp_handler_info);
	}
	sigprocmask (SIG_SETMASK, &old_mask, 0);

	return 0;
}


void jr_RemoveAsyncIOHandler (fd, is_for_reading)
	jr_int					fd;
	jr_int					is_for_reading;
{
	jr_AsyncInfoStruct		tmp_handler_info[1];
	jr_AsyncInfoStruct *	handler_info;

	sigset_t				new_mask;
	sigset_t				old_mask;
	jr_int					sig_num;


	if (!InitializedTable) {
		return;
	}

#	ifdef has_streams_async_io
		sig_num				= SIGPOLL;
#	else
		sig_num				= SIGIO;
#	endif

	sigemptyset (&new_mask);

	sigaddset (&new_mask, sig_num);
	sigprocmask (SIG_BLOCK, &new_mask, &old_mask);
	{

		jr_AsyncInfoInit (tmp_handler_info, fd, 0, 0, 0, is_for_reading);

		handler_info = jr_HTableDeleteElement (HandlerTable, tmp_handler_info);

		jr_AsyncInfoUndo (tmp_handler_info);

		if (handler_info) {
			jr_AsyncInfoUndo (handler_info);
		}

		if (jr_HTableSize (HandlerTable)  ==  0) {
			sigaction (sig_num, OldAction, 0);

			jr_HTableUndo (HandlerTable);
			InitializedTable = 0;
		}
	}
	sigprocmask (SIG_SETMASK, &old_mask, 0);
}


void jr_AsyncIOHandler (sig_number)
	jr_int					sig_number;
{
	jr_AsyncInfoStruct *	handler_info;

	fd_set         			read_fds;
	fd_set          		write_fds;
	fd_set          		exception_fds;
	struct timeval  		timeout;

	int             		n_ready;
	jr_int					max_fd					= -1;



	if (!InitializedTable) {
		return;
	}

	FD_ZERO (&read_fds);
	FD_ZERO (&write_fds);  		/* or pass a 0 to select */
	FD_ZERO (&exception_fds);	/* or pass a 0 to select */

	jr_HTableForEachElementPtr (HandlerTable, handler_info) {
		if (handler_info->is_for_reading) {
			FD_SET  (handler_info->fd, &read_fds);

			if (handler_info->fd > max_fd) {
				max_fd		= handler_info->fd;
			}
		}
		else {
			FD_SET  (handler_info->fd, &write_fds);

			if (handler_info->fd > max_fd) {
				max_fd		= handler_info->fd;
			}
		}
	}

	timeout.tv_sec         = 0;
	timeout.tv_usec        = 0;

	while (1) {						/* infinite loop to restart if interrupted */
		n_ready = select (
			max_fd + 1, &read_fds, &write_fds, &exception_fds, &timeout
		);

		if (n_ready < 0) {
			if (errno == EINTR) {
				/*
				 * select is interrupted by signals
				 * and not restarted even under BSD
				 */
				continue;
			}
			return;
		}
		if (n_ready  ==  0) {
			return;
		}
		if (n_ready  >  0) {
			break;
		}
	}

	if (n_ready == 0) {
		return;
	}


	jr_HTableForEachElementPtr (HandlerTable, handler_info) {
		if (	FD_ISSET (handler_info->fd, &read_fds)
			&&	handler_info->is_for_reading) {

			if (handler_info->fp) {
				(*handler_info->handler_fn) (handler_info->fp, handler_info->handler_param);
			}
			else {
				(*handler_info->handler_fn) (handler_info->fd, handler_info->handler_param);
			}
		}
		if (	FD_ISSET (handler_info->fd, &write_fds)
			&&	!handler_info->is_for_reading) {

			if (handler_info->fp) {
				(*handler_info->handler_fn) (handler_info->fp, handler_info->handler_param);
			}
			else {
				(*handler_info->handler_fn) (handler_info->fd, handler_info->handler_param);
			}
		}
	}

	if (	OldAction->sa_handler  !=  SIG_DFL
		&&	OldAction->sa_handler  !=  SIG_IGN) {

		(*OldAction->sa_handler) (sig_number);
	}
}



void jr_AsyncInfoInit (handler_info, fd, fp, handler_fn, handler_param, is_for_reading)
	jr_AsyncInfoStruct *		handler_info;
	jr_int						fd;
	FILE *						fp;
	void						(*handler_fn) ();
	void *						handler_param;
	jr_int						is_for_reading;
{
	handler_info->fd				= fd;
	handler_info->fp				= fp;
	handler_info->handler_fn		= handler_fn;
	handler_info->handler_param		= handler_param;
	handler_info->is_for_reading	= is_for_reading;
}

void jr_AsyncInfoUndo (handler_info)
	jr_AsyncInfoStruct *		handler_info;
{
}

jr_int jr_AsyncInfoHash (void_arg_1)
	const void *				void_arg_1;
{
	const jr_AsyncInfoStruct *	handler_info		= void_arg_1;
	jr_int						hash_value			= 0;

	hash_value += handler_info->fd;
	hash_value += handler_info->is_for_reading;

	return hash_value;
}

jr_int jr_AsyncInfoCmp (void_arg_1, void_arg_2)
	const void *				void_arg_1;
	const void *				void_arg_2;
{
	const jr_AsyncInfoStruct *	handler_info_1		= void_arg_1;
	const jr_AsyncInfoStruct *	handler_info_2		= void_arg_2;
	jr_int						diff;

	diff = handler_info_1->fd - handler_info_2->fd;
	if (diff != 0) {
		return diff;
	}

	diff = handler_info_1->is_for_reading - handler_info_2->is_for_reading;
	if (diff != 0) {
		return diff;
	}

	return 0;
}


void jr_BlockAsyncIO (old_mask_ptr)
	void *					old_mask_ptr;
{
	sigset_t				new_mask;
	jr_int					sig_num;


#	ifdef has_streams_async_io
		sig_num				= SIGPOLL;
#	else
		sig_num				= SIGIO;
#	endif

	sigemptyset (&new_mask);

	sigaddset (&new_mask, sig_num);
	sigprocmask (SIG_BLOCK, &new_mask, old_mask_ptr);
}

void jr_UnBlockAsyncIO (old_mask_ptr)
	void *					old_mask_ptr;
{
	sigprocmask (SIG_SETMASK, old_mask_ptr, 0);
}


#	ifdef has_streams_async_io

#include <sys/types.h>
#include <stropts.h>
#include <sys/conf.h>

jr_int jr_InitAsyncIO (fd, fp, handler_fn, handler_param, is_for_reading, error_buf)
	jr_int		fd;
	FILE *		fp;
	void		(*handler_fn) ();
	void *		handler_param;
	jr_int		is_for_reading;
	char *		error_buf;
{
	jr_int		status;
	jr_int		ioctl_args				= 0;


	status		= jr_AddAsyncIOHandler (fd, fp, handler_fn, handler_param, is_for_reading, error_buf);

	if (status != 0) {
		return -1;
	}

	/*
	 * Set the handler before setting the enabling
	 * the generation of SIGPOLL.
	 */
	if (is_for_reading) {
		ioctl_arg	|= S_RDNORM;
	}
	else {
		ioctl_arg	|= S_WRNORM;
	}


	status = ioctl (fd, I_SETSIG, ioctl_args);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't ioctl (%d, I_SETSIG): %s",
			fd, strerror (errno)
		);

		jr_FileDescUnSetAsyncHandler (fd);

		return -1;
	}

	return 0;
}

jr_int jr_UndoAsyncIO (fd, is_for_reading, error_buf)
	jr_int				fd;
	jr_int				is_for_reading;
	char *				error_buf;
{

	jr_RemoteAsyncHandler (fd, is_for_reading);

	status = ioctl (fd, I_SETSIG, 0);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't ioctl (%d, I_SETSIG): %s",
			fd, strerror (errno)
		);

		return -1;
	}
	return 0;
}

#else

jr_int jr_InitAsyncIO (fd, fp, handler_fn, handler_param, is_for_reading, error_buf)
	jr_int				fd;
	FILE *				fp;
	void				(*handler_fn) ();
	void *				handler_param;
	jr_int				is_for_reading;
	char *				error_buf;
{
	jr_int				open_flags;
	jr_int				status;


	open_flags = fcntl (fd, F_GETFL, 0);

	if (open_flags == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_GETFL): %s",
			fd, strerror (errno)
		);
		return -1;
	}


	open_flags |= FASYNC;

	status = fcntl (fd, F_SETFL, open_flags);

	if (status == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_SETFL): %s",
			fd, strerror (errno)
		);
		return -1;
	}

	status		= jr_AddAsyncIOHandler (fd, fp, handler_fn, handler_param, is_for_reading, error_buf);

	if (status != 0) {
		return -1;
	}

	/*
	 * Set the handler before setting the enabling
	 * the generation of SIGIO.
	 */

	status = fcntl (fd, F_SETOWN, getpid ());
	/*
	 * tell device driver which process should receive SIGIO
	 */

	if (status == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_SETOWN): %s",
			fd, strerror (errno)
		);

		jr_RemoveAsyncIOHandler (fd, is_for_reading);
		return -1;
	}
	return 0;
}

jr_int jr_UndoAsyncIO (fd, is_for_reading, error_buf)
	jr_int				fd;
	jr_int				is_for_reading;
	char *				error_buf;
{
	jr_int				open_flags;
	jr_int				status;


	jr_RemoveAsyncIOHandler (fd, is_for_reading);

	open_flags = fcntl (fd, F_GETFL, 0);

	if (open_flags == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_GETFL): %s",
			fd, strerror (errno)
		);
		return -1;
	}
	open_flags |= FASYNC;

	status = fcntl (fd, F_SETFL, open_flags);

	if (status == -1) {
		jr_esprintf (error_buf, "couldn't fcntl (%d, F_SETFL): %s",
			fd, strerror (errno)
		);
		return -1;
	}
	return 0;
}

#	endif
