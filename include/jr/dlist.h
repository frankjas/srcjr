#ifndef _dlisth
#define _dlisth	1

#include "ezport.h"

#include <string.h>
#include <stdarg.h>

typedef struct {
	void *		tail;
	void *		head;
	jr_int		objsize;
	jr_int		prefix_size;

	unsigned	allocates_elements		: 1;
} jr_DList;

extern void			jr_DListInit			PROTO ((jr_DList *list, jr_int objsize));
extern jr_DList *	jr_DListCreate			PROTO ((jr_int objsize));

extern void			jr_DListDestroy			PROTO ((jr_DList *list));
extern void			jr_DListUndo			PROTO ((jr_DList *list));
extern void			jr_DListEmpty			PROTO ((jr_DList *list));

extern void *		jr_DListInsertElement	PROTO ((
												jr_DList *		list,
												const void *	next_el,
												const void *	new_el
											));

extern void *		jr_DListAppendElement	PROTO ((
												jr_DList *		list,
												const void *	prev_el,
												const void *	new_el
											));

extern void *		jr_DListExtractElement	PROTO ((jr_DList *list, const void *el)) ;

extern void			jr_DListDeleteElement	PROTO ((jr_DList *list, void *el));



extern jr_int		jr_DListElementPrefixSize	PROTO ((jr_DList *list));
extern void			jr_DListSetPrefixSize		PROTO ((jr_DList *list, jr_int prefix_size));


extern void			jr_DListTransformPrefix		PROTO ((
						jr_DList *				list,
						void *					el_ptr,
						unsigned jr_int			magnitude,
						jr_int					which_way,
						jr_int					all_elements
					));

extern void			jr_DListSetContainers		PROTO ((jr_DList *list, ...));

extern void			jr_DListSetContainersFromVA_List	PROTO ((
						jr_DList *				list,
						va_list					arg_list
					));


#define				jr_DListHeadPtr(list)			((list)->head)
#define				jr_DListTailPtr(list)			((list)->tail)
#define				jr_DListIsEmpty(list)			((list)->head == 0)
#define				jr_DListElementSize(list)		((list)->objsize)
#define				jr_DListPrefixSize(list)		((list)->prefix_size)

#define				jr_DListAllocatesElements(list)		((list)->allocates_elements)
#define				jr_DListSetAllocatesElements(list)	((list)->allocates_elements = 1)

#define				jr_DListBytesPerElement(list)	\
					(jr_DListElementSize (list)  +  jr_DListPrefixSize (list))

#define				jr_DListHasMultipleElements(list)			\
					(!jr_DListIsEmpty (list)  &&  jr_DListHeadPtr (list) != jr_DListTailPtr (list))


/******** Element Operations ********/

extern void *		jr_DListAllocateElementWithSize		PROTO ((jr_DList *list, jr_int size));


extern void *		jr_DListAllocateTmpElementWithSize	PROTO ((jr_DList *list, jr_int size));
					/*
					 * Used to allocate an element with particular values
					 * without adding it to the list. Used for creating
					 * temporaries lookup elements, where the lookup uses
					 * list next,prev pointer info, for example, jr_Graph
					 * edge lookups
					 */

extern void			jr_DListFreeElement		PROTO ((
						jr_DList *				list,
						void *					obj_ptr
					));



#define				jr_FreeDListElementWithPrefix(obj_ptr, prefix_size) \
					jr_free ((char *) (obj_ptr) - 2 * sizeof (void *) - (prefix_size));

#define				jr_NextDListElementPtrWithPrefix(objptr, prefix_size)			\
					(((void **) ((char *) (objptr) - (prefix_size)))[-1])

#define				jr_PrevDListElementPtrWithPrefix(objptr, prefix_size)			\
					(((void **) ((char *) (objptr) - (prefix_size)))[-2])


extern void			jr_MakeNullDListElementWithPrefix	PROTO ((const void *el, jr_int prefix_size));
extern jr_int		jr_IsNullDListElementWithPrefix		PROTO ((const void *el, jr_int prefix_size));
					/*
					** 8/15/2005: A null element doesn't mean the element isn't on a list
					** since the only element on a list will have null prev and next pointers.
					*/

