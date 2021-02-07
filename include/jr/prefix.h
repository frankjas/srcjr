#ifndef __jr_prefix_h___
#define __jr_prefix_h___

#include "ezport.h"

#include <stdarg.h>

typedef struct {
	char *		container_name;
	void *		container_ptr;
	jr_int		prefix_id;
	jr_int		prefix_size;
	jr_int		(*element_prefix_size_fn) ();
	void		(*prefix_transform_fn) ();
	void		(*set_prefix_size_fn) ();
	
	unsigned	needs_transform_fn				: 1;
} jr_PrefixInfoType;



extern void			jr_PrefixAddContainerType			PROTO ((
						char *							str_const_container_name,
						jr_int							(*element_prefix_size_fn) (),
						void 							(*prefix_transform_fn) (),
						void 							(*set_prefix_size_fn) (),
						jr_int							needs_transform_fn
					));

extern void			jr_SetContainers					PROTO ((
						char *							container_name,
						void *							container_ptr,
						...
					));


extern jr_PrefixInfoType *jr_PrefixArrayCreateFromVA_List	PROTO ((
						va_list					arg_list,
						jr_int					needs_transform_fn
					));

extern void			jr_PrefixArrayDestroy				PROTO ((
						jr_PrefixInfoType *				prefix_array
					));

extern void			jr_PrefixArraySetInfo				PROTO ((
						jr_PrefixInfoType *				prefix_array,
						char *							container_name,
						jr_int							prefix_size,
						void							(*prefix_transform_fn) ()
					));

extern void			jr_PrefixArrayTransformElement		PROTO ((
						jr_PrefixInfoType *				prefix_array,
						void *							el_ptr,
						unsigned jr_int					magnitude,
						jr_int							which_way,
						jr_int							all_elements
					));

extern jr_int		jr_PrefixArrayInitPrefixes			PROTO ((
						jr_PrefixInfoType *				prefix_array
					));

#define jr_PrefixArrayForEachInfoPtr(prefix_array, prefix_info)		\
		for ((prefix_info) = prefix_array;  !jr_PrefixInfoIsNull (prefix_info);  (prefix_info)++) 


/******** Prefix Info ********/

extern void			jr_PrefixInfoInit					PROTO ((
						jr_PrefixInfoType *				prefix_info,
						char *							container_name,
						void *							container_ptr
					));

extern void			jr_PrefixInfoUndo					PROTO ((
						jr_PrefixInfoType *				prefix_info
					));


extern void			jr_PrefixInfoInitPrefix				PROTO ((
						jr_PrefixInfoType *				prefix_info
					));

#define jr_PrefixInfoContainerPtr(prefix_info)			((prefix_info)->container_ptr)
#define jr_PrefixInfoContainerName(prefix_info)			((prefix_info)->container_name)
#define jr_PrefixInfoPrefixSize(prefix_info)			((prefix_info)->prefix_size)
#define jr_PrefixInfoTransformFn(prefix_info)			((prefix_info)->prefix_transform_fn)
#define jr_PrefixInfoNeedsTransformFn(prefix_info)		((prefix_info)->needs_transform_fn)
#define jr_PrefixInfoSetTransformFn(prefix_info, v)		((prefix_info)->prefix_transform_fn = (v))
#define jr_PrefixInfoSetPrefixSize(prefix_info, v)		((prefix_info)->prefix_size = (v))


#define jr_PrefixInfoIsNull(prefix_info)				((prefix_info)->container_name == 0)


extern jr_PrefixInfoType	jr_PrefixInfoArray [];


#endif
