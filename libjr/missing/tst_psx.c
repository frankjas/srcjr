#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <dirent.h>

void on_intr()
{
	fprintf(stdout,"got interrupt\n") ;
}

void main()
{
	char		dirname_buf[127] ;
	char *		dirname_ptr ;
	jr_int		target_descriptor	= dup2(0,4) ;
	pid_t		pgrp				= getpgrp() ;
	jr_int		maxpathlen			= pathconf(".", _PC_PATH_MAX) ;
	jr_int		sid					= setsid() ;
	mode_t		my_mode				= 0 ;
	ssize_t		my_ssize			= 0 ;

	dirname_ptr = getcwd(dirname_buf, sizeof(dirname_buf)) ;
	fprintf(stdout, "getcwd(%#010x) == %s\n", dirname_ptr, dirname_ptr) ;
	if (! dirname_ptr) {
		perror(dirname_buf) ;
	}

	fprintf(stdout, "dup2(0,4) == %d\ngetprgp() == %d\npathconf(_PC_PATH_MAX) == %d\nsetsid() == %d\ndummy mode == %d\ndummy size == %ld\n",
		target_descriptor,
		pgrp,
		maxpathlen,
		sid,
		my_mode,
		my_ssize
	) ;
		
	setpgid(getpid(), pgrp) ;

	{
		struct sigaction new_action[1];
		struct sigaction old_action[1];

		new_action->sa_handler = on_intr;
		sigemptyset (&new_action->sa_mask);
		new_action->sa_flags   = 0;

		sigaction (SIGINT, new_action, old_action) ;
	}

	{
		sigset_t set[1] ;


		sigfillset(set) ;
		sigdelset(set, SIGINT) ;

		sigemptyset(set) ;
		sigaddset(set, SIGINT) ;

		sigismember(set, SIGINT) ;
	}

	{
		sigset_t set[1] ;

		sigemptyset(set) ;

		fprintf(stdout, "Enter a ^C\n") ;
		sigsuspend(set) ;
	}

	{
		sigset_t new_mask ;
		sigset_t old_mask ;

		sigprocmask(SIG_SETMASK, &new_mask, &old_mask) ;
		sigprocmask(SIG_SETMASK, &old_mask, 0) ;
	}

	{
		char *  new_string = strdup("oldstring") ;

		fprintf(stdout, "new_string == %s\n", new_string) ;
	}

	{
		jr_int maxargs = sysconf(_SC_ARG_MAX) ;

		fprintf(stdout, "maxargs == %d\n", maxargs) ;
	}

	{
		setpgid(0, 0) ;
	}


	{
		pid_t pgrp ;

		pgrp = tcgetpgrp(0) ;
		tcsetpgrp(0, pgrp) ;
	}
#ifndef S_IRWXU
	printf("missing posix permission constants like S_IRWXU\n") ;
#else
	printf("has posix permission constants like S_IRWXU\n") ;
#endif

#ifndef WSTOPPED
	printf("missing WSTOPPED\n") ;
#endif

#ifndef WEXITSTATUS
	printf("missing WEXITSTATUS\n") ;
#else
	/*
	 * check that it uses int rather than union wait
	 */
	{
		int status = 0;

		WEXITSTATUS (status);
	}
#endif

#ifndef WSTOPSIG
	printf("missing WSTOPSIG\n") ;
#endif

#ifndef WTERMSIG
	printf("missing WTERMSIG\n") ;
#endif

#ifndef WIFCORE
	printf("missing WIFCORE\n") ;
#endif

#ifndef WIFSTOPPED
	printf("missing WIFSTOPPED\n") ;
#endif

#ifndef WIFEXITED
	printf("missing WIFEXITED\n") ;
#endif

#ifndef WIFSIGNALED
	printf("missing WIFSIGNALED\n") ;
#endif

	{
		int		status ;
		pid_t	pid ;
		
		pid = waitpid(-1, &status, WNOHANG) ;

		fprintf(stdout,"waitpid(-1, &status, WNOHANG) == %d\n", pid) ;
	}

	/* directory manipulations */ {
		DIR *			my_dir_stream ;
		struct dirent *	my_dir_entry ;
		
		my_dir_stream = opendir(".") ;
		if (my_dir_stream == NULL) {
			perror("couldn't opendir(\".\")") ;
		}

		for (my_dir_entry = (struct dirent *) readdir(my_dir_stream) ;
			my_dir_entry != NULL ;
			my_dir_entry = (struct dirent *) readdir(my_dir_stream)) {

			fprintf(stdout, "%s\n", my_dir_entry->d_name) ;
		}
	}

	/* environment functions */ {
		char *	home ;
		char *	found ;
		char 	new_env_buf[128] ;

		home = getenv("HOME") ;
		if (home) {
			fprintf(stdout, "HOME = %s\n", home) ;
		}
		else {
			fprintf(stdout, "HOME not in environment\n", home) ;
		}

		found = "TESTPUTENV" ;
		sprintf(new_env_buf, "%s=%s", found, "hello") ;
		putenv(strdup(new_env_buf)) ;

		fprintf(stdout, "%s = %s\n", found, getenv(found)) ;
	}
}
