#define _POSIX_SOURCE 1
#include "ezport.h"

#include <string.h>
#include <signal.h>

#include "jr/syscalls.h"

# ifndef ostype_winnt

void jr_critical_section (how, opt_void_old_mask_ptr)
	char *				how;
	void *				opt_void_old_mask_ptr;
{
	sigset_t *			old_mask_ptr		= opt_void_old_mask_ptr;
	sigset_t			new_mask;

	sigemptyset (&new_mask);

	sigaddset (&new_mask, SIGHUP);		/* terminal gets hung up */
	sigaddset (&new_mask, SIGINT);		/* cntrl-C */
	sigaddset (&new_mask, SIGQUIT);		/* cntrl-backslash */
	sigaddset (&new_mask, SIGALRM);		/* software alarm */
	sigaddset (&new_mask, SIGTERM);		/* software kill */
	sigaddset (&new_mask, SIGTSTP);		/* cntrl-Z */

	if (strcmp (how, "begin") == 0) {
		sigprocmask (SIG_BLOCK, &new_mask, old_mask_ptr);
	}
	if (strcmp (how, "end") == 0) {
		sigprocmask (SIG_SETMASK, old_mask_ptr, 0);
	}
}

#endif
