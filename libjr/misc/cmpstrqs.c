#include "ezport.h"

#include <string.h>

#include "jr/string.h"


jr_int jr_strpcmp(void_arg_1,void_arg_2)
	const void *		void_arg_1;
	const void *		void_arg_2;
{
	char **				sa1			= (void *) void_arg_1 ;
	char **				sa2			= (void *) void_arg_2 ;

	return(strcmp(*sa1,*sa2)) ;
}

jr_int jr_stricmp(void_arg_1, void_arg_2)
	const void *		void_arg_1;
	const void *		void_arg_2;
{
	char *		str1		= (void *) void_arg_1;
	char *		str2		= (void *) void_arg_2;
	jr_int		c1;
	jr_int		c2;
	jr_int		diff;

	for (; *str1 != 0  &&  *str2 != 0;  str1++, str2++) {
		c1 = jr_toupper (*str1);
		c2 = jr_toupper (*str2);

		diff = c1 - c2;
		if (diff != 0) return diff;
	}

	return *str1 - *str2;
}
