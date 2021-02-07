#ifndef __ALIST_HEADER__
#define __ALIST_HEADER__

#include "ezport.h"

#include <string.h>
#include <stdarg.h>


typedef struct {
	char *	buffer;					/* buffer where elements are stored		*/
	jr_int	buffer_capacity;		/* maximum number of objects allowed	*/
	char *	free_ptr;
	jr_int	objsize;
	jr_int	free_index;

	jr_int	prefix_size;
} jr_AList;


typedef jr_AList jr_AListType;


/*
 * To reduce the size of the jr_AList,
 * we can instead allocate a separate structure to hold the
 * prefix transformation code, and keep a pointer to that structure.
 * The we only allocate that structure when the prefix is non-zero.
 * But this means all HTable element accesses are more expensive.
 *
 * We could actually store the prefix_size in the structure, and if
 * the prefix size is non-zero, than an extra 8 bytes is added to
 * the buffer (as a prefix, perhaps), that hold the prefix transformation
 * information.
 */

typedef struct {
	void	(*prefix_transform_fn) ();
	void *	prefix_transform_arg;
} jr_AListPrefixTransformStruct;


extern void		jr_AListInit     				PROTO ((jr_AList *list, jr_int objsize));
extern void		jr_AListUndo     				PROTO ((jr_AList *list));
extern jr_AList *jr_AListCreate   				PROTO ((jr_int objsize));
extern void		jr_AListDestroy  				PROTO ((jr_AList *list));

extern void		jr_AListEmpty    				PROTO ((jr_AList *list));
extern void		jr_AListInitCopy  				PROTO ((jr_AList *dest, jr_AList *src));
extern void *	jr_AListExtractBuffer			PROTO ((jr_AList *list));
extern void		jr_AListTruncate				PROTO ((jr_AList *list, jr_int i));
extern void		jr_AListReverse					PROTO ((jr_AList *list));

#define			jr_AListMove(src, dest)			memcpy (dest, src, sizeof (jr_AList));

extern jr_int	jr_AListIncreaseCapacity		PROTO ((jr_AList *list, jr_int new_size));

extern void *	jr_AListNewTailPtr     			PROTO ((jr_AList *list));
extern void *	jr_AListDeletedTailPtr			PROTO ((jr_AList *list));

extern void *	jr_AListNewPrevElementPtr		PROTO ((jr_AList *list, void *insertptr));
extern void *	jr_AListNewNextElementPtr		PROTO ((jr_AList *list, void *insertptr));

extern void		jr_AListSetTailIndex       		PROTO ((jr_AList *list, jr_int index));

extern jr_AList *jr_AListDup  					PROTO ((jr_AList *list));
extern void *	jr_AListBufferDup				PROTO ((jr_AList *list));

extern void		jr_AListDeleteRangeFromPtr		PROTO ((
					jr_AList *					list,
					void *						ptr,
					jr_int						num_entries
				));

extern void **	jr_AListVectorPtr				PROTO ((jr_AList *list));



extern void		jr_AListSetPrefixSize			PROTO ((
					jr_AList *					list,
					jr_int						prefix_size,
					void						(*prefix_transform_fn) (),
					void *						prefix_transform_arg
				));

				/*
				 
				void prefix_transform_fn		PROTO ((
					void *						container_ptr,
					void *						el_ptr,
					unsigned jr_int				magnitude,
					jr_int						which_way,
					jr_int						all_elements
				))
				*/

extern void		jr_AListSetContainers			PROTO ((
					jr_AList *					list,
					...
				));
				/*
				 * jr_AListSetContainers (list, "jr_LList", llist_ptr, "jr_GNode", graph_ptr, 0);
				 * also accepts    "MyContainerType", container_ptr
				 * use jr_AListSetCustomPrefixInfo (list, "MyContainerType", etc.)
				 * to set the container's prefix size and prefix transform function
				 */

extern void		jr_AListSetCustomPrefixInfo		PROTO ((
					jr_AList *					list,
					char *						container_name,
					jr_int						prefix_size,
					void						(*prefix_transform_fn) ()
				));

extern void		jr_AListSetContainersFromVA_List	PROTO ((
					jr_AList *					list,
					va_list						arg_list
				));



#define			jr_AListSize(list)				((list)->free_index)
#define			jr_AListEndPtr(list)			((void *) ((list)->free_ptr))
#define			jr_AListElementSize(list)		((list)->objsize - (list)->prefix_size)
#define			jr_AListPrefixSize(list)		((list)->prefix_size)