/******* ForEach Loops ********/

#define		jr_DListForEachElementPtrWithPrefix(list, current, prefix_size)				\
			for (	(current) = jr_DListHeadPtr (list);									\
					(current) != 0;														\
					(current) = jr_NextDListElementPtrWithPrefix(current, prefix_size)	\
			)

#define		jr_DListForEachElementPtrRevWithPrefix(list, current, prefix_size)			\
			for (	(current) = jr_DListTailPtr (list);									\
					(current) != 0;														\
					(current) = jr_PrevDListElementPtrWithPrefix(current, prefix_size)	\
			)

#if defined(__cplusplus)
#	undef jr_DListForEachElementPtrWithPrefix
#	undef jr_DListForEachElementPtrRevWithPrefix


#	define	jr_DListForEachElementPtrWithPrefix(list, current, prefix_size)				\
			for (	(*(void **)&(current)) = jr_DListHeadPtr (list);					\
					(current) != 0 ;													\
					(*(void **)&(current)) = jr_NextDListElementPtrWithPrefix (current, prefix_size)\
				)

#	define	jr_DListForEachElementPtrRevWithPrefix(list, current, prefix_size)			\
			for (	(*(void **)&(current)) = jr_DListTailPtr (list);					\
					(current) != 0 ;													\
					(*(void **)&(current)) = jr_DListElementPrevPtrWithPrefix (current, prefix_size)\
				)


#endif


#define		jr_DListForEachElementPtr(list, current)				\
			jr_DListForEachElementPtrWithPrefix (list, current, jr_DListPrefixSize (list))

#define		jr_DListForEachElementPtrRev(list, current)				\
			jr_DListForEachElementPtrRevWithPrefix (list, current, jr_DListPrefixSize (list))


/****** Convenience Functions ******/

#define				jr_DListAllocateElement(list)	\
					jr_DListAllocateElementWithSize (list, jr_DListElementSize (list))


#define				jr_DListAllocateTmpElement(list)	\
					jr_DListAllocateTmpElementWithSize (list, jr_DListElementSize (list))


#define				jr_FreeDListElement(obj_ptr) \
					jr_FreeDListElementWithPrefix (obj_ptr, 0)

#define				jr_DListNextElementPtr(list, objptr)	\
					jr_NextDListElementPtrWithPrefix (objptr, jr_DListPrefixSize (list))

#define				jr_DListPrevElementPtr(list, objptr)	\
					jr_PrevDListElementPtrWithPrefix (objptr, jr_DListPrefixSize (list))

#define				jr_DListMakeNullElement(list, objptr)			\
					jr_MakeNullDListElementWithPrefix (objptr, jr_DListPrefixSize (list))

#define				jr_DListIsNullElement(list, objptr)			\
					jr_IsNullDListElementWithPrefix (objptr, jr_DListPrefixSize (list))
					/*
					** 8/15/2005: A null element doesn't mean the element isn't on a list
					** since the only element on a list will have null prev and next pointers.
					*/

#define				jr_DListNewPrevElementPtrWithSize(list, next, size)	\
					jr_DListInsertElement (list, next, jr_DListAllocateElementWithSize (list, size))

#define				jr_DListNewNextElementPtrWithSize(list, next, size)	\
					jr_DListAppendElement (list, next, jr_DListAllocateElementWithSize (list, size))



#define				jr_DListNewHeadPtrWithSize(list, s)				\
					jr_DListNewNextElementPtrWithSize(list, 0, s)

#define				jr_DListNewTailPtrWithSize(list, s)				\
					jr_DListNewPrevElementPtrWithSize(list, 0, s)

#define				jr_DListNewHeadPtr(list)						\
					jr_DListNewHeadPtrWithSize (list, jr_DListElementSize (list))

#define				jr_DListNewTailPtr(list)						\
					jr_DListNewTailPtrWithSize (list, jr_DListElementSize (list))

#define				jr_DListNewPrevElementPtr(list, where)	\
					jr_DListNewPrevElementPtrWithSize(list, where, jr_DListElementSize (list))

#define				jr_DListNewNextElementPtr(list, where)	\
					jr_DListNewNextElementPtrWithSize(list, where, jr_DListElementSize (list))

