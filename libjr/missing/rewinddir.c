#define _POSIX_SOURCE 1

#include "ezport.h"

#ifdef missing_rewinddir_objects

#include <dirent.h>

void rewinddir(dirp)
	DIR *			dirp;
{
	seekdir (dirp, 0L);
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