#define			jr_AListTailIndex(list)			((list)->free_index - 1)
#define			jr_AListNumBytes(list)			((char *) (list)->free_ptr - (char *) (list)->buffer)
#define			jr_AListCapacity(list)			((list)->buffer_capacity)
#define			jr_AListIsEmpty(list)			((list)->free_index == 0)

#define			jr_AListAllocatesElements(list)	(1)


#define			jr_AListHasPrefixTransformFn(list)				\
				(		jr_AListPrefixSize (list)  >  0			\
					&&  (list)->buffer != 0 					\
					&&	jr_AListPrefixTransformFn (list)		\
				)

#define			jr_AListPrefixTransformPtr(list)								\
				((jr_AListPrefixTransformStruct *)								\
					((list)->buffer - sizeof (jr_AListPrefixTransformStruct))	\
				)

#define			jr_AListPrefixTransformFn(list)		\
				(jr_AListPrefixTransformPtr (list)->prefix_transform_fn)

#define			jr_AListPrefixTransformArg(list)		\
				(jr_AListPrefixTransformPtr (list)->prefix_transform_arg)


#define			jr_AListElementPtrWithPrefix(list, i, prefix_size)		\
				((void *)((list)->buffer + (i)*(list)->objsize + (prefix_size)))

#define			jr_AListTailPtrWithPrefix(list, prefix_size)			\
				((void *)((list)->free_ptr - (list)->objsize + (prefix_size)))

#define			jr_AListHeadPtrWithPrefix(list, prefix_size)			\
				((void *)((list)->buffer + (prefix_size)))


#define			jr_AListElementIndexWithPrefix(list, ptr, prefix_size) \
				( (((char *) (ptr) - (prefix_size)) - (list)->buffer)  /  (list)->objsize)

#define			jr_AListIncrElementPtr(list, elptr) \
				((void *)(((char *) (elptr)) + (list)->objsize))

#define			jr_AListDecrElementPtr(list, elptr) \
				((void *)(((char *) (elptr)) - (list)->objsize))


#define			jr_AListNextElementPtr(list, elptr)									\
				(jr_AListIncrElementPtr(list, elptr)  <  jr_AListEndPtr (list)		\
					? jr_AListIncrElementPtr(list, elptr)							\
					: 0																\
				)

#define			jr_AListPrevElementPtr(list, elptr)									\
				((void *)(elptr) > jr_AListHeadPtr (list)									\
					? jr_AListDecrElementPtr(list, elptr)							\
					: 0																\
				)


#define			jr_TransformAListPtr(ptr, magnitude, which_way)		\
				((void *) ((which_way) > 0							\
					? ((char *) (ptr)) + (magnitude)				\
					: ((char *) (ptr)) - (magnitude))				\
				)

#define			jr_UnTransformAListPtr(ptr, magnitude, which_way)	\
				jr_TransformAListPtr (ptr, magnitude, -(which_way))




/******** Convenience Functions ********/


#define			jr_AListNewElementPtr(list, i)			\
				jr_AListNewPrevElementPtr (list, jr_AListElementPtr(list, i))

#define			jr_AListElementPtr(list, i)				\
				jr_AListElementPtrWithPrefix (list, i, jr_AListPrefixSize (list))

#define			jr_AListTailPtr(list)					\
				jr_AListTailPtrWithPrefix (list, jr_AListPrefixSize (list))

#define			jr_AListHeadPtr(list)					\
				jr_AListHeadPtrWithPrefix (list, jr_AListPrefixSize (list))


#define			jr_AListElementIndex(list, ptr) \
				jr_AListElementIndexWithPrefix (list, ptr, jr_AListPrefixSize (list))


#define			jr_AListForEachElementPtr(list, current) \
				jr_AListForEachElementPtrWithPrefix (list, current, jr_AListPrefixSize (list))

#define			jr_AListForEachElementPtrRev(list, current)\
				jr_AListForEachElementPtrRevWithPrefix (list, current, jr_AListPrefixSize (list))



#define			jr_AListSetSize(list, size)		jr_AListSetTailIndex (list, (size) - 1)



#define			jr_AListSetNewTail(list, elptr)											\
				memcpy (jr_AListNewTailPtr(list), elptr, jr_AListElementSize (list))

#define			jr_AListSetNewElement(list, i, elptr)									\
				memcpy (																\
					jr_AListNewPrevElementPtr(list, jr_AListElementPtr(list, i)),		\
					elptr,																\
					jr_AListElementSize (list)											\
				)

