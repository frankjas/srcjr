#include "ezport.h"

#include <stdlib.h>

/*
** 4-15-2011: Fix the C++ problem wrt linking debug builds
** against no-debug libraries.  The no-debug libraries call
** _invalid_parameter_noinfo(), but that isn't provided
** when linking agains the debug standard libraries.
**
** The following define may help
** #define _SCL_SECURE 0
**
** Can set a custom handler with: _set_invalid_parameter_handler()?
*/

#ifdef missing_invalid_parameter_noinfo

jr_EXTERN(void) __cdecl _invalid_parameter_noinfo(void)
{
	abort();
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
