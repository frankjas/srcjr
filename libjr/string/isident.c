#include "ezport.h"

#include "jr/string.h"

jr_int jr_StringIsValidIdentifier (str)
	const char *	str;
{
	/*
	 * first character must be alphabetic or underscore
	 */

	if ((!isalpha(*str)) && (*str != '_')) {
		return (0);
	}
	str++;

	for (; *str; str++) {
		if (!isalnum(*str) && *str != '_') {
			return (0);
		}
	}
	return (1);
}
