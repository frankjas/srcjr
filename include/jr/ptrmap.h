#ifndef __jr_ptrmap_h___
#define __jr_ptrmap_h___

#include "ezport.h"

#include "jr/alist.h"

typedef struct {
	jr_AList	category_list[1];
	jr_int		max_category_size;
	jr_int		ptr_data_size;
} jr_PtrMapType;

typedef struct {
	jr_int		min_value;
	jr_int		num_values;

	jr_int		next_deleted_desc;
	jr_int		num_deletes;

	jr_AList	ptr_list[1];
} jr_PtrCategoryType;


extern void				jr_PtrMapInit					PROTO ((
							jr_PtrMapType *				ptr_map,
							jr_int						max_category_size,
							jr_int						ptr_data_size
						));

extern void				jr_PtrMapUndo					PROTO ((
							jr_PtrMapType *				ptr_map
						));

extern jr_PtrMapType *	jr_PtrMapCreate					PROTO ((
							jr_int						max_category_size,
							jr_int						ptr_data_size
						));

extern void				jr_PtrMapDestroy				PROTO ((
							jr_PtrMapType *				ptr_map
						));


extern jr_int			jr_PtrMapAddNewPtrWithFileName	PROTO ((
							jr_PtrMapType *				ptr_map,
							jr_int						category_kind,
							void *						ptr_value,
							const char *				file_name,
							jr_int						line_number
						));

#define					jr_PtrMapAddNewPtr(ptr_map, category_kind, ptr_value)			\
						jr_PtrMapAddNewPtrWithFileName (								\
							ptr_map, category_kind, ptr_value, 0, 0						\
						)

#define					jr_PtrMapCloseDesc(ptr_map, category_kind, ptr_desc)			\
						jr_PtrCategoryCloseDesc (										\
							jr_PtrMapCategoryPtr (ptr_map, category_kind), ptr_desc		\
						)

#define					jr_PtrMapGetPtr(ptr_map, category_kind, ptr_desc)				\
						jr_PtrCategoryGetPtr (											\
							jr_PtrMapCategoryPtr (ptr_map, category_kind), ptr_desc		\
						)

#define					jr_PtrMapSetPtr(ptr_map, category_kind, ptr_desc, v)			\
						jr_PtrCategorySetPtr (											\
							jr_PtrMapCategoryPtr (ptr_map, category_kind), ptr_desc, v	\
						)

#define					jr_PtrMapGetFileName(ptr_map, category_kind, ptr_desc)			\
						jr_PtrCategoryGetFileName (										\
							jr_PtrMapCategoryPtr (ptr_map, category_kind), ptr_desc		\
						)

#define					jr_PtrMapGetLineNumber(ptr_map, category_kind, ptr_desc)		\
						jr_PtrCategoryGetLineNumber (									\
							jr_PtrMapCategoryPtr (ptr_map, category_kind), ptr_desc		\
						)


#define					jr_PtrMapGetDataPtr(ptr_map, category_kind, ptr_desc)			\
						jr_PtrCategoryGetDataPtr (										\
							jr_PtrMapCategoryPtr (ptr_map, category_kind), ptr_desc		\
						)

#define					jr_PtrMapDataPtr(ptr_map, category_kind, ptr_desc)				\
						jr_PtrCategoryDataPtr (											\
							jr_PtrMapCategoryPtr (ptr_map, category_kind), ptr_desc		\
						)


#define					jr_PtrMapIsValidDesc(ptr_map, category_kind, ptr_desc)			\
						(		jr_PtrMapIsValidCategory (ptr_map, category_kind)		\
							&&	jr_PtrCategoryIsValidDesc (								\
									jr_PtrMapCategoryPtr (ptr_map, category_kind),		\
									ptr_desc											\
								)														\
						)


extern void				jr_PtrCategoryCloseDesc			PROTO ((
							jr_PtrCategoryType *		ptr_category,
							jr_int						ptr_desc
						));

extern void *			jr_PtrCategoryGetPtr			PROTO ((
							jr_PtrCategoryType *		ptr_category,
							jr_int						ptr_desc
						));

