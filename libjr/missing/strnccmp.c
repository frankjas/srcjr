#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <ctype.h>


#ifdef missing_strncasecmp

int strncasecmp(
	const char *		str1,
	const char *		str2,
	size_t				max_chars)
{
	int					c1;
	int					c2;
	int					k;
	int					diff;

	for (k = 0; *str1 != 0  &&  *str2 != 0  &&  k < max_chars;  str1++, str2++, k++) {
		c1 = toupper (*str1);
		c2 = toupper (*str2);

		diff = c1 - c2;
		if (diff != 0) return diff;
	}

	return *str1 - *str2;
}


#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
