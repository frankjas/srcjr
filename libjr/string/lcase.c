#include "ezport.h"

#include "jr/string.h"

void jr_ConvertToUpperCase(string)
	char *string ;
{
	for (; *string ; string++) {
		*string = jr_toupper(*string) ;
	}
}

void jr_ConvertToLowerCase (string)
	char *string ;
{
	for (; *string ; string++) {
		*string = jr_tolower(*string) ;
	}
}

void	jr_CopyAndConvertCase (target_string, bounds_arg, source_string, case_control_string)
	char *			target_string;
	jr_int			bounds_arg ;
	const char *	source_string ;
	const char *	case_control_string ;
{
	jr_int target_array_bounds ;

	target_array_bounds = bounds_arg - 1 ;	/* leave room for null */

	if ((*case_control_string == 'u') || (*case_control_string == 'U')) {
		jr_int i ;

		for (i=0; i < target_array_bounds; i++) {
			if (source_string[i] == '\0') break ;
			target_string[i] = jr_toupper(source_string[i]) ;
		}
		target_string[i] = 0 ;
	}
	else {
		jr_int i ;

		for (i=0; i < target_array_bounds; i++) {
			if (source_string[i] == '\0') break ;
			target_string[i] = jr_tolower(source_string[i]) ;
		}
		target_string[i] = 0 ;
	}
}

jr_int jr_StringIsAllUpper(str)
	const char *str ;
{
	for (; *str; str++) {
		if ((isalpha(*str)) && (! isupper(*str))) return(0) ;
	}
	return(1) ;
}

jr_int jr_StringIsAllLower(str)
	const char *str ;
{
	for (; *str; str++) {
		if ((isalpha(*str)) && (! islower(*str))) return(0) ;
	}
	return(1) ;
}
