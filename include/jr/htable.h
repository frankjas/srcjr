#ifndef _htableh
#define _htableh

#include "ezport.h"

#include <stdio.h>
#include <stdarg.h>

#include "jr/alist.h"


typedef struct {
	jr_int	hash_value;				/* makes rehashing easier		*/
	jr_int	next_index;				/* next entry element			*/
} jr_HTableElementStruct;


typedef struct {
	jr_int		element_size;

	jr_int		(*hashfn)	PROTO ((const void *));
	jr_int		(*cmpfn)	PROTO ((const void *, const void *, ...));
									/* -1, 0, 1, like strcmp			*/
	void *		cmp_arg;

	jr_int *	buckets_array;
	jr_int		num_buckets;		/* number of hash buckets			*/

	jr_AList	entry_array[1];

	jr_int		delete_list; 		/* for deleted entries				*/
	jr_int		num_deletes;
} jr_HTable;



extern jr_HTable *		jr_HTableCreate			PROTO ((
							jr_int				key_size,
							jr_int				(*hashfn) (const void *),
							jr_int				(*cmpfn) (const void *, const void *)
						));

extern void				jr_HTableInit			PROTO ((
							jr_HTable *			htable,
							jr_int				key_size,
							jr_int				(*hashfn) (const void *),
							jr_int				(*cmpfn) (const void *, const void *)
						));

extern void				jr_HTableDestroy		PROTO ((jr_HTable *htable));
extern void				jr_HTableUndo			PROTO ((jr_HTable *htable));

extern void				jr_HTableEmpty			PROTO ((jr_HTable *htable));

extern void				jr_HTableIncreaseCapacity		PROTO ((
							jr_HTable *					htable,
							jr_int						new_capacity
						));

extern void				jr_HTableSetPrefixSize	PROTO ((
							jr_HTable *			htable,
							jr_int				prefix_size,
							void				(*prefix_transform_fn) (),
							void *				prefix_transform_arg
						));

extern void				jr_HTableSetContainers	PROTO ((
							jr_HTable *			htable,
							...
						));


extern void				jr_HTableSetContainersFromVA_List	PROTO ((
							jr_HTable *				htable,
							va_list					arg_list
						));


extern jr_int		jr_HTableFindIndexGeneric	PROTO ((
						jr_HTable *				htable,
						const void *			key_ptr,
						jr_int					is_ptr_value
					));

extern jr_int		jr_HTableNewIndexGeneric	PROTO ((
						jr_HTable *				htable,
						const void *			key_ptr,
						jr_int					is_ptr_value
					));

extern void			jr_HTableDeleteIndex		PROTO ((
						jr_HTable *				htable,
						jr_int					index
					));


#define				jr_HTableFindElementIndex(htable, ptr_value)		\
					jr_HTableFindIndexGeneric (htable, ptr_value, 0)

#define				jr_HTableNewElementIndex(htable, ptr_value)			\
					jr_HTableNewIndexGeneric (htable, ptr_value, 0)

#define				jr_HTableFindPtrValueIndex(htable, ptr_value)		\
					jr_HTableFindIndexGeneric (htable, ptr_value, 1)



extern void			jr_HTableHashDist			PROTO ((
						jr_HTable *				htable,
						FILE *					fp,
						char *					s
					));

#define				jr_HTableElementSize(htable)			\
					((htable)->element_size)

#define				jr_HTableSize(htable)					\
					(jr_AListSize ((htable)->entry_array) - (htable)->num_deletes)

#define				jr_HTableIsEmpty(htable)				\
					(jr_HTableSize (htable)  ==  0)

#define				jr_HTableNumBytes(htable)				\
					jr_AListNumBytes ((htable)->entry_array)

#define				jr_HTablePrefixSize(htable)				\
					jr_AListPrefixSize ((htable)->entry_array)

#define				jr_HTableAllocatesElements(htable)		\
					jr_AListAllocatesElements ((htable)->entry_array)

#define				jr_HTableSetCmpFnArg(htable, v)			\
					((htable)->cmp_arg = (v))

