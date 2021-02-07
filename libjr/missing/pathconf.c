#define _POSIX_SOURCE 1


#include "ezport.h"


#ifdef missing_pathconf_objects

#include <errno.h>
#include <unistd.h>
#include <sys/param.h>

#include <sys/dir.h>


long pathconf(file_name, desired_value)
	const char *	file_name ;
	int     		desired_value ;
{
	switch(desired_value) {

		case _PC_LINK_MAX         :
			/*
			 * maximum number of links to a file or directory.
			 * We should open this name to see if its a directory
			 * and report zero in that event on most systems.
			 */
			return(MAXLINK) ;		/* sys/param.h */

		case _PC_MAX_CANON        :
			/*
			 * maximum number of bytes on a line
			 * using canonical input processing
			 */
			return(CANBSIZ) ;		/* sys/param.h */

		case _PC_MAX_INPUT        :
			/*
			 * maximum "packet" size that can be sent to a tty device.
			 * I couldn't find a constant like this so I assumed
			 * it would be the same as CANBSIZ
			 */
			return(CANBSIZ) ;		/* sys/param.h */

		case _PC_NAME_MAX         :
			return(MAXNAMLEN) ;		/* sys/dir.h */

		case _PC_PATH_MAX         :
			return(MAXPATHLEN) ;	/* sys/param.h */

		case _PC_PIPE_BUF         :
				/*
				 * 4096 is in the BUGS section of the man
				 * page for  pipe()  but not a constant anywhere
				 * that I can find.
				 */
				return(4096) ;

		case _PC_CHOWN_RESTRICTED :
			 /*
			  * Use of chown is always restricted to root on our system
			  */
			 return(1) ;	/* Boolean determined experimentally */

		case _PC_NO_TRUNC         :
			/*
			 * always 1 on reasonable systems, where if you try to create
			 * a file with a name that is too long, open or creat fail
			 * and errno will be set to ENAMETOOLONG
			 */
			return(1) ;	/* Boolean from the man for open() */

		case _PC_VDISABLE         :
			/*
			 * Is there a character that can disable special tty functions?
			 * None that I can be sure of, so just return false.
			 */
			return(0) ;	/* Boolean, same as found on Sun and safest */

		default                   :
			errno = EINVAL ;
			return(-1) ;
	}
}

long fpathconf(fd, desired_value)
	int     fd ;
	int     desired_value ;
{
	return  pathconf("dummy", desired_value) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
