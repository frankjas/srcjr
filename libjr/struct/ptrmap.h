#ifndef _ptrmap_h___
#define _ptrmap_h___

#define _POSIX_SOURCE 1

#include "ezport.h"

#include "jr/malloc.h"
#include "jr/ptrmap.h"



extern void				jr_PtrCategoryInit				PROTO ((
							jr_PtrCategoryType *		ptr_category,
							jr_int						min_value,
							jr_int						num_values,
							jr_int						ptr_data_size
						));

extern void				jr_PtrCategoryUndo				PROTO ((
							jr_PtrCategoryType *		ptr_category
						));

extern jr_int			jr_PtrCategoryOpenPtr			PROTO ((
							jr_PtrCategoryType *		ptr_category,
							void *						ptr_value,
							const char *				file_name,
							jr_int						line_number
						));


#define	jr_PtrCategoryNextDeletedDesc(ptr_category, ptr_desc)							\
		(jr_PtrCategoryPtrInfo (ptr_category, ptr_desc)->next_deleted_desc)

#define	jr_PtrCategorySetPtrValue(ptr_category, ptr_desc, v)							\
		(jr_PtrCategoryPtrInfo (ptr_category, ptr_desc)->ptr_value = (v))

#define	jr_PtrCategorySetNextDeletedDesc(ptr_category, ptr_desc, v)						\
		(jr_PtrCategoryPtrInfo (ptr_category, ptr_desc)->next_deleted_desc = (v))


extern void				jr_PtrInfoInit					PROTO ((
							jr_PtrInfoStruct *			ptr_info,
							void *						ptr_value,
							const char *				file_name,
							jr_int						line_number
						));


#endif
