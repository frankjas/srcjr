#include "ezport.h"

#include "jr/misc.h"
/*
	Author: Frank Jas
	Creation Date: March, 1987
*/

jr_int shorthash (i)
	short i;
{
	return ((i*i)+1);
}

/*
jr_int jr_intphash (n)
	jr_int *n;
{
	return ((*n * *n)+1);
}
*/

jr_int shortphash (n)
	short *n;
{
	jr_int i = (*n  *  *n) ;

	if (i < 0) {
		i = -i ;
		i >>= i ;
	}
	return (i) ;
}
