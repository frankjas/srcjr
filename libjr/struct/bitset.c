#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/bitset.h"
#include "jr/misc.h"
#include "jr/malloc.h"


void jr_BitSetInit (bset, num_bits)
	jr_BitSet *		bset;
	jr_int			num_bits;
{
	memset (bset, 0, sizeof (*bset));

	if (num_bits > 0) {
		jr_BitSetChangeNumBits (bset, num_bits);
	}
}

void jr_BitSetUndo (bset)
	jr_BitSet *			bset;
{
	if (bset->int_array) {
		jr_free (bset->int_array);
	}
}

jr_BitSet *jr_BitSetCreate (num_bits)
	jr_int			num_bits;
{
	jr_BitSet *		bset			= jr_malloc (sizeof (jr_BitSet));

	jr_BitSetInit (bset, num_bits);

	return bset;
}

void jr_BitSetDestroy (bset)
	jr_BitSet *			bset;
{
	jr_BitSetUndo (bset);
	jr_free (bset);
}

void jr_BitSetChangeNumBits (bset, new_num_bits)
	jr_BitSet *			bset;
	jr_int				new_num_bits;
{
	jr_int				old_num_ints;
	jr_int				new_num_ints;
	jr_int				num_last_bits;
	jr_int				last_bits_mask;
	jr_int				i;


	if (new_num_bits  <=  bset->num_bits) {
		/*
		 * 7/23/2005: don't deallocate if the bit set is shrinking, since
		 * if the application is changing the bit set size, it's likely to be
		 * changed upward again.
		 * If the application wants to deallocate a large bit set and get a small one
		 * then undo and re-init.
		 */
		bset->num_bits	= new_num_bits;
		return;
	}


	old_num_ints		= bset->num_bits >> jr_INT_BIT_SIZE_POWER;		/* division */
	new_num_ints		= new_num_bits >> jr_INT_BIT_SIZE_POWER;		/* division */

	if (bset->num_bits & jr_INT_BIT_SIZE_MASK) {						/* mod */
		old_num_ints++;
	}

	if (new_num_bits & jr_INT_BIT_SIZE_MASK) {							/* mod */
		new_num_ints++;
	}

	if (new_num_ints > old_num_ints) {
		bset->int_array		= jr_realloc (bset->int_array, new_num_ints * sizeof (jr_int));
	}

	/*
	 * zero the new bits, first do the partial int at the end of the old int_array
	 */
	num_last_bits	= bset->num_bits & jr_INT_BIT_SIZE_MASK;  	/* mod by bits-per-int */
	last_bits_mask	= 0xffffffff >> (sizeof (jr_int) * CHAR_BIT - num_last_bits);

	if (old_num_ints > 0) {
		bset->int_array[old_num_ints - 1]	&= last_bits_mask;
	}

	for (i = old_num_ints;  i < new_num_ints;  i++) {
		bset->int_array[i] = 0;
	}

	bset->num_bits				= new_num_bits;
}


void jr_BitSetUnion (bsetc, bseta, bsetb)
	jr_BitSet *bsetc;
	jr_BitSet *bseta;
	jr_BitSet *bsetb;
{
	jr_BitSetEmpty (bsetc);
	jr_BitSetAddSet (bsetc, bseta);
	jr_BitSetAddSet (bsetc, bsetb);
}
	
void jr_BitSetAddSet (dest, src)
	jr_BitSet *		dest;
	jr_BitSet *		src;
{
	jr_int			i;
	jr_int			num_ints ;

	if (dest->num_bits < src->num_bits) {
		jr_BitSetChangeNumBits (dest, src->num_bits);
	}

	num_ints		= src->num_bits >> jr_INT_BIT_SIZE_POWER;

	if (src->num_bits & jr_INT_BIT_SIZE_MASK) {
		num_ints++;
	}

	for (i = 0; i < num_ints; i++) {
		dest->int_array[i] |= src->int_array[i];
	}
}

void jr_BitSetEmpty (bset)
	jr_BitSet *			bset;
{
	register jr_int		num_ints;
	register jr_int		i;

	num_ints		= bset->num_bits >> jr_INT_BIT_SIZE_POWER;

	if (bset->num_bits & jr_INT_BIT_SIZE_MASK) {
		num_ints++;
	}

	for (i=0; i < num_ints; i++) {
		bset->int_array[i] = 0;
	}
}

void jr_BitSetAddAll (bset)
	jr_BitSet *			bset;
{
	register jr_int		num_ints;
	register jr_int		i;

	num_ints		= bset->num_bits >> jr_INT_BIT_SIZE_POWER;

	if (bset->num_bits & jr_INT_BIT_SIZE_MASK) {
		num_ints++;
	}
	for (i=0; i < num_ints; i++) {
		bset->int_array[i] = 0xffffffff;
	}
}

int jr_BitSetIsEmpty (bset)
	jr_BitSet *			bset;
{
	register jr_int		num_ints;
	register jr_int		i;
	register jr_int		last_bits_mask;
	register jr_int		num_last_bits;

	num_ints		= bset->num_bits >> jr_INT_BIT_SIZE_POWER;

	if (bset->num_bits & jr_INT_BIT_SIZE_MASK) {
		num_ints++;
	}
	for (i=0; i < num_ints; i++) {
		if (i == num_ints - 1) {
			/*
			 * The last partial int may have 1's off the end
			 */
			num_last_bits	= bset->num_bits & jr_INT_BIT_SIZE_MASK;  	/* mod by bits-per-int */
			last_bits_mask	= 0xffffffff >> (sizeof (jr_int) * CHAR_BIT - num_last_bits);

			if ((bset->int_array[i] & last_bits_mask)  !=  0) {
				return 0;
			}
		}
		else if (bset->int_array[i] != 0) {
			return 0;
		}
	}
	return 1;
}

int jr_BitSetIsFull (bset)
	jr_BitSet *			bset;
{
	jr_int				num_ints;
	jr_int				i;
	unsigned jr_int		last_bits_mask;
	jr_int				num_last_bits;

	num_ints		= bset->num_bits >> jr_INT_BIT_SIZE_POWER;

	if (bset->num_bits & jr_INT_BIT_SIZE_MASK) {
		num_ints++;
	}
	for (i=0; i < num_ints; i++) {
		if (i == num_ints - 1) {
			/*
			 * The last partial int may have 0's off the end
			 */
			num_last_bits	= bset->num_bits & jr_INT_BIT_SIZE_MASK;  	/* mod by bits-per-int */
			last_bits_mask	= 0xffffffff >> (sizeof (jr_int) * CHAR_BIT - num_last_bits);

			if ((bset->int_array[i] & last_bits_mask)  !=  last_bits_mask) {
				return 0;
			}
		}
		else if (~bset->int_array[i] != 0) {
			return 0;
		}
	}
	return 1;
}


int jr_BitSetSize (bset)
	jr_BitSet *			bset;
{
	jr_int				b;
	jr_int				count			= 0;

	jr_BitSetForEachElement (bset, b) {
		count++;
	}

	return (count);
}
