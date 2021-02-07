#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_sys_siglist_objects

#include <signal.h>

#ifdef ostype_winnt
/*
** 1/27/07: don't make these globally available, since
** other code may have ifdefs based on the existence
** of these signals (coredump(), for example)
*/
#define		SIGHUP				(64 + 0)
#define		SIGQUIT				(64 + 1)
#define		SIGTRAP				(64 + 2)
#define		SIGIOT				(64 + 3)
#define		SIGBUS				(64 + 4)
#define		SIGKILL				(64 + 5)
#define		SIGUSR1				(64 + 6)
#define		SIGUSR2				(64 + 7)
#define		SIGPIPE				(64 + 8)
#define		SIGALRM				(64 + 9)
#define		SIGCHLD				(64 + 10)
#define		SIGCONT				(64 + 11)
#define		SIGSTOP				(64 + 12)
#define		SIGTSTP				(64 + 13)
#define		SIGTTIN				(64 + 14)
#define		SIGTTOU				(64 + 15)
#define		SIGURG				(64 + 16)
#define		SIGXCPU				(64 + 17)
#define		SIGXFSZ				(64 + 18)
#define		SIGVTALRM			(64 + 19)
#define		SIGPROF				(64 + 20)
#define		SIGWINCH			(64 + 21)
#define		SIGIO				(64 + 22)
#define		SIGPWR				(64 + 23)
#define		SIGSYS				(64 + 24)
#endif

char *	sys_siglist[128];		/* 1/27/07: Need 128 for fake Microsoft signals */

void jr_sys_siglist_init ()
{
	sys_siglist [SIGHUP]		= "Hangup";							/*(POSIX).  */
	sys_siglist [SIGINT]		= "Interrupt";						/*(ANSI).  */
	sys_siglist [SIGQUIT]		= "Quit";							/*(POSIX).  */
	sys_siglist [SIGILL]		= "Illegal instruction";			/*(ANSI).  */
	sys_siglist [SIGTRAP]		= "Trace trap";						/*(POSIX).  */
	sys_siglist [SIGABRT]		= "Abort";							/*(ANSI).  */
	sys_siglist [SIGIOT]		= "IOT trap";						/*(4.2 BSD).  */
	sys_siglist [SIGBUS]		= "BUS error";						/*(4.2 BSD).  */
	sys_siglist [SIGFPE]		= "Floating-point exception";		/*(ANSI).  */
	sys_siglist [SIGKILL]		= "Kill";							/*(POSIX).  */
	sys_siglist [SIGSEGV]		= "Segmentation violation";			/*(ANSI).  */
	sys_siglist [SIGUSR1]		= "User-defined signal 1";			/*(POSIX).  */
	sys_siglist [SIGUSR2]		= "User-defined signal 2";			/*(POSIX).  */
	sys_siglist [SIGPIPE]		= "Broken pipe";					/*(POSIX).  */
	sys_siglist [SIGALRM]		= "Alarm clock";					/*(POSIX).  */
	sys_siglist [SIGTERM]		= "Termination";					/*(ANSI).  */
	sys_siglist [SIGCHLD]		= "Child status has changed";		/*(POSIX).  */
	sys_siglist [SIGCONT]		= "Continue";						/*(POSIX).  */
	sys_siglist [SIGSTOP]		= "Stop";							/*(POSIX).  */
	sys_siglist [SIGTSTP]		= "Keyboard stop";					/*(POSIX).  */
	sys_siglist [SIGTTIN]		= "Background read from tty";		/*(POSIX).  */
	sys_siglist [SIGTTOU]		= "Background write to tty";		/*(POSIX).  */
	sys_siglist [SIGURG]		= "Urgent condition on socket";		/*(4.2 BSD).  */
	sys_siglist [SIGXCPU]		= "CPU limit exceeded";				/*(4.2 BSD).  */
	sys_siglist [SIGXFSZ]		= "File size limit exceeded";		/*(4.2 BSD).  */
	sys_siglist [SIGVTALRM]		= "Virtual alarm clock";			/*(4.2 BSD).  */
	sys_siglist [SIGPROF]		= "Profiling alarm clock";			/*(4.2 BSD).  */
	sys_siglist [SIGWINCH]		= "Window size change";				/*(4.3 BSD, Sun).  */
	sys_siglist [SIGIO]			= "I/O now possible";				/*(4.2 BSD).  */
	sys_siglist [SIGPWR]		= "Power failure restart";			/*(System V).  */
	sys_siglist [SIGSYS]		= "Bad system call";
}

#else

#ifdef ostype_macosx
#pragma GCC diagnostic ignored "-Winfinite-recursion"
#endif

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
