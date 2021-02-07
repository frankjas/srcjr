#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <ctype.h>


#ifdef missing_strcasecmp

int strcasecmp (str1, str2)
	const char *		str1;
	const char *		str2;
{
	int					c1;
	int					c2;
	int					diff;

	for (; *str1 != 0  &&  *str2 != 0;  str1++, str2++) {
		c1 = toupper (*str1);
		c2 = toupper (*str2);

		diff = c1 - c2;
		if (diff != 0) return diff;
	}

	return *str1 - *str2;
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
