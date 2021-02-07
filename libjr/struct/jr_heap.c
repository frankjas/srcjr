#include "jr/heap.h"
#include "jr/misc.h"

void jr_HeapInsert (heap, obj, cmpfn)
	jr_AList *heap;
	void *obj;
	jr_int (*cmpfn) ();
{
	memcpy (jr_AListNewTailPtr (heap), obj, jr_HeapElementSize(heap)); 

	if (jr_HeapSize(heap) > 1) {
		jr_HeapPercolateUp (heap, jr_HeapLastLeafIndex(heap), cmpfn);
	}
}

void jr_HeapDeleteMin (heap, obj, cmpfn)
	jr_AList *heap;
	void *obj;
	jr_int (*cmpfn) ();
{
	/* save the root
	 */
	memcpy (obj, jr_HeapElementPtr(heap, 0), jr_HeapElementSize(heap)); 

	/* copy the last leaf into root
	 */
	memcpy (jr_HeapElementPtr(heap, 0), jr_HeapLastLeafPtr(heap), jr_HeapElementSize(heap));

	jr_AListDeletedTailPtr (heap);

	jr_HeapPercolateDown (heap, 0, cmpfn);
}

void jr_Heapify (heap, cmpfn)
	jr_AList *heap;
	jr_int (*cmpfn) ();
{
	jr_int node_index;

	for (node_index = jr_HeapLastNodeIndex(heap); node_index >= 0; node_index--) {
		jr_HeapPercolateDown (heap, node_index, cmpfn);
	}
}

void jr_HeapPercolateDown (heap, node_index, cmpfn)
	jr_AList *heap;
	jr_int node_index;
	jr_int (*cmpfn) ();
{
	void *node_ptr, *right_child_ptr, *left_child_ptr;


	if (!jr_HeapHasRightChildIndex (heap, node_index)) return;
	/*
	 * it has no children, return
	 */

	node_ptr		= jr_HeapElementPtr(heap, node_index);
	right_child_ptr	= jr_HeapRightChildPtr (heap, node_index);
	left_child_ptr	= jr_HeapLeftChildPtr (heap, node_index);

	/* 
	 * right <= node and (no left or right <= left)
	 * switch right and node
	 */
	if ((*cmpfn) (right_child_ptr, node_ptr) <= 0 &&
		(!jr_HeapHasLeftChildIndex(heap, node_index) ||
		(*cmpfn) (right_child_ptr, left_child_ptr) <= 0)) {

		jr_memswitch (node_ptr, right_child_ptr, jr_HeapElementSize(heap));

		if (!jr_HeapIsNodeIndex(heap, jr_HeapRightChildIndex(heap, node_index))) return;

		jr_HeapPercolateDown (heap, jr_HeapRightChildIndex(heap, node_index), cmpfn);
		return;
	}

	/* we know right > node or 
	 * right <= node and has left and left < right
	 */
	/* check if has left and left < node and left <= right
	 * switch left and node
	 */
	if (jr_HeapHasLeftChildIndex(heap, node_index) &&
		(*cmpfn) (left_child_ptr, node_ptr) <= 0) {

		jr_memswitch (node_ptr, left_child_ptr, jr_HeapElementSize(heap));

		if (!jr_HeapIsNodeIndex(heap, jr_HeapLeftChildIndex(heap, node_index))) return;

		jr_HeapPercolateDown (heap, jr_HeapLeftChildIndex(heap, node_index), cmpfn);
		return;
	}
}


void jr_HeapPercolateUp (heap, node_index, cmpfn)
	jr_AList *heap;
	jr_int node_index;
	jr_int (*cmpfn) ();
{
	void *node_ptr, *parent_ptr;

	node_ptr   = jr_HeapElementPtr(heap, node_index);
	parent_ptr = jr_HeapParentPtr(heap, node_index);

	if ((*cmpfn) (node_ptr, parent_ptr) < 0) {
		jr_memswitch (node_ptr, parent_ptr, jr_HeapElementSize(heap));

		if (!jr_HeapIsRootIndex(heap, jr_HeapParentIndex(heap, node_index))) {
			jr_HeapPercolateUp (heap, jr_HeapParentIndex(heap, node_index), cmpfn);
		}
	}
}

/*
void HeapToSortedAL (heap, cmpfn)
	jr_AList *heap;
	jr_int (*cmpfn) ();
{
	jr_int size = jr_HeapSize(heap);

	while (jr_HeapSize(heap) > 0) {
		jr_memswitch (jr_HeapElementPtr(heap, 0), jr_HeapLastLeafPtr(heap),
			jr_HeapElementSize(heap)); 

		jr_AListDeletedTailPtr (heap);

		jr_HeapPercolateDown (heap, 0, cmpfn);
	}
}
*/

