#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_dup2

#include <stdio.h>
#include <errno.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>

int dup2(oldd, target_descriptor)
	int oldd ;
	int target_descriptor ;
{
	jr_int descriptor_set[FOPEN_MAX] ;
	jr_int newd ;
	jr_int set_index ;

	if ((target_descriptor < 0) ||
		(target_descriptor >= FOPEN_MAX)) {
		errno = EINVAL ;
		return(-1) ;
	}

	if (close(target_descriptor) == -1) {
		errno = 0 ;
	}

	set_index = 0 ;
	while ((newd = dup(oldd)) != target_descriptor) {
		if (newd == -1) {
			break ;
		}

		/* Remember The Descriptor */ {
			descriptor_set[set_index] = newd ;
			set_index++ ;
		}
	}

	/* ForEachRememberedDescriptor */ {
		jr_int i ;

		for (i = 0; i < set_index ; i++) {
			close(descriptor_set[i]) ;
		}
	}

	return(newd) ;
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif

/*
void main()
{
	char buf[128] ;
	jr_int new_stderr = dup2(2, 27) ;
	jr_int next_descriptor = dup(1) ;

	sprintf(buf, "new_stderr == %d, next available descriptor == %d\n",
		new_stderr,
		next_descriptor
	) ;

	write(new_stderr, buf, strlen(buf)) ;

	exit(0) ;
}
*/
