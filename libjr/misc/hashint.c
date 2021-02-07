#include "ezport.h"

#include "jr/misc.h"
/*
	Author: Frank Jas
	Creation Date: March 1987
	Modified: Will Russell, May 1989
*/

jr_int jr_inthash (i)
	jr_int i;
{
	if (i < 0) {
		i = -i ;
		i >>= 1 ;
	}
	return(i) ;
}

jr_int jr_intphash (void_arg_1)
	const void *	void_arg_1;
{
	const jr_int *	i_ptr			= void_arg_1;
	jr_int			i				= *i_ptr ;
	
	if (i < 0) {
		i = -i ;
		i >>= 1 ;
	}

	return(i) ;
}
