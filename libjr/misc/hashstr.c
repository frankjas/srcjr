#include "jr/misc.h"

#include <limits.h>
#include <ctype.h>
#include <string.h>

jr_int jr_strhash_old (void_arg_1)
	const void *	void_arg_1;
{
	const char *	str				= void_arg_1;
	size_t			length			= strlen (str);
	unsigned jr_int	hash_value		= 0;
	jr_int			i;
	
	for (i=0; str[i]; i++) {
		hash_value += str[i];
	}
	hash_value *= (unsigned jr_int) length;

	return hash_value;
}
/* .bp */

#define BITS_IN_int					(sizeof (jr_int) * CHAR_BIT)
#define THREE_QUARTERS				((jr_int) ((BITS_IN_int * 3) / 4))
#define ONE_EIGHTH					((jr_int) (BITS_IN_int / 8))
#define HIGH_BITS					( ~((unsigned jr_int) (~0) >>  ONE_EIGHTH))

jr_int jr_strhash_pjw (void_arg_1)
	const void *	void_arg_1;
{
	const char *	string			= void_arg_1;
	/*
	 * An adaptation of Peter Weinberger's generic hashing algorithm
	 * based on Allen Holub's version.  From Dr. Dobbs, April, 1996
	 */

	unsigned jr_int	hash_value;
	jr_int			i;

	for (hash_value = 0;  *string;  string++) {
		hash_value = (hash_value << ONE_EIGHTH) + *string;
		if ((i = hash_value & HIGH_BITS) != 0) {
			hash_value = (hash_value ^ (i >> THREE_QUARTERS)) & ~HIGH_BITS;
		}
	}
	return hash_value;
}

jr_int jr_strcasehash_pjw (void_arg_1)
	const void *	void_arg_1;
{
	const char *	string			= void_arg_1;
	/*
	 * An adaptation of Peter Weinberger's generic hashing algorithm
	 * based on Allen Holub's version.  From Dr. Dobbs, April, 1996
	 */

	unsigned jr_int	hash_value;
	jr_int			i;

	for (hash_value = 0;  *string;  string++) {
		hash_value = (hash_value << ONE_EIGHTH) + toupper (*string);
		if ((i = hash_value & HIGH_BITS) != 0) {
			hash_value = (hash_value ^ (i >> THREE_QUARTERS)) & ~HIGH_BITS;
		}
	}
	return hash_value;
}

jr_int jr_strhash (void_arg_1)
	const void *	void_arg_1;
{
	const char *	str				= void_arg_1;
	size_t			length			= strlen (str);
	jr_int			hash_value		= 0;
	jr_int			i;
	
	for (i=0; str[i]; i++) {
		hash_value += str[i] * ((unsigned jr_int) length + i);
	}

	return hash_value;
}

