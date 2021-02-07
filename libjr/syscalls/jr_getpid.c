#include "ezport.h"


#ifdef ostype_winnt

#include <process.h>

#include "jr/syscalls.h"

jr_int	jr_getpid ()
{
	return _getpid ();
}


#else


#include <unistd.h>
#include "jr/syscalls.h"

jr_int	jr_getpid ()
{
	return getpid ();
}



#endif
