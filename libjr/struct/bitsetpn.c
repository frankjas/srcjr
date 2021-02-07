#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>

#include "jr/bitset.h"

void jr_BitSetPrint (bset, wfp)
	jr_BitSet *	bset;
	FILE *		wfp;
{
	register jr_int		num_ints		= bset->num_bits >> jr_INT_BIT_SIZE_POWER;
	jr_int				i;

	if (bset->num_bits & jr_INT_BIT_SIZE_MASK) {
		 /* If n is not a multiple of the number of bites per int, then
		  * add another int.
		  */
		num_ints++;
	}

	for (i=0; i < num_ints; i++) {
		fprintf (wfp, "%08x|", bset->int_array[i]);
	}
}
