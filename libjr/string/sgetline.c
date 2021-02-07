#include "ezport.h"

#include <stdio.h>

#include "jr/string.h"

char *jr_StringFgets(target, bounds_arg, source)
	char *			target ;
	jr_int			bounds_arg ;
	const char *	source ;
{
	jr_int target_array_bounds ;
	jr_int i ;

	if (*source == '\0') return(0) ;

	target_array_bounds = bounds_arg - 1 ;

	for (i=0; i < target_array_bounds; i++) {
		if (*source == '\0') break ;
		if (*source == '\n') {
			*target = '\n' ; target++ ;
			break ;
		}

		*target = *source ;

		target++ ;
		source++ ;
	}
	*target = 0 ;

	while (*source && (*source != '\n')) source++ ;

	if (*source == '\n') source++ ;

	return((char *) source) ;
}

/*
void main()
{
	char *	input_buf ;
	char *	input_ptr ;
	char 	line_buf[128] ;
	jr_int	i ;

	input_buf = "test\ntwo\nthree\nfour\n\nsix\n\n" ;
	input_ptr = input_buf ;
	
	i = 0 ;
	while (input_ptr = jr_StringFgets(line_buf, sizeof(line_buf), input_ptr)) {
		i++ ;

		fprintf(stdout, "%d) %s", i, line_buf) ;
	}
}
*/