#define			jr_AListSetNewPrevElement(list, next_elptr, elptr)						\
				memcpy (																\
					jr_AListNewPrevElementPtr(list, next_elptr),						\
					elptr,																\
					jr_AListElementSize (list)											\
				)

#define			jr_AListSetNewNextElement(list, prev_elptr, elptr)						\
				memcpy (																\
					jr_AListNewNextElementPtr(list, prev_elptr),						\
					elptr,																\
					jr_AListElementSize (list)											\
				)

#define			jr_AListSetElement(list, i, elptr)										\
				memcpy (																\
					jr_AListElementPtr(list, i),										\
					elptr,																\
					jr_AListElementSize (list)											\
				)

#define			jr_AListGetTail(list, elptr)											\
				memcpy (elptr, jr_AListTailPtr(list), jr_AListElementSize (list))

#define			jr_AListGetHead(list, elptr)											\
				memcpy (elptr, jr_AListHeadPtr(list), jr_AListElementSize (list))

#define			jr_AListGetElement(list, i, elptr)										\
				memcpy (elptr, jr_AListElementPtr(list, i), jr_AListElementSize (list))


#define			jr_AListDeleteTail(list)		\
				((void) jr_AListDeletedTailPtr (list))

#define			jr_AListGetDeletedTail(list, elptr) \
				memcpy (elptr, jr_AListDeletedTailPtr(list), jr_AListElementSize (list))

#define			jr_AListDeleteElement(list, elptr) \
				jr_AListDeleteRangeFromPtr (list, elptr, 1)

#define			jr_AListDeleteIndex(list, i) \
				jr_AListDeleteRangeFromPtr (list, jr_AListElementPtr (list, i), 1)

#define			jr_AListDeleteRangeFromIndex(list, i, num_to_delete) \
				jr_AListDeleteRangeFromPtr (list, jr_AListElementPtr (list, i), num_to_delete)


extern void		jr_AListAppendString(
					jr_AList *					char_list,
					const char *				string);

extern void *	jr_AListFindInsertPtr			PROTO ((
					jr_AList *					list,
					const void *				elptr,
					jr_int						(*cmpfn)()
				));

extern void *	jr_AListFindElementPtr			PROTO ((
					jr_AList *					list,
					const void *				elptr,
					jr_int						(*cmpfn)()
				));

extern void *	jr_AListFindFirstElementPtr		PROTO ((
					jr_AList *					list,
					const void *				elptr,
					jr_int						(*cmpfn)()
				));

extern void *	jr_AListSetNewTailUniquely		PROTO ((
					jr_AList *					list,
					const void *				elptr,
					jr_int						(*cmpfn)()
				));

extern void  	jr_AListSortRange				PROTO ((
					jr_AList *					list,
					jr_int						start_index,
					jr_int						num_elements,
					jr_int						(*cmpfn)()
				));

extern jr_int	jr_AListCmp						PROTO ((
					const jr_AList *			list1,
					const jr_AList *			list2,
					jr_int						(*cmpfn)()
				));

extern jr_int	jr_AListHash					PROTO ((
					const jr_AList *			list,
					jr_int						(*hash_fn) ()
				));

#define			jr_AListNewElementPtrInOrder(list, elptr, cmpfn)					\
				jr_AListNewPrevElementPtr (list, jr_AListFindInsertPtr (list, elptr, cmpfn))
			
#define			jr_AListSetNewElementInOrder(list, elptr, cmpfn)					\
				memcpy (															\
					jr_AListNewElementPtrInOrder (list, elptr, cmpfn),				\
					elptr,															\
					jr_AListElementSize (list)										\
				)


#define			jr_AListQSort(list, cmpfn)											\
				qsort (																\
					jr_AListElementPtr(list, 0), jr_AListSize(list),				\
					jr_AListElementSize(list), cmpfn								\
				)


#define			jr_AListSort(list, cmpfn)											\
				jr_AListSortRange (list, 0, jr_AListSize (list), cmpfn)

#define			jr_AListSortUniquely(list, cmpfn)									\
				jr_AListSortUniquelyRange (list, 0, jr_AListSize (list), cmpfn)

#define			jr_AListSortUniquelyRange(list, start_index, num_elements, cmpfn)	\
				jr_AListSortUniquelyRangeFreeingDeletes (							\
					list, start_index, num_elements, cmpfn, 0, 0					\
				)

#define			jr_AListSortUniquelyFreeingDeletes(list, cmpfn, freefunc, arg) 		\
				jr_AListSortUniquelyRangeFreeingDeletes (							\
					list, 0, jr_AListSize (list), cmpfn, freefunc, arg				\
				)


