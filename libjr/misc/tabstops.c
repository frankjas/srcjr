#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdlib.h>

#include "jr/misc.h"

jr_int jr_EnvTabStops ()
{
	char *		value_str;
	jr_int		tabstops		= 0;

	value_str	= getenv ("TABSTOPS");

	if (value_str) {
		tabstops	= atoi (value_str);
	}

	if (tabstops == 0) {
		tabstops	= 8;
	}

	return tabstops;
}