extern void				jr_PtrCategorySetPtr			PROTO ((
							jr_PtrCategoryType *		ptr_category,
							jr_int						ptr_desc,
							void *						ptr_value
						));

extern const char *		jr_PtrCategoryGetFileName		PROTO ((
							jr_PtrCategoryType *		ptr_category,
							jr_int						ptr_desc
						));

extern jr_int			jr_PtrCategoryGetLineNumber		PROTO ((
							jr_PtrCategoryType *		ptr_category,
							jr_int						ptr_desc
						));

extern void *			jr_PtrCategoryGetDataPtr		PROTO ((
							jr_PtrCategoryType *		ptr_category,
							jr_int						ptr_desc
						));


#define	jr_PtrMapMaxOpenPtrs(ptr_map)					\
		((ptr_map)->max_category_size)

#define	jr_PtrMapForEachCategoryKind(ptr_map, category_kind)			\
		jr_AListForEachElementIndex ((ptr_map)->category_list, category_kind)


#define	jr_PtrMapCategoryNumOpenPtrs(ptr_map, category_kind)			\
		jr_PtrCategoryNumOpenPtrs (jr_PtrMapCategoryPtr (ptr_map, category_kind))



#define jr_PtrMapIsValidCategory(ptr_map, i)											\
		jr_AListIsValidIndex ((ptr_map)->category_list, i)

#define jr_PtrMapCategoryPtr(ptr_map, i)												\
		((jr_PtrCategoryType *) jr_AListElementPtr ((ptr_map)->category_list, i))


#define jr_PtrCategoryIsValidDesc(ptr_category, ptr_desc)								\
		(jr_AListIsValidIndex (															\
			(ptr_category)->ptr_list, (ptr_desc) - (ptr_category)->min_value			\
		)  &&  jr_PtrCategoryPtrValue (ptr_category, ptr_desc) != 0)

#define jr_PtrCategoryNumOpenPtrs(ptr_category)											\
		(jr_AListSize ((ptr_category)->ptr_list)  -  (ptr_category)->num_deletes)

#define jr_PtrCategoryMinValue(ptr_category)											\
		((ptr_category)->min_value)

#define jr_PtrCategoryMaxValue(ptr_category)											\
		((ptr_category)->min_value  +  jr_AListSize ((ptr_category)->ptr_list))


#define jr_PtrCategoryForEachOpenDesc(ptr_category, ptr_desc)							\
		for ((ptr_desc) = jr_PtrCategoryMinValue (ptr_category);						\
			(ptr_desc) < jr_PtrCategoryMaxValue (ptr_category);							\
			(ptr_desc)++)																\
			if (jr_PtrCategoryPtrValue (ptr_category, ptr_desc))


typedef struct {
	void *			ptr_value;
	jr_int			next_deleted_desc;

	const char *	file_name;
	jr_int			line_number;
} jr_PtrInfoStruct;


#define	jr_PtrCategoryPtrInfo(ptr_category, ptr_desc)									\
		((jr_PtrInfoStruct *) jr_AListElementPtr (										\
			(ptr_category)->ptr_list, (ptr_desc) - (ptr_category)->min_value)			\
		)

#define	jr_PtrCategoryPtrValue(ptr_category, ptr_desc)									\
		(jr_PtrCategoryPtrInfo (ptr_category, ptr_desc)->ptr_value)

#define	jr_PtrCategoryFileName(ptr_category, ptr_desc)									\
		(jr_PtrCategoryPtrInfo (ptr_category, ptr_desc)->file_name)

#define	jr_PtrCategoryLineNumber(ptr_category, ptr_desc)								\
		(jr_PtrCategoryPtrInfo (ptr_category, ptr_desc)->line_number)


#define	jr_PtrCategoryDataPtr(ptr_category, ptr_desc)									\
		(((jr_PtrInfoStruct *) jr_AListElementPtr (										\
			(ptr_category)->ptr_list, (ptr_desc) - (ptr_category)->min_value)			\
		) + 1)

#define	jr_PtrCategoryDataPtrSize(ptr_category)											\
		(jr_AListElementSize ((ptr_category)->ptr_list)  -  sizeof (jr_PtrInfoStruct))

#endif
