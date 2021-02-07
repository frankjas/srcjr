#include "ezport.h"

#include <apr-1/apr_fnmatch.h>

#include "jr/string.h"

jr_int jr_StringHasGlobChars(
	const char *			string)
{
	jr_int					status;

	status	= apr_fnmatch_test( string);

	return status;
}

jr_int jr_StringHasGlobMatch(
	const char *			string,
	const char *			pattern,
	unsigned jr_int			flags)
{
	unsigned jr_int			apr_flags			= 0;
	jr_int					status;

	if (! (flags & jr_GLOB_MATCH_PATH_SEPARATOR)) {
		apr_flags	|=  APR_FNM_PATHNAME;
	}
	if (! (flags & jr_GLOB_MATCH_PERIOD)) {
		apr_flags	|= APR_FNM_PERIOD;
	}
	if (flags & jr_GLOB_NO_ESCAPES) {
		apr_flags	|= APR_FNM_NOESCAPE;
	}
	if (flags & jr_GLOB_IGNORE_CASE) {
		apr_flags	|= APR_FNM_CASE_BLIND;
	}

	status	= apr_fnmatch( pattern, string, apr_flags);

	return status;
}
