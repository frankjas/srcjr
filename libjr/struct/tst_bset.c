#include "ezport.h"

#include <stdio.h>

#include "jr/bitset.h"
#include "jr/malloc.h"

void main ()
{
	jr_int j;
	jr_int bit;
	jr_BitSet *bitset1, *bitset2, *bitset3;

	jr_malloc_stats (stderr, "jr_BitSet - before initialization");

	fprintf (stderr, "--------jr_BitSetCreate\n");
	fprintf (stderr, "\tbitset1 = jr_BitSetCreate (31)\n");
	fprintf (stderr, "\tbitset2 = jr_BitSetCreate (20)\n");
	fprintf (stderr, "\tbitset3 = jr_BitSetCreate (100)--------\n");

	bitset1 = jr_BitSetCreate (31);
	bitset2 = jr_BitSetCreate (20);
	bitset3 = jr_BitSetCreate (100);

	jr_malloc_stats (stderr, "jr_BitSet - after initialization");

	fprintf (stderr, "bitset1 size = %d num bits %d\n", 
			jr_BitSetSize(bitset1), jr_BitSetNumBits (bitset1));
	fprintf (stderr, "bitset2 size = %d num bits %d\n",
			jr_BitSetSize(bitset2), jr_BitSetNumBits (bitset2));
	fprintf (stderr, "bitset3 size = %d num bits %d\n",
			jr_BitSetSize(bitset3), jr_BitSetNumBits (bitset3));

	fprintf (stderr, "--------jr_BitSetAddElement (0, 2, 17, 31), jr_BitSetPrint-------\n");
	bit = 0;
	fprintf (stderr, "bit %d -> set[%d] |= %08x\n",
			bit, (bit) >> jr_INT_BIT_SIZE_POWER, (1 << (bit & jr_INT_BIT_SIZE_MASK)));
	bit = 2;
	fprintf (stderr, "bit %d -> set[%d] |= %08x\n",
			bit, (bit) >> jr_INT_BIT_SIZE_POWER, (1 << (bit & jr_INT_BIT_SIZE_MASK)));
	bit = 17;
	fprintf (stderr, "bit %d -> set[%d] |= %08x\n",
			bit, (bit) >> jr_INT_BIT_SIZE_POWER, (1 << (bit & jr_INT_BIT_SIZE_MASK)));
	bit = 31;
	fprintf (stderr, "bit %d -> set[%d] |= %08x\n",
			bit, (bit) >> jr_INT_BIT_SIZE_POWER, (1 << (bit & jr_INT_BIT_SIZE_MASK)));

	jr_BitSetAddElement (bitset1, 0);
	jr_BitSetAddElement (bitset1, 2);
	jr_BitSetAddElement (bitset1, 17);
	jr_BitSetAddElement (bitset1, 31);

	fprintf (stderr, "bitset1: ");
	jr_BitSetPrint (bitset1, stderr);
	fprintf (stderr, "\n");

	bit	= 0;
	if (!jr_BitSetHasElement (bitset1, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	bit	= 2;
	if (!jr_BitSetHasElement (bitset1, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	bit	= 17;
	if (!jr_BitSetHasElement (bitset1, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	bit	= 31;
	if (!jr_BitSetHasElement (bitset1, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	fprintf (stderr, "--------jr_BitSetAddSet (bitset3, bitset1)----------\n");
	jr_BitSetAddSet (bitset3, bitset1);
	jr_BitSetForEachElement (bitset3, j) {
		fprintf (stderr, "%d ", j);
	}
	fprintf (stderr, "\n");


	fprintf (stderr, "-------- bitset2----------\n");
	jr_BitSetAddElement (bitset2, 1);
	jr_BitSetAddElement (bitset2, 5);
	jr_BitSetAddElement (bitset2, 15);
	jr_BitSetAddElement (bitset2, 20);

	bit	= 1;
	if (!jr_BitSetHasElement (bitset2, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	bit	= 5;
	if (!jr_BitSetHasElement (bitset2, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	bit	= 15;
	if (!jr_BitSetHasElement (bitset2, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	bit	= 20;
	if (!jr_BitSetHasElement (bitset2, bit)) {
		fprintf (stderr, "Bit %d should be present but isn't\n", bit);
		exit (1);
	}

	jr_BitSetForEachElement (bitset2, j) {
		fprintf (stderr, "%d ", j);
	}
	fprintf (stderr, "\n");

	fprintf (stderr, "--------jr_BitSetAddSet (bitset3, bitset2)----------\n");
	jr_BitSetAddSet (bitset3, bitset2);
	jr_BitSetForEachElement (bitset3, j) {
		fprintf (stderr, "%d ", j);
	}
	fprintf (stderr, "\n");

	fprintf (stderr, "--------bitset3 = jr_BitSetCreate (200)--------\n");

	jr_BitSetDestroy(bitset3);
	bitset3 = jr_BitSetCreate (200);

	fprintf (stderr, "bitset3 size = %d num bits %d\n",
			jr_BitSetSize(bitset3), jr_BitSetNumBits (bitset3));

	fprintf (stderr, "--------jr_BitSetAddSet (bitset3, bitset1)----------\n");
	jr_BitSetAddSet (bitset3, bitset1);
	jr_BitSetForEachElement (bitset3, j) {
		fprintf (stderr, "%d ", j);
	}
	fprintf (stderr, "\n");

	jr_BitSetAddAll (bitset2);
	fprintf (stderr, "--------jr_BitSetAddAll (bitset2) (is full %d) ----------\n",
		jr_BitSetIsFull (bitset2)
	);
	jr_BitSetForEachElement (bitset2, j) {
		fprintf (stderr, "%d ", j);
	}
	fprintf (stderr, "\n");

	jr_BitSetDeleteElement (bitset2, 10);
	fprintf (stderr, "--------jr_BitSetDelete (bitset2, 10) (is full %d) ----------\n",
		jr_BitSetIsFull (bitset2)
	);
	fprintf (stderr, "--------jr_BitChangeSize (bitset2, 15)----------\n");
	jr_BitSetChangeNumBits (bitset2, 15);
	jr_BitSetForEachElement (bitset2, j) {
		fprintf (stderr, "%d ", j);
	}
	fprintf (stderr, "\n");

	fprintf (stderr, "--------jr_BitChangeSize (bitset2, 25)----------\n");
	jr_BitSetChangeNumBits (bitset2, 25);
	jr_BitSetForEachElement (bitset2, j) {
		fprintf (stderr, "%d ", j);
	}
	fprintf (stderr, "\n");

	jr_BitSetDestroy (bitset1);
	jr_BitSetDestroy (bitset2);
	jr_BitSetDestroy (bitset3);
	jr_malloc_stats (stderr, "jr_BitSet - after jr_free");
}

