#include "ezport.h"

#include <string.h>
#include <ctype.h>

#include "jr/misc.h"

jr_int jr_rev_strcmp (s, t)
	const char *s, *t;
{
	return (- strcmp (s, t));
}

jr_int jr_opt_strcmp (str1, str2)
	const char *	str1;
	const char *	str2;
{
	if (str1 == 0  &&  str2 == 0)	return 0;
	if (str1 == 0  &&  str2 != 0)	return -1;
	if (str1 != 0  &&  str2 == 0)	return 1;

	return strcmp (str1, str2);
}

jr_int jr_opt_strcasecmp (str1, str2)
	const char *	str1;
	const char *	str2;
{
	if (str1 == 0  &&  str2 == 0)	return 0;
	if (str1 == 0  &&  str2 != 0)	return -1;
	if (str1 != 0  &&  str2 == 0)	return 1;

	return strcasecmp (str1, str2);
}


jr_int jr_opt_canon_strcmp (str1, str2)
	const char *	str1;
	const char *	str2;
{
	if (str1 == 0  &&  str2 == 0)	return 0;
	if (str1 == 0  &&  str2 != 0)	return -1;
	if (str1 != 0  &&  str2 == 0)	return 1;

	return jr_canon_strcmp (str1, str2);
}


jr_int jr_canon_strcmp (str1, str2)
	const char *	str1;
	const char *	str2;
{
	jr_int			diff;
	jr_int			was_alnum_1					= 0;
	jr_int			was_alnum_2					= 0;
	jr_int			skipped_space_1				= 0;
	jr_int			skipped_space_2				= 0;

	/*
	 * Ignore case and white space, except if the
	 * white space separates words (alphabetic/numeric characters),
	 * but then treat arbitrary sequences of white space as equal.
	 */

	while (1) {
		for (; *str1; str1++) {
			if (isspace (*str1)) {
				skipped_space_1		= 1;
			}
			else {
				break;
			}
		}
		for (; *str2; str2++) {
			if (isspace (*str2)) {
				skipped_space_2		= 1;
			}
			else {
				break;
			}
		}

		if (	isalnum (*str1)  &&  isalnum (*str2)
			&&	was_alnum_1  &&  was_alnum_2) {

			diff	= skipped_space_1 - skipped_space_2;

			if (diff != 0) {
				return diff;
			}
		}

		diff	= tolower (*str1) - tolower (*str2);

		if (diff != 0) {
			return diff;
		}

		if (*str1 == 0) {
			return 0;
		}

		skipped_space_1		= 0;
		skipped_space_2		= 0;

		was_alnum_1			= isalnum (*str1);
		was_alnum_2			= isalnum (*str2);

		str1++;
		str2++;
	}

	return 0;
}

jr_int jr_canon_is_prefix (str1, str2)
	const char *	str1;
	const char *	str2;
{
	jr_int			diff;
	jr_int			was_alnum_1					= 0;
	jr_int			was_alnum_2					= 0;
	jr_int			skipped_space_1				= 0;
	jr_int			skipped_space_2				= 0;

	/*
	 * Ignore case and white space, except if the
	 * white space separates words (alphabetic/numeric characters),
	 * but then treat arbitrary sequences of white space as equal.
	 */

	while (1) {
		for (; *str1; str1++) {
			if (isspace (*str1)) {
				skipped_space_1		= 1;
			}
			else {
				break;
			}
		}

		for (; *str2; str2++) {
			if (isspace (*str2)) {
				skipped_space_2		= 1;
			}
			else {
				break;
			}
		}

		if (*str1 == 0) {
			return 1;
		}


		if (	isalnum (*str1)  &&  isalnum (*str2)
			&&	was_alnum_1  &&  was_alnum_2) {

			diff	= skipped_space_1 - skipped_space_2;

			if (diff != 0) {
				return 0;
			}
		}

		diff	= tolower (*str1) - tolower (*str2);

		if (diff != 0) {
			return 0;
		}


		skipped_space_1		= 0;
		skipped_space_2		= 0;

		was_alnum_1			= isalnum (*str1);
		was_alnum_2			= isalnum (*str2);

		str1++;
		str2++;
	}

	return 0;
}
