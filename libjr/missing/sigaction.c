#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_sigaction_objects

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int sigaction(signal_number, new_action, old_action)
	int                 		signal_number ;
	const struct sigaction *	new_action ;
	struct sigaction *			old_action ;
{
	struct sigvec  new_vec[1] ;
	struct sigvec  old_vec[1] ;
	jr_int            status ;

	if (new_action != NULL) {
		new_vec->sv_handler = new_action->sa_handler ;
		new_vec->sv_mask    = new_action->sa_mask    ;
		new_vec->sv_flags   = new_action->sa_flags   ;

		status = sigvec(signal_number, new_vec, old_vec) ;
	}
	else {
		status = sigvec(signal_number, 0, old_vec) ;
	}

	if (old_action != NULL) {
		old_action->sa_handler = old_vec->sv_handler ;
		old_action->sa_mask    = old_vec->sv_mask    ;
		old_action->sa_flags   = old_vec->sv_flags   ;
	}

	return(status) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif

/*
void onintr(signum)
	jr_int signum ;
{
	fprintf(stderr, "Interrupted with signal number %d\n", signum) ;
	fprintf(stderr, "Enter a return or try to kill this handler with SIGQUIT: ") ;
	getchar() ;
	fprintf(stderr, "Returning from handler now.\n") ;
}

main(argc, argv)
	jr_int      argc ;
	char **  argv ;
{
	struct sigaction  new_action[1] ;
	struct sigaction  old_action[1] ;
	jr_int c ;

	new_action->sa_handler = onintr ;
	sigemptyset(&(new_action->sa_mask)) ;
	sigaddset(&(new_action->sa_mask), SIGQUIT) ;
	new_action->sa_flags   = 0 ;

	sigaction(SIGINT, new_action, 0) ;
	sigaction(SIGINT, new_action, old_action) ;

	fprintf(stdout,"old_action->sa_handler == %#010x\n", old_action->sa_handler) ;
	fprintf(stdout,"old_action->sa_mask    == %#010x\n", old_action->sa_mask) ;
	fprintf(stdout,"old_action->sa_flags   == %#010x\n", old_action->sa_flags) ;

	fprintf(stderr,"Signals which are blocked (masked), may not be delivered.\n") ;
	fprintf(stderr,"If they are generated while they are blocked, then they\n") ;
	fprintf(stderr,"stay 'pending' until they are unblocked.\n") ;

	fprintf(stderr,"Enter text or interrupt: ") ;
	while ((c = getchar()) != EOF) {
		putchar(c) ;
		if (c == '\n') {
			fprintf(stderr,"Enter text or interrupt: ") ;
		}
	}

	exit(0) ;
}
*/