#define				jr_HTableForEachElementIndexWithPrefix(htable, i, prefix_size)				\
					jr_AListForEachElementIndex ((htable)->entry_array, i)						\
						if (jr_HTableHashValueWithPrefix(htable, i, prefix_size) != -1)

#define				jr_HTableForEachElementPtrWithPrefix(htable, el_ptr, prefix_size)			\
					jr_AListForEachElementPtrWithPrefix (										\
						(htable)->entry_array, el_ptr, prefix_size								\
					) if (((jr_HTableElementStruct *) 											\
							((char *) (el_ptr) - (prefix_size))									\
						)->hash_value != -1)



/******** Element Macros ********/

#define				jr_HTableElementInfoPtrWithPrefix(htable, i, prefix_size)					\
					((jr_HTableElementStruct *)													\
						jr_AListElementPtrWithPrefix ((htable)->entry_array, i, 0)				\
					)
					/*
					 * The above has to use 0, since the jr_HTableElementStruct is in the prefix.
					 */

#define				jr_HTableElementPtrWithPrefix(htable, i, prefix_size)						\
					((void *) (jr_AListElementPtrWithPrefix ((htable)->entry_array, i, prefix_size)))

#define				jr_HTableElementIndexWithPrefix(htable, ptr, prefix_size)					\
					jr_AListElementIndexWithPrefix ((htable)->entry_array, ptr, prefix_size)


#define				jr_HTableHashValueWithPrefix(htable, i, prefix_size)						\
					(jr_HTableElementInfoPtrWithPrefix (htable, i, prefix_size)->hash_value)

#define				jr_HTableNextElementIndexWithPrefix(htable, i, prefix_size)					\
					(jr_HTableElementInfoPtrWithPrefix (htable, i, prefix_size)->next_index)

#define				jr_HTableIsValidIndexWithPrefix(htable, i, prefix_size)						\
					(		jr_AListIsValidIndex ((htable)->entry_array, i)						\
						&&	jr_HTableHashValueWithPrefix (htable, i, prefix_size) != -1			\
					)

#define				jr_HTableIsDeletedIndexWithPrefix(htable, i, prefix_size)					\
					(jr_HTableHashValueWithPrefix (htable, i, prefix_size) == -1)





/******** Convenience Functions ********/

#define				jr_HTableInitForPtrValues(htable, hashfn, cmpfn)							\
					jr_HTableInit (htable, 2 * sizeof (void *), hashfn, cmpfn)

#define				jr_HTableCreateForPtrValues(hashfn, cmpfn)									\
					jr_HTableCreate (2 * sizeof (void *), hashfn, cmpfn)


extern void *		jr_HTableFindPtrGeneric		PROTO ((
						jr_HTable *				htable,
						const void *			key_ptr,
						jr_int					is_ptr_value
					));

extern void *		jr_HTableNewPtrGeneric		PROTO ((
						jr_HTable *				htable,
						const void *			key_ptr,
						jr_int					is_ptr_value
					));


extern void *		jr_HTableDeleteElement		PROTO ((
						jr_HTable *				htable,
						void *					el_ptr
					));

extern void *		jr_HTableDeletePtrValue		PROTO ((
						jr_HTable *				htable,
						void *					el_ptr
					));


#define				jr_HTableFindElementPtr(htable, ptr_value)						\
					jr_HTableFindPtrGeneric (htable, ptr_value, 0)

#define				jr_HTableNewElementPtr(htable, ptr_value)						\
					jr_HTableNewPtrGeneric (htable, ptr_value, 0)


extern void *		jr_HTableSetNewElement			PROTO ((
						jr_HTable *					htable,
						const void *				key_ptr
					));

extern jr_int		jr_HTableSetNewElementIndex		PROTO ((
						jr_HTable *					htable,
						const void *				key_ptr
					));


#define				jr_HTableFindPtrValue(htable, ptr_value)						\
					jr_HTableFindPtrGeneric (htable, ptr_value, 1)

#define				jr_HTableNewPtrValueIndex(htable, ptr_value)					\
					jr_HTableNewIndexGeneric (htable, ptr_value, 1)

