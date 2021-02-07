#ifndef _llisth
#define _llisth

#include "ezport.h"

#include <string.h>
#include <stdarg.h>

typedef struct {
	void *		tail;
	void *		head;
	jr_int		objsize;
	jr_int		prefix_size;

	unsigned	allocates_elements		: 1;
} jr_LList;

extern jr_LList *	jr_LListCreate			PROTO ((jr_int objsize));
extern void			jr_LListDestroy			PROTO ((jr_LList *list));

extern void			jr_LListInit			PROTO ((jr_LList *list, jr_int objsize));
extern void			jr_LListUndo			PROTO ((jr_LList *list));

extern void			jr_LListEmpty			PROTO ((jr_LList *list));

extern void *		jr_LListAppendElement	PROTO ((jr_LList *list, void *next_el, void *new_el));

extern void *		jr_LListExtractNextElement		PROTO ((jr_LList *list, void *curr));
extern void			jr_LListDeleteNextElement		PROTO ((jr_LList *list, void *curr));
extern void			jr_LListMakeNullElement			PROTO ((jr_LList *dlist, void *el));

extern jr_int		jr_LListElementPrefixSize		PROTO ((jr_LList *list));
extern void			jr_LListSetPrefixSize			PROTO ((jr_LList *list, jr_int prefix_size));

extern void			jr_LListTransformPrefix	PROTO ((
						jr_LList *			list,
						void *				el_ptr,
						unsigned jr_int		magnitude,
						jr_int				which_way,
						jr_int				all_elements
					));

extern void			jr_LListSetContainers		PROTO ((jr_LList *list, ...));

extern void			jr_LListSetContainersFromVA_List	PROTO ((
						jr_LList *				list,
						va_list					arg_list
					));

/***** List Query Operations *****/

#define jr_LListHeadPtr(list)					((list)->head)
#define jr_LListTailPtr(list)					((list)->tail)


#define jr_LListIsEmpty(list)					((list)->tail == 0)
#define jr_LListElementSize(list)				((list)->objsize)
#define jr_LListPrefixSize(list)				((list)->prefix_size)
#define jr_LListAllocatesElements(list)			((list)->allocates_elements)
#define jr_LListSetAllocatesElements(list, v)	((list)->allocates_elements = (v))

#define	jr_LListHasMultipleElements(list)		\
		(!jr_LListIsEmpty (list)  &&  jr_LListHeadPtr (list) != jr_LListTailPtr (list))


/***** Element Allocation *****/

extern void *		jr_LListAllocateElementWithSize		PROTO ((jr_LList *list, jr_int size));


extern void *		jr_NewLListElementWithPrefix		PROTO ((
						jr_int							element_size,
						jr_int							prefix_size
					));

#define				jr_FreeLListElementWithPrefix(objptr, prefix_size)		\
					jr_free ((char *) (objptr) - (prefix_size) - sizeof (void *))

#define				jr_NextLListElementPtrWithPrefix(objptr, prefix_size)	\
					(((void **) ((char *) (objptr) - (prefix_size)))[-1])



/***** Looping Constructs *****/

#define jr_LListForEachElementPtrWithPrefix(list, current, prefix_size) 		\
		for ((current) = jr_LListHeadPtr (list);								\
			 (current) != 0;													\
			 (current) = jr_NextLListElementPtrWithPrefix(current, prefix_size)		\
		)

#if defined(__cplusplus)
#	undef jr_LListForEachElementPtrWithPrefix


#	define	jr_LListForEachElementPtrWithPrefix(list, current, prefix_size)				\
			for (	(*(void **)&(current)) = jr_LListHeadPtr (list);					\
					(current) != 0 ;													\
					(*(void **)&(current)) = jr_NextLListElementPtrWithPrefix (current, prefix_size)\
				)

#endif


/***** Convenience Functions *****/

#define				jr_LListAllocateElement(list)							\
					jr_LListAllocateElementWithSize (list, jr_LListElementSize (list))

