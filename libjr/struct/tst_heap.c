#include <stdio.h>
#include "jr/misc.h"
#include "jr/heap.h"

main ()
{
	jr_AList *heap;
	jr_int i, *ip;
	char buf[8];

	heap = jr_AListCreate (2 * sizeof(jr_int));

	fprintf (stderr, "Enter numbers for heapify test:\n");
	while (OBSOLETE_getln (stdin, buf) != EOF && (i = atoi (buf))) {
		jr_AListSetNewTail (heap, &i);
	}

	fprintf (stderr, "Size of Heap == %d\n", jr_HeapSize(heap));
	fprintf (stderr, "Last node == %d\n", jr_HeapLastNodeIndex(heap));
	fprintf (stderr, "Last leaf == %d\n", jr_HeapLastLeafIndex(heap));

	jr_Heapify (heap, jr_intpcmp);

	i = 0;
	jr_AListForEachElementPtr (heap, ip, jr_int *) {
		fprintf (stderr, "%d: %d\tparent %d, right %d left %d\n",
			i++, *ip, 
			jr_HeapParentIndex(heap, i), jr_HeapRightChildIndex(heap, i), jr_HeapLeftChildIndex(heap, i));
	}
	putchar ('\n');

	jr_AListEmpty (heap);

	fprintf (stderr, "Enter numbers for INSERT heap test:\n");
	while (OBSOLETE_getln (stdin, buf) != EOF && (i = atoi (buf))) {
		jr_HeapInsert (heap, &i, jr_intpcmp);
	}

	fprintf (stderr, "Size of Heap == %d\n", jr_HeapSize(heap));
	fprintf (stderr, "Last node == %d\n", jr_HeapLastNodeIndex(heap));
	fprintf (stderr, "Last leaf == %d\n", jr_HeapLastLeafIndex(heap));

	i = 0;
	jr_AListForEachElementPtr (heap, ip, jr_int *) {
		fprintf (stderr, "%d: %d\tparent %d, right %d left %d\n",
			i++,
			*ip, jr_HeapParentIndex(heap, i), jr_HeapRightChildIndex(heap, i), jr_HeapLeftChildIndex(heap, i));
	}
	putchar ('\n');

	while (jr_HeapSize(heap) > 0) {
		jr_HeapDeleteMin (heap, &i, jr_intpcmp);
		fprintf (stderr, "%d\n", i);
	}
	fprintf (stderr, "Size of Heap == %d\n", jr_HeapSize(heap));
	fprintf (stderr, "Last node == %d\n", jr_HeapLastNodeIndex(heap));
	fprintf (stderr, "Last leaf == %d\n", jr_HeapLastLeafIndex(heap));
}
