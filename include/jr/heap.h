#ifndef _heaph

#include "ezport.h"

#include "jr/alist.h"

extern void jr_HeapInsert PROTO((jr_AList *heap, void *obj, jr_int (*cmpfn)()));
extern void jr_HeapDeleteMin PROTO((jr_AList *heap, void *obj, jr_int (*cmpfn)()));
extern void jr_Heapify PROTO((jr_AList *heap, jr_int (*cmpfn)()));
extern void jr_HeapPercolateDown PROTO((jr_AList *heap, jr_int node, jr_int (*cmpfn)()));
extern void jr_HeapPercolateUp PROTO((jr_AList *heap, jr_int node, jr_int (*cmpfn)()));

#define jr_HeapSize(heap) jr_AListSize(heap)
#define jr_HeapElementSize(heap) jr_AListElementSize(heap)

#define jr_HeapIsEmpty(heap) jr_AListIsEmpty(heap)
#define jr_HeapIsEmpty(heap) jr_AListIsEmpty(heap)

#define jr_HeapLastLeafIndex(heap) (jr_HeapSize(heap)-1)
#define jr_HeapLastNodeIndex(heap) jr_HeapParentIndex(heap, jr_HeapLastLeafIndex(heap))

#define jr_HeapIsNodeIndex(heap, node) ((node) <= jr_HeapLastNodeIndex(heap))
#define jr_HeapIsRootIndex(heap, node) ((node) == 0)

/*
 * changed below 1/19/94 to take dummy heap pointer for
 * added consistency.  May break code using these macors
 * directly, however I hope they are only referenced in 
 * the macros below.  I already changed the references
 * in heap.c and heaptest.c
 */

#define jr_HeapRightChildIndex(heap, parent)	(((parent) << 1) + 1)
#define jr_HeapLeftChildIndex(heap, parent)		(((parent) << 1) + 2)
#define jr_HeapParentIndex(heap, child) 		(((child)-1) >> 1)

#define jr_HeapElementPtr(heap, node)	jr_AListElementPtr(heap, node)
#define jr_HeapRightChildPtr(heap, p)	jr_HeapElementPtr(heap, jr_HeapRightChildIndex(heap, p))
#define jr_HeapLeftChildPtr(heap, p)	jr_HeapElementPtr(heap, jr_HeapLeftChildIndex(heap, p))
#define jr_HeapParentPtr(heap, child)	jr_HeapElementPtr(heap, jr_HeapParentIndex(heap, child))

#define jr_HeapHasRightChildIndex(heap, parent)		\
		(jr_HeapRightChildIndex(heap, parent) < jr_HeapSize(heap))

#define jr_HeapHasLeftChildIndex(heap, parent)		\
		(jr_HeapLeftChildIndex(heap, parent) < jr_HeapSize(heap))

#define jr_HeapLastLeafPtr(heap) jr_HeapElementPtr(heap, jr_HeapLastLeafIndex(heap))

#endif
