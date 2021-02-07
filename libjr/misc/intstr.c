#include "ezport.h"

#include "jr/misc.h"
#include <limits.h>

/*
	Author: Frank Jas
	Creation Date: March 1987
*/

jr_int jr_intstrcmp (is1, is2)
	const jr_int *is1 ;
	const jr_int *is2;
{
	for (; *is1 != jr_INT_NULL; is1++, is2++) {
		if (*is2 == jr_INT_NULL) return (1);
		if (*is1 != *is2) return (*is1 - *is2);
	}
	if (*is2 == jr_INT_NULL) return (0);
	return (-1);
	/* is1 is shorter, hence it comes before is2 */
}

jr_int jr_intstrlen (is)
	const jr_int *is;
{
	jr_int n;

	for (n = 0; *is != jr_INT_NULL; is++, n++);
	return (n);
}

jr_int jr_intstrbytes (is)
	const jr_int *is;
{
	jr_int n;

	for (n = 0; *is != jr_INT_NULL; is++, n++);
	return ((n + 1) * sizeof(jr_int));
}

jr_int jr_intstrhash (is)
	const jr_int *is;
{
	jr_int len = jr_intstrlen (is),
		sub = len / 3,
		val;
	
	/* a hashing function derived from Knuth's fundamental algorithms 
	 * is not good if the integers are uniformly small (i.e. < 20)
	 */
	val = (*is + *(is+sub) + *(is+2*sub) + *(is+len-1)+len) * len;
	if (val < 0) return (-val);
	return (val);
}

jr_int jr_intstrlenhash (is, len)
	const jr_int *is ;
	jr_int len;
		 /* a hashing function derived from Knuth's 
		  * fundamental algorithms 
		  */
{
	jr_int sub = len / 3,
		val;
	
	len /= sizeof(jr_int);
	 /* len is in bytes, we need the number of ints to avoid
	  * running off the end of is 
	  */
	sub = len / 3,
	val = (*is + *(is+sub) + *(is+2*sub) + *(is+len-1)+len) * len;
	if (val < 0) return (-val);
	return (val);
}
