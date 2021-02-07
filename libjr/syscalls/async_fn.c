#include "async_fn.h"

static jr_int					InitializedTree		= 0;
static jr_AVL_Tree				HandlerTree[1];

static jr_int					CurrentTime			= 0;
static jr_int					AlarmTime			= 0;
static jr_int					NumAlarms			= 0;

static struct sigaction			OldAction[1];


jr_int jr_AddAsyncFunction (
	void						(*handler_fn) (),
	void *						handler_param,
	jr_int						num_seconds,
	jr_int						do_repeat,
	char *						error_buf)
{
	struct sigaction			new_action[1];
	sigset_t					new_mask;
	sigset_t					old_mask;
	jr_int						sig_num;
	jr_int						old_alarm;
	jr_int						status;


	sig_num				= SIGALRM;

	sigemptyset (&new_mask);

	sigaddset (&new_mask, sig_num);
	sigprocmask (SIG_BLOCK, &new_mask, &old_mask);
	{
		jr_AsyncFunctionStruct		tmp_handler_info[1];
		jr_AsyncFunctionStruct *	handler_info_ptr;
		void						(*curr_handler_fn) ()		= 0;


		if (num_seconds <= 0) {
			jr_esprintf (error_buf, "num_seconds must be greater than zero, not %d", num_seconds);
			status = -1;
			goto return_status;
		}

		CurrentTime	= time (0);
		/*
		 * 8/4/2005: get the time now so that we're sure the new call times will be after this.
		 */


		if (!InitializedTree) {
			InitializedTree = 1;

			jr_AVL_TreeInit (
				HandlerTree, sizeof (jr_AsyncFunctionStruct), jr_AsyncFunctionCmp
			);

			new_action->sa_handler = jr_AsyncFunctionHandler;
			sigemptyset (&new_action->sa_mask);
			new_action->sa_flags   = SA_RESTART;

			sigaction (sig_num, new_action, OldAction);

			if (	OldAction->sa_handler != SIG_IGN
				&&	OldAction->sa_handler != SIG_DFL) {
				
				curr_handler_fn		= OldAction->sa_handler;
			}

			old_alarm	= alarm (0);

			if (old_alarm > 0  &&  curr_handler_fn) {
				status	= jr_AddAsyncFunction (curr_handler_fn, 0, old_alarm, 0, error_buf);

				iferr (status != 0) {
					alarm (old_alarm);
					jr_esprintf (error_buf, "couldn't add existing handler %p: %s",
						curr_handler_fn, error_buf
					);
					status = -1;
					goto return_status;
				}
			}
		}

		/*
		** 8/9/2005: remove any handlers that went off but didn't repeat.
		** Couldn't deallocate them inside the signal handler
		*/

		handler_info_ptr	= jr_AVL_TreeGreatestElementPtr (HandlerTree);

		while (handler_info_ptr) {
			if (jr_AsyncFunctionCallTime (handler_info_ptr) == jr_INT_MAX) {
				jr_AVL_TreeExtractElement (HandlerTree, handler_info_ptr);
				jr_AsyncFunctionUndo (handler_info_ptr);
				jr_AVL_TreeFreeElement (HandlerTree, handler_info_ptr);
			}
			else {
				break;
			}
			handler_info_ptr	= jr_AVL_TreeGreatestElementPtr (HandlerTree);
		}


		jr_AVL_TreeForEachElementPtr (HandlerTree, handler_info_ptr) {
			iferr_sim (	jr_AsyncFunctionPtr (handler_info_ptr) == handler_fn
					&&	jr_AsyncFunctionParam (handler_info_ptr) == handler_param) {

				jr_esprintf (error_buf, "handler fn %p with parameter %p already loaded", 
					handler_fn, handler_param
				);
				status = -1;
				goto return_status;
			}
		}

		jr_AsyncFunctionInit (tmp_handler_info, handler_fn, handler_param, num_seconds, do_repeat);

		handler_info_ptr = jr_AVL_TreeFindElementPtr (HandlerTree, tmp_handler_info);

		iferr_sim (handler_info_ptr) {
			jr_AsyncFunctionUndo (tmp_handler_info);

			jr_esprintf (error_buf, "can't insert into handler tree after duplicate check??");
			status = -1;
			goto return_status;
		}

		jr_AVL_TreeSetNewElement (HandlerTree, tmp_handler_info);

		handler_info_ptr = jr_AVL_TreeSmallestElementPtr (HandlerTree);

		if (jr_AsyncFunctionCallTime (handler_info_ptr)  -  CurrentTime  >  0) {
			alarm (jr_AsyncFunctionCallTime (handler_info_ptr) - CurrentTime);
		}
		else {
			alarm (1);
		}
	}

	status = 0;

	return_status : {
		sigprocmask (SIG_SETMASK, &old_mask, 0);
	}

	return status;
}

void jr_RemoveAsyncFunction (
	void						(*handler_fn) (),
	void *						handler_param)
{
	sigset_t					new_mask;
	sigset_t					old_mask;
	jr_int						sig_num;


	sig_num				= SIGALRM;

	sigemptyset (&new_mask);

	sigaddset (&new_mask, sig_num);
	sigprocmask (SIG_BLOCK, &new_mask, &old_mask);
	{
		jr_AsyncFunctionStruct *	handler_info_ptr;

		jr_AVL_TreeForEachElementPtr (HandlerTree, handler_info_ptr) {
			if (	jr_AsyncFunctionPtr (handler_info_ptr) == handler_fn
				&&	jr_AsyncFunctionParam (handler_info_ptr) == handler_param) {

				jr_AVL_TreeExtractElement (HandlerTree, handler_info_ptr);
				jr_AsyncFunctionUndo (handler_info_ptr);
				jr_AVL_TreeFreeElement (HandlerTree, handler_info_ptr);

				break;
			}
		}

		if (jr_AVL_TreeIsEmpty (HandlerTree)) {
			sigaction (sig_num, OldAction, 0);

			jr_AVL_TreeUndo (HandlerTree);
			InitializedTree = 0;
		}
	}

	sigprocmask (SIG_SETMASK, &old_mask, 0);
}