#define				jr_DListSetNewHeadWithSize(list, obj, s)							\
					memcpy (															\
						jr_DListNewHeadPtrWithSize(list, s), obj, s						\
					)

#define				jr_DListSetNewTailWithSize(list, obj, s)							\
					memcpy (															\
						jr_DListNewTailPtrWithSize(list, s), obj, s						\
					)

#define				jr_DListSetNewPrevElementWithSize(list, where, obj, s)						\
					memcpy (															\
						jr_DListNewPrevElementPtrWithSize(list, where, s), obj, s		\
					)

#define				jr_DListSetNewNextElementWithSize(list, where, obj, s)						\
					memcpy (															\
						jr_DListNewNextElementPtrWithSize(list, where, s), obj, s		\
					)


#define				jr_DListSetNewHead(list, obj)	\
					jr_DListSetNewHeadWithSize (list, obj, jr_DListElementSize (list))

#define				jr_DListSetNewTail(list, obj)	\
					jr_DListSetNewTailWithSize (list, obj, jr_DListElementSize (list))

#define				jr_DListSetNewPrevElement(list, where, obj)	\
					jr_DListSetNewPrevElementWithSize (list, where, obj, jr_DListElementSize (list))

#define				jr_DListSetNewNextElement(list, where, obj)	\
					jr_DListSetNewNextElementWithSize (list, where, obj, jr_DListElementSize (list))



#define				jr_DListInsertToHead(list, prev)						\
					jr_DListAppendElement (list, 0, prev)

#define				jr_DListAppendToTail(list, next)						\
					jr_DListAppendElement (list, jr_DListTailPtr (list), next)



#define				jr_DListDeleteHead(list) \
					jr_DListDeleteElement (list, jr_DListHeadPtr (list))

#define				jr_DListDeleteTail(list) \
					jr_DListDeleteElement (list, jr_DListTailPtr (list))


extern void *		jr_DListNewElementPtrInOrderWithSizeAndUnique		PROTO ((
								jr_DList *list,
								void *obj,
								jr_int (*cmpfn) (),
								jr_int size,
								jr_int unique
					));

#define				jr_DListNewElementPtrInOrder(list, obj, cmpfn)	\
					jr_DListNewElementPtrInOrderWithSizeAndUnique (	\
						list, obj, cmpfn, jr_DListElementSize (list), 0		\
					)

#define				jr_DListNewElementPtrInOrderWithSize(list, obj, cmpfn, size)	\
					jr_DListNewElementPtrInOrderWithSizeAndUnique (	\
						list, obj, cmpfn, size, 0					\
					)

#define				jr_DListNewElementPtrUniquely(list, obj, cmpfn)	\
					jr_DListNewElementPtrInOrderWithSizeAndUnique (	\
						list, obj, cmpfn, jr_DListElementSize (list), 1		\
					)

#define				jr_DListNewElementPtrUniquelyWithSize(list, obj, cmpfn, size)	\
					jr_DListNewElementPtrInOrderWithSizeAndUnique (list, obj, cmpfn, size, 1)



extern jr_int		jr_DListCmp  PROTO ((
						jr_DList *			list1,
						jr_DList *			list2,
						jr_int				(*cmpfn)()
					));

extern jr_int		jr_DListHash  PROTO ((jr_DList *list, jr_int (*hashfn)()));

extern void *		jr_DListFindInsertPtr	PROTO ((
						jr_DList *			list,
						void *				obj,
						jr_int				(*cmpfn)()
					));

extern void *		jr_DListFindElementPtr	PROTO ((
						jr_DList *			list,
						void *				obj,
						jr_int				(*cmpfn)()
					));


/******** Native jr_DList Macros ********/

#define				jr_DListNativeSetNewHead(list, element, type) \
					(* (type *) jr_DListNewHeadPtr (list) = element)

#define				jr_DListNativeSetNewTail(list, element, type) \
					(* (type *) jr_DListNewTailPtr (list) = element)

#define				jr_DListNativeHead(list, type)				(* (type *) jr_DListHeadPtr (list))
#define				jr_DListNativeTail(list, type)				(* (type *) jr_DListTailPtr (list))

		


#endif
