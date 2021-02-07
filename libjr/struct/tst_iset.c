#include "ezport.h"

#include <stdio.h>
#include "jr/intset.h"
#include "jr/malloc.h"

void main ()
{
	jr_int i, n;
	jr_IntSet *iset1, *iset2, *iset3;

	jr_malloc_stats (stdout, "jr_IntSet - before initialization");
	iset1 = jr_IntSetCreate ();
	iset2 = jr_IntSetCreate ();
	iset3 = jr_IntSetCreate ();
	jr_malloc_stats (stdout, "jr_IntSet - after initialization");

	printf ("--------jr_IntSetAddMax, ForEach loop--------\n");
	for (i=0; i < 100; i++) {
		if (i % 6 == 0) jr_IntSetAddMax (iset1, i);
	}
	jr_IntSetForEachElement(iset1, i, n) {
		printf ("%3d:\t%3d\n", i, n);
	}
	printf ("--------jr_IntSetAdd, ForEach loop counting down--------\n");
	for (i=120; i > 0; i--) {
		if (i % 5 == 0) jr_IntSetAdd (iset1, i);
	}
	jr_IntSetForEachElement(iset1, i, n) {
		printf ("%3d:\t%3d\n", i, n);
	}
	jr_malloc_stats (stdout, "jr_IntSet - after adds");
	printf ("--------jr_IntSetIntersection (5 , 6 and 8 mults)--------\n");
	for (i=100; i > 0; i--) {
		if (i % 8 == 0) jr_IntSetAdd (iset2, i);
	}
	jr_IntSetIntersection (iset3, iset1, iset2);
	jr_IntSetForEachElement(iset3, i, n) {
		printf ("%3d:\t%3d\n", i, n);
	}
	printf ("--------jr_IntSetUnion (5, 6, and 8 mults)--------\n");
	jr_IntSetUnion (iset3, iset1, iset2);
	jr_IntSetForEachElement(iset3, i, n) {
		printf ("%3d:\t%3d\n", i, n);
	}
	printf ("--------jr_IntSetAddSet (5, 6, and 8 mults)--------\n");
	jr_IntSetAddSet (iset1, iset2);
	jr_IntSetForEachElement(iset1, i, n) {
		printf ("%3d:\t%3d\n", i, n);
	}
	printf ("--------jr_IntSetCmp (Added, Unioned sets)--------\n");
	printf ("%s\n", jr_IntSetCmp (iset1, iset3) ? "not equal" : "equal");
	printf ("--------jr_IntSetFind (print those not found)--------\n");
	jr_IntSetForEachElement(iset1, i, n) {
		if (!jr_IntSetFind (iset1, n)) printf ("%3d not found\n", n);
	}
	printf ("--------jr_IntSetDelete (print those not found)--------\n");
	jr_IntSetForEachElement(iset2, i, n) {
		jr_IntSetDelete (iset1, n);
		if (i == 0) {
			printf ("After deleting %d from one set: %s\n", n,
					jr_IntSetCmp (iset1, iset3) ? "not equal" : "equal");
		}
		if (!jr_IntSetFind (iset1, n)) printf ("%3d not found\n", n);
	}
	jr_IntSetDestroy (iset1);
	jr_IntSetDestroy (iset2);
	jr_IntSetDestroy (iset3);
	jr_malloc_stats (stdout, "jr_IntSet: after jr_freeing");

#ifdef INT_STRING_CONVERSION
	printf ("--------jr_IntSetCreateFromIntStr--------\n");
	k = 0;
	for (i=10; i > 0; i--) {
		  /* for each number from 1 - 10 (counting backwards to
		  screw up the sort more */
		for (j=0; j < i; j++) ia[k++] = i;
		  /* add that number i times to the array */
	}
	ia[k] = jr_INT_NULL;
	iset1 = jr_IntSetCreateFromIntStr (ia);
	printf ("--------after jr_IntSetCreateFromIntStr--------\n");
	jr_IntSetForEachElement(iset1, i, n) {
		printf ("%3d:\t%3d\n", i, n);
	}
	jr_IntSetFree(iset1);
	jr_malloc_stats (stdout, "jr_IntSet: after jr_free");
#endif
}
