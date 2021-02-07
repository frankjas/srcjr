#include "ezport.h"

#include "jr/string.h"

char *jr_strnchr (
	const char *	str,
	jr_int			c,
	jr_int			len)
{
	jr_int q;

	for (q=0; str[q] && q < len; q++) {
		if (str[q] == c) {
			return (char *) str + q;
		}
	}
	return 0;
}

char *jr_rstrchr (
	const char *	end_str,
	const char *	str,
	jr_int			c)
{
	for (; end_str >= str; end_str--) {
		if (*end_str == c) {
			return (char *) end_str;
		}
	}
	return 0;
}

