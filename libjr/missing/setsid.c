#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_setsid

#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>

int setsid()
{
	jr_int		tty_fd;
	pid_t		grp_id;

	tty_fd = open ("/dev/tty", O_RDONLY) ;

	if (tty_fd < 0) return(-1) ;

	if (ioctl(tty_fd, TIOCNOTTY, 0) < 0) {
		return(-1) ;
	}
	close(tty_fd) ;

	grp_id	= getpgrp (0);
	/*
	 * Get the process group of the current process.
	 * Under POSIX, this function does not take a parameter
	 */

	if (grp_id  !=  getpid()) {
		/*
		 * If the current process is not a process group leader,
		 * then make it the leader of its own process group
		 */
		grp_id = setpgrp (0, getpid());
	}
	return grp_id;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
/*
main(argc, argv)
	jr_int      argc ;
	char **  argv ;
{
	jr_int pid ;
	jr_int old_pgrp  ;
	jr_int new_pgrp  ;

	pid = getpid() ;
	old_pgrp = getpgrp() ;

	fprintf(stdout, "%s : %d has process group %d\n",
		argv[0], pid, old_pgrp
	) ;

	fprintf(stderr, "Enter a return: ") ;
	getchar() ;

	setsid() ;

	fprintf(stderr, "Enter a return: ") ;
	getchar() ;

	new_pgrp = getpgrp() ;
	fprintf(stdout, "%s : %d has process group %d\n",
		argv[0], pid, new_pgrp
	) ;

	exit(0) ;
}
*/