#define				jr_HTableNewPtrValue(htable, ptr_value)							\
					jr_HTableNewPtrGeneric (htable, ptr_value, 1)


#define				jr_HTableForEachElementIndex(htable, index)						\
					jr_HTableForEachElementIndexWithPrefix (						\
						htable, index, jr_HTablePrefixSize (htable)					\
					)

#define				jr_HTableForEachElementPtr(htable, key_ptr)						\
					jr_HTableForEachElementPtrWithPrefix (							\
						htable, key_ptr, jr_HTablePrefixSize (htable)				\
					)

#define				jr_HTableNativeElementWithPrefix(htable, i, type, prefix_size)			\
					(*(type *) jr_HTableElementPtrWithPrefix (htable, i, prefix_size))

#define				jr_HTableSetNativeElementWithPrefix(htable, i, v, type, prefix_size)	\
					(*(type *) jr_HTableElementPtrWithPrefix (htable, i, prefix_size) = (v))


#define				jr_HTableElementPtr(htable, i)									\
					jr_HTableElementPtrWithPrefix(htable, i, jr_HTablePrefixSize (htable))

#define				jr_HTableElementIndex(htable, ptr)								\
					jr_HTableElementIndexWithPrefix(htable, ptr, jr_HTablePrefixSize (htable))


#define				jr_HTableHashValue(htable, i)									\
					jr_HTableHashValueWithPrefix(htable, i, jr_HTablePrefixSize (htable))

#define				jr_HTableNextElementIndex(htable, i)							\
					jr_HTableNextElementIndexWithPrefix(htable, i, jr_HTablePrefixSize (htable))

#define				jr_HTableIsValidIndex(htable, i)								\
					jr_HTableIsValidIndexWithPrefix(htable, i, jr_HTablePrefixSize (htable))

#define				jr_HTableIsDeletedIndex(htable, i)								\
					jr_HTableIsDeletedIndexWithPrefix(htable, i, jr_HTablePrefixSize (htable))

#define				jr_HTableNativeElement(htable, i, type)							\
					jr_HTableNativeElementWithPrefix (htable, i, type, jr_HTablePrefixSize (htable))

#define				jr_HTableSetNativeElement(htable, i, v, type)					\
					jr_HTableSetNativeElementWithPrefix (							\
						htable, i, v, type, jr_HTablePrefixSize (htable)			\
					)


/******** Ptr Value Interface ********/

#define				jr_HTableKeyPtrValueWithPrefix(htable, i, prefix_size)					\
					jr_HTableNativeElementWithPrefix (htable, i, void *, prefix_size)

#define				jr_HTableSetKeyPtrValueWithPrefix(htable, i, v, prefix_size)			\
					jr_HTableSetNativeElementWithPrefix (htable, i, v, void *, prefix_size)


#define				jr_HTableDataPtrValueFromElementPtr(htable, element_ptr)				\
					(((void **) (element_ptr))[1])

#define				jr_HTableDataPtrValueWithPrefix(htable, i, prefix_size)					\
					jr_HTableDataPtrValueFromElementPtr (									\
						htable, jr_HTableElementPtrWithPrefix (htable, i, prefix_size)		\
					)


#define				jr_HTableSetDataPtrValueWithPrefix(htable, i, v, prefix_size)			\
					(jr_HTableDataPtrValueWithPrefix (htable, i, prefix_size) = (v))



#define				jr_HTableKeyPtrValue(htable, i)											\
					jr_HTableKeyPtrValueWithPrefix (htable, i, jr_HTablePrefixSize (htable))

#define				jr_HTableSetKeyPtrValue(htable, i, v)									\
					jr_HTableSetKeyPtrValueWithPrefix (htable, i, v, jr_HTablePrefixSize (htable))

#define				jr_HTableDataPtrValue(htable, i)										\
					jr_HTableDataPtrValueWithPrefix (htable, i, jr_HTablePrefixSize (htable))

#define				jr_HTableSetDataPtrValue(htable, i, v)									\
					jr_HTableSetDataPtrValueWithPrefix (htable, i, v, jr_HTablePrefixSize (htable))

#endif