#define				jr_LListFreeElement(list, objptr)	\
					jr_FreeLListElementWithPrefix (objptr, (list)->prefix_size);


#define				jr_NewLListElement(el_size)			\
					jr_NewLListElementWithPrefix (el_size, 0)

#define				jr_FreeLListElement(objptr)		\
					jr_FreeLListElementWithPrefix (objptr, 0)

#define				jr_NextLListElementPtr(objptr)							\
					jr_NextLListElementPtrWithPrefix (objptr, 0)


#define				jr_LListNewNextElementPtrWithSize(list, prev, size)		\
					jr_LListAppendElement (list, prev, jr_LListAllocateElementWithSize (list, size))


#define				jr_LListNewHeadPtrWithSize(list, size)					\
					jr_LListNewNextElementPtrWithSize (list, 0, size)


#define				jr_LListNewTailPtrWithSize(list, size)					\
					jr_LListNewNextElementPtrWithSize (list, jr_LListTailPtr (list), size)



#define				jr_LListNewNextElementPtr(list, prev)					\
					jr_LListNewNextElementPtrWithSize(list, prev, jr_LListElementSize (list))

#define				jr_LListNewHeadPtr(list)								\
					jr_LListNewHeadPtrWithSize(list, jr_LListElementSize (list))

#define				jr_LListNewTailPtr(list)								\
					jr_LListNewTailPtrWithSize(list, jr_LListElementSize (list))

#define				jr_LListInsertToHead(list, prev)						\
					jr_LListAppendElement (list, 0, prev)

#define				jr_LListAppendToTail(list, next)						\
					jr_LListAppendElement (list, jr_LListTailPtr (list), next)



#define				jr_LListSetNewHeadWithSize(list, obj, size)	\
					memcpy (															\
						jr_LListNewHeadPtrWithSize(list, size), obj, size				\
					)

#define				jr_LListSetNewTailWithSize(list, obj, size)							\
					memcpy ( 															\
						jr_LListNewTailPtrWithSize(list, size), obj, size				\
					)

#define				jr_LListSetNewNextElementWithSize(list, el, obj, size)				\
					memcpy (															\
						jr_LListNewNextElementPtrWithSize (list, el, size), obj, size	\
					)

#define				jr_LListSetNewHead(list, obj)										\
					jr_LListSetNewHeadWithSize (list, obj, jr_LListElementSize (list))

#define				jr_LListSetNewTail(list, obj)										\
					jr_LListSetNewTailWithSize (list, obj, jr_LListElementSize (list))

#define				jr_LListSetNewNextElement(list, el, obj)							\
					jr_LListSetNewNextElementWithSize (list, el, obj, jr_LListElementSize (list))


#define				jr_LListGetTail(list, elptr) \
					memcpy (elptr, jr_LListTailPtr(list), (list)->element_size)

#define				jr_LListGetHead(list, elptr) \
					memcpy (elptr, jr_LListHeadPtr(list), (list)->element_size)


#define				jr_LListDeleteHead(list)								\
					jr_LListDeleteNextElement (list, 0)


extern jr_LList *	jr_LListCopy		PROTO ((
						jr_LList *		dest,
						jr_LList *		src
					));

#define				jr_LListDup(src) \
					jr_LListCopy (jr_LListCreate (jr_LListElementSize (src)), src)

extern void *		jr_LListFindElementPtr  PROTO ((
						jr_LList *			list,
						void *				obj,
						jr_int				(*cmpfn)()
					));

extern jr_int		jr_LListCmp				PROTO ((
						jr_LList *			list1,
						jr_LList *			list2,
						jr_int				(*cmpfn)()
					));

extern jr_int		jr_LListHash			PROTO ((jr_LList *list, jr_int (*hashfn)()));



extern void *		jr_LListFindAppendPtr		PROTO ((
							jr_LList *			list,
							void *				obj,
							jr_int				(*cmpfn) ()
					));

extern void  *		jr_LListNewElementPtrInOrderWithSizeAndUniqueFlag  PROTO ((
						jr_LList *			list,
						void *				obj,
						jr_int				(* cmpfn)(),
						jr_int				size,
						jr_int				unique
					));