extern void		jr_AListSortUniquelyRangeFreeingDeletes		PROTO ((
					jr_AList *								list,
					jr_int									start_index,
					jr_int									num_elements,
					jr_int									(*cmpfn)(),
					void									(*freefunc)(),
					void *									freefunc_arg
				));

/******** Native jr_AList Macros ********/

#define			jr_AListNativeSetNewTail(list, element, type) \
				(* (type *) jr_AListNewTailPtr (list) = element)

#define			jr_AListNativeSetNewElement(list, i, element, type) \
				(* (type *) jr_AListNewPrevElementPtr (list, jr_AListElementPtr (list, i)) = element)
		

#define			jr_AListNativeDeletedTail(list, type)   	(* (type *) jr_AListDeletedTailPtr (list))
#define			jr_AListNativeTail(list, type)				(* (type *) jr_AListTailPtr (list))
#define			jr_AListNativeHead(list, type)				(* (type *) jr_AListHeadPtr (list))
#define			jr_AListNativeElement(list, i, type)		(* (type *) jr_AListElementPtr (list, i))
#define			jr_AListNativeSetTail(list, v, type)		(* (type *) jr_AListTailPtr (list) = (v))
#define			jr_AListNativeSetHead(list, v, type)		(* (type *) jr_AListHeadPtr (list) = (v))

#define			jr_AListNativeSetElement(list, i, v, type)	\
				(* (type *) jr_AListElementPtr (list, i) = (v))


#define			jr_AListNativeGetDeletedTail(list, type)	jr_AListNativeDeletedTail(list, type)
		

/******** For Each Loop Macros ********/

#define			jr_AListIsValidIndex(list, i)				((i) >= 0  &&  (i) < jr_AListSize (list))


#define			jr_AListForEachElementIndex(list, i) \
				for ((i) = 0; (i) < jr_AListSize(list); (i)++)


#define			jr_AListForEachElementIndexRev(list, i) \
				for ((i) = jr_AListSize(list) - 1; i >= 0; i--)


#define			jr_AListForEachElementPtrWithPrefix(list, current, prefix_size) 				\
				for (	(current) = jr_AListHeadPtrWithPrefix (list, prefix_size);				\
						((void *) (current)) < jr_AListEndPtr (list);							\
						(current) = jr_AListIncrElementPtr (list, current)						\
					)
				/*
				** 9/5/05: if the object size if 0 and the prefix is > 0, then using
				** "< end ptr" as the test will skip the last element,
				** but using "< tail ptr" will screw up on an empty list, since you
				** can't subtract from a null pointer.
				*/

#define			jr_AListForEachElementPtrRevWithPrefix(list, current, prefix_size)				\
				if (jr_AListSize (list)  >  0)													\
				for (	(current) = jr_AListTailPtrWithPrefix (list, prefix_size);				\
						((void *) (current)) >= jr_AListHeadPtr (list);							\
						(current) = jr_AListDecrElementPtr (list, current)						\
					)



#if defined(__cplusplus)
				/*
				 * void * assignment generates warnings in C++, but
				 * by casting the address of the loop pointer to a void **,
				 * we create a void * expression on the left of the assignment.
				 * These operations do not actually execute at run time.
				 */

#	undef		jr_AListForEachElementPtrWithPrefix
#	undef		jr_AListForEachElementPtrRevWithPrefix

#	define		jr_AListForEachElementPtrWithPrefix(list, current, prefix_size)					\
				for (	(*(void **)&(current)) = jr_AListHeadPtrWithPrefix (list, prefix_size);	\
						( (void *)  (current)) < jr_AListEndPtr (list);							\
						(*(void **)&(current)) = jr_AListIncrElementPtr (list, current)			\
					)
				/*
				** 9/5/05: if the object size if 0 and the prefix is > 0, then using
				** "< end ptr" as the test will skip the last element,
				** but using "< tail ptr" will screw up on an empty list, since you
				** can't subtract from a null pointer.
				*/

#	define		jr_AListForEachElementPtrRevWithPrefix(list, current, prefix_size)				\
				if (jr_AListSize (list)  >  0)													\
				for (	(*(void **)&(current)) =  jr_AListTailPtrWithPrefix (list, prefix_size);\
						( (void *)  (current)) >= jr_AListHeadWithPrefixPtr (list, prefix_size);\
						(*(void **)&(current)) =  jr_AListDecrElementPtr (list, current)		\
					)

#endif


#endif
