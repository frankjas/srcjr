#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_sigaction_objects

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int sigprocmask (how, set, oldset)
	int         		how ;
	const sigset_t *	set ;
	sigset_t *  		oldset ;
{
	int prev_mask ;
	int  new_mask ;

	prev_mask = sigblock(0) ;

	if (set != NULL) {
		switch(how) {
			case SIG_BLOCK   : {
				new_mask = *set;
				sigblock (new_mask);
				break;
			}
			case SIG_UNBLOCK : {
				new_mask = prev_mask &  ~(*set);
				sigsetmask (new_mask);
				break;
			}
			case SIG_SETMASK : {
				new_mask = *set;
				sigsetmask (new_mask);
				break;
			}
			default : {
				errno = EINVAL;
				return (-1);
			}
		}
	}
	if (oldset != NULL) {
		*oldset = prev_mask;
	}
	return(0) ;
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
	sigset_t new_mask ;
	sigset_t old_mask ;
	jr_int      c ;

	sigemptyset(&new_mask) ;
	sigaddset(&new_mask, SIGINT) ;

	sigprocmask(SIG_SETMASK, &new_mask, 0) ;

	sigemptyset(&new_mask) ;
	sigaddset(&new_mask, SIGQUIT) ;

	sigprocmask(SIG_BLOCK, &new_mask, &old_mask) ;

	fprintf(stderr,"old_mask == %#010x\n", old_mask) ;

	fprintf(stderr,"Signals which are blocked (masked), may not be delivered.\n") ;
	fprintf(stderr,"If they are generated while they are blocked, then they\n") ;
	fprintf(stderr,"stay 'pending' until they are unblocked.\n") ;
	fprintf(stderr,"A feature of the terminal driver is to empty input\n") ;
	fprintf(stderr,"buffers when keyboard interrupts are generated.\n") ;


	fprintf(stderr,"Enter text or interrupt (interrupts are blocked): ") ;
	while ((c = getchar()) != EOF) {
		putchar(c) ;
		if (c == '\n') {
			sigset_t empty ;
			sigemptyset(&empty) ;
			sigprocmask(SIG_SETMASK, &empty, 0) ;
			fprintf(stderr,"Enter text or interrupt (interrupts are deliverable): ") ;
		}
	}

	exit(0) ;
}

*/
