#define _POSIX_SOURCE 1

#include "ezport.h"

#include <unistd.h>

#include "jr/io.h"

jr_int jr_FileDescCopy(wfd, rfd, buf, bufsize)
        jr_int	wfd ;
        jr_int	rfd ;
        void *	buf ;
        jr_int	bufsize ;
{
	ssize_t  nbytes ;

	while (nbytes = read(rfd, buf, bufsize)) {

		if (nbytes == -1) {
				return(-1) ;
		}

		if (write(wfd, buf, nbytes) != nbytes) {
				return(-1) ;
		}
	}
	return(0) ;
}
