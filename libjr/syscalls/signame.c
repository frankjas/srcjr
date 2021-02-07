#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <signal.h>

#include "jr/syscalls.h"

extern	void jr_sys_siglist_init ();

char *jr_SignalName (sig_num)
	unsigned jr_int		sig_num;
{
	const char *	sig_list_entry;
	static char		return_buf [32];

#	ifdef missing_sys_siglist_objects
	{
		if (sys_siglist[SIGINT] == 0) {
			jr_sys_siglist_init ();
		}
	}
#	endif

	if (sig_num > 0  &&  sig_num < NSIG) {
		sig_list_entry = sys_siglist[sig_num];

		if (sig_list_entry) {
			sprintf (return_buf, "%.31s", sig_list_entry);
		}
		else {
			sprintf (return_buf, "Signal %d", sig_num);
		}
	}
	else {
#		ifdef ostype_winnt
			/*
			** 2-5-2019: jr_process_wait() calls apr_proc_wait() which calls
			** GetExitCodeProcess() which could return 0xc000XXXX as the exception code
			*/
			sprintf (return_buf, "%#x", sig_num);
#		else
			sprintf (return_buf, "Invalid signal %d", sig_num);
#		endif
	}
	return return_buf;
}