jr_int jr_GetAsyncFunctionNumSeconds (
	void						(*handler_fn) (),
	void *						handler_param)
{
	jr_AsyncFunctionStruct *	handler_info_ptr;

	jr_AVL_TreeForEachElementPtr (HandlerTree, handler_info_ptr) {
		if (	jr_AsyncFunctionPtr (handler_info_ptr) == handler_fn
			&&	jr_AsyncFunctionParam (handler_info_ptr) == handler_param) {

			return jr_AsyncFunctionNumSeconds (handler_info_ptr);
		}
	}
	return 0;
}


jr_int jr_GetAsyncFunctionCallTime (
	void						(*handler_fn) (),
	void *						handler_param)
{
	jr_AsyncFunctionStruct *	handler_info_ptr;

	jr_AVL_TreeForEachElementPtr (HandlerTree, handler_info_ptr) {
		if (	jr_AsyncFunctionPtr (handler_info_ptr) == handler_fn
			&&	jr_AsyncFunctionParam (handler_info_ptr) == handler_param) {

			return jr_AsyncFunctionCallTime (handler_info_ptr);
		}
	}
	return 0;
}


void jr_AsyncFunctionHandler (
	jr_int						signum)
{
	jr_AsyncFunctionStruct *	handler_info_ptr;
	jr_AsyncFunctionStruct *	next_handler_info_ptr;

	if (NumAlarms % 20 == 0) {
		CurrentTime		= time (0);
	}
	else {
		CurrentTime		+= AlarmTime;
	}

	if (!InitializedTree) {
		return;
	}

	handler_info_ptr	= jr_AVL_TreeSmallestElementPtr (HandlerTree);

	/*
	** 8/9/2005: can't do any malloc()/free() operations
	*/

	while (handler_info_ptr) {

		if (jr_AsyncFunctionCallTime (handler_info_ptr)  >  CurrentTime) {

			alarm (jr_AsyncFunctionCallTime (handler_info_ptr) - CurrentTime);

			break;
		}

		next_handler_info_ptr	= jr_AVL_TreeNextElementPtr (HandlerTree, handler_info_ptr);

		if (jr_AsyncFunctionCallTime (handler_info_ptr)  <=  CurrentTime) {

			(*handler_info_ptr->handler_fn) (handler_info_ptr->handler_param, CurrentTime);

			jr_AVL_TreeExtractElement (HandlerTree, handler_info_ptr);

			if (jr_AsyncFunctionDoRepeat (handler_info_ptr)) {
				jr_AsyncFunctionSetCallTime (
					handler_info_ptr, CurrentTime + jr_AsyncFunctionNumSeconds (handler_info_ptr)
				);
			}
			else {
				jr_AsyncFunctionSetCallTime (handler_info_ptr, jr_INT_MAX);
			}
			jr_AVL_TreeInsertElement (HandlerTree, handler_info_ptr);

			if (	next_handler_info_ptr == 0
				&&	jr_AsyncFunctionCallTime (handler_info_ptr)  >  CurrentTime) {

				next_handler_info_ptr	= handler_info_ptr;
			}
		}
		handler_info_ptr	= next_handler_info_ptr;
	}
}


void jr_AsyncFunctionInit (
	jr_AsyncFunctionStruct *	handler_info_ptr,
	void						(*handler_fn) (),
	void *						handler_param,
	jr_int						num_seconds,
	jr_int						do_repeat)
{
	memset (handler_info_ptr, 0, sizeof (*handler_info_ptr));

	handler_info_ptr->call_time			= time (0) + num_seconds;
	handler_info_ptr->handler_fn		= handler_fn;
	handler_info_ptr->handler_param		= handler_param;
	handler_info_ptr->num_seconds		= num_seconds;
	handler_info_ptr->do_repeat			= do_repeat != 0;
}

void jr_AsyncFunctionUndo (
	jr_AsyncFunctionStruct *	handler_info_ptr)
{
}


jr_int jr_AsyncFunctionCmp (
	const void *				void_arg_1,
	const void *				void_arg_2)
{
	const jr_AsyncFunctionStruct *	handler_info_ptr_1		= void_arg_1;
	const jr_AsyncFunctionStruct *	handler_info_ptr_2		= void_arg_2;

	if (handler_info_ptr_1->call_time  <  handler_info_ptr_2->call_time) {
		return -1;
	}
	if (handler_info_ptr_1->call_time  >  handler_info_ptr_2->call_time) {
		return 1;
	}

	if (handler_info_ptr_1->handler_fn  <  handler_info_ptr_2->handler_fn) {
		return -1;
	}
	if (handler_info_ptr_1->handler_fn  >  handler_info_ptr_2->handler_fn) {
		return 1;
	}

	if (handler_info_ptr_1->handler_param  <  handler_info_ptr_2->handler_param) {
		return -1;
	}
	if (handler_info_ptr_1->handler_param  >  handler_info_ptr_2->handler_param) {
		return 1;
	}


	return 0;
}