#define				jr_LListNewElementPtrInOrder(list, obj, cmpfn)				\
					jr_LListNewElementPtrInOrderWithSizeAndUniqueFlag (			\
						list, obj, cmpfn, jr_LListElementSize (list), 0			\
					)

#define				jr_LListNewElementPtrInOrderWithSize(list, obj, cmpfn, size)	\
					jr_LListNewElementPtrInOrderWithSizeAndUniqueFlag (list, obj, cmpfn, size, 0)

#define				jr_LListNewElementPtrUniquely(list, obj, cmpfn)				\
					jr_LListNewElementPtrInOrderWithSizeAndUniqueFlag (			\
						list, obj, cmpfn, jr_LListElementSize (list), 1			\
					)

extern void *		jr_LListSetNewElementUniquely			PROTO ((
						jr_LList *		list,
						void *			obj,
						jr_int			(*cmpfn) ()
					));

#define				jr_LListNewElementPtrUniquelyWithSize(list, obj, cmpfn, size)	\
					jr_LListNewElementPtrInOrderWithSizeAndUniqueFlag (list, obj, cmpfn, size, 1)


#define				jr_LListSetNewElementInOrder(list, obj, cmpfn)				\
					memcpy (													\
						jr_LListNewElementPtrInOrder (list, obj, cmpfn), obj, 	\
						jr_LListElementSize (list)								\
					)

extern void			jr_LListMergeWithUniqueFlag  PROTO ((
						jr_LList *list1, jr_LList *list2, jr_int (*cmpfn)(), jr_int unique
					));

#define				jr_LListMerge(l1, l2, cmpfn)	\
					jr_LListMergeWithUniqueFlag (l1, l2, cmpfn, 0)

#define				jr_LListMergeUniquely(l1, l2, cmpfn)	\
					jr_LListMergeWithUniqueFlag (l1, l2, cmpfn, 1)


/***** Native List Macros *****/

#define jr_LListNativeSetNewTail(list, v, type) \
		(* (type *) jr_LListNewTailPtr(list) = v)

#define jr_LListNativeSetNewHead(list, v, type) \
		(* (type *) jr_LListNewHeadPtr(list) = v)

#define jr_LListNativeTail(list, type)			(* (type *) jr_LListTailPtr (list))
#define jr_LListNativeHead(list, type)			(* (type *) jr_LListHeadPtr (list))


#define	jr_LListNextElementPtr(list, objptr)				\
		jr_NextLListElementPtrWithPrefix (objptr, jr_LListPrefixSize (list))


#define jr_LListForEachElementPtr(list, current) 	\
		jr_LListForEachElementPtrWithPrefix (list, current, jr_LListPrefixSize (list))


#endif

/*
.EXIT
Out of Date - preserved for historical reasons.
This library provides several general list manipulation functions.
The object is copied into the list, the objects must be referenced
through pointers.

	LinkedListInit (objectsize, fn) : takes size of the object to be stored
		in the list and the object comparison function and returns
		a pointer to a list.
	AddToTail (list, obj) : Creates new element at the tail and copies
		object into it.
	AddToHead (list, obj) : Creates new element at the head and copies
		object into it.
	AddToSort (list, obj) : Creates new element and inserts in sorted
		(lower to higher) order according to the list cmp function.
	SortLinkedList (list) : sorts the list.
	IsInLinkedList (list, obj) : 0 if object not in list, returns object
		of element found.
	DeleteObj (list, obj) : If object is in the list, deletes it.
	EmptyLinkedList (list) : frees up all the elements of the list.
	FreeLinkedList (list) : frees up the elements and the list.
	HeadOf(list) : (define) returns ptr to object of head of list.
	TailOf(list) : (define) returns ptr to object of tail of list.
	ObjectOf(elementptr) : (define) returns object of elementptr.
	IsLinkedListEmpty(list) : (define) true if list is empty.
	ForEachLinkedListElement(list, elptr) : (define) is a standard for-loop.
*/
