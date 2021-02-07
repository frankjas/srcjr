#include "project.h"

void jr_DListSetContainers (
	jr_DList *				list, ...)
{
	va_list					arg_list;

	va_start (arg_list, list);

	jr_DListSetContainersFromVA_List (list, arg_list);

	va_end (arg_list);
}


void jr_DListSetContainersFromVA_List (list, arg_list)
	jr_DList *				list;
	va_list					arg_list;
{
	jr_PrefixInfoType *		prefix_array;
	jr_int					prefix_size;
	jr_int					needs_transform_fn			= 0;


	prefix_array			= jr_PrefixArrayCreateFromVA_List	(arg_list, needs_transform_fn);
	prefix_size				= jr_PrefixArrayInitPrefixes		(prefix_array);

	jr_DListSetPrefixSize (list, prefix_size);

	jr_PrefixArrayDestroy (prefix_array);
}


void jr_DListTransformPrefix (list, el_ptr, magnitude, which_way, all_elements)
	jr_DList *			list;
	void *				el_ptr;
	unsigned jr_int		magnitude;
	jr_int				which_way;
	jr_int				all_elements;
{
	void *				next_ptr;
	void *				prev_ptr;
	void *				old_el_ptr;
	
	prev_ptr			= jr_DListPrevElementPtr (list, el_ptr);
	next_ptr			= jr_DListNextElementPtr (list, el_ptr);

	old_el_ptr			= jr_UnTransformAListPtr (el_ptr, magnitude, which_way);


	if (old_el_ptr  ==  jr_DListHeadPtr (list)) {
		jr_DListHeadPtr (list)	= el_ptr;
	}

	if (old_el_ptr  ==  jr_DListTailPtr (list)) {
		jr_DListTailPtr (list)	= el_ptr;
	}


	if (all_elements) {
		/*
		 * Do this element's references
		 */
		if (prev_ptr) {
			prev_ptr		= jr_TransformAListPtr (prev_ptr, magnitude, which_way);
			jr_DListPrevElementPtr (list, el_ptr)			= prev_ptr;
		}

		if (next_ptr) {
			next_ptr		= jr_TransformAListPtr (next_ptr, magnitude, which_way);
			jr_DListNextElementPtr (list, el_ptr)			= next_ptr;
		}
	}
	else {
		/*
		 * Do references to this element
		 */
		if (prev_ptr) {
			jr_DListNextElementPtr (list, prev_ptr)		= el_ptr;
		}
		if (next_ptr) {
			jr_DListPrevElementPtr (list, next_ptr)		= el_ptr;
		}
	}
}

void jr_MakeNullDListElementWithPrefix (el_ptr, prefix_size)
	const void *			el_ptr;
	jr_int					prefix_size;
{
	jr_NextDListElementPtrWithPrefix ((void *) el_ptr, prefix_size)		= 0;
	jr_PrevDListElementPtrWithPrefix ((void *) el_ptr, prefix_size)		= 0;
}

jr_int jr_IsNullDListElementWithPrefix (el_ptr, prefix_size)
	const void *			el_ptr;
	jr_int					prefix_size;
{
	if (	jr_NextDListElementPtrWithPrefix ((void *) el_ptr, prefix_size) == 0
		&&	jr_PrevDListElementPtrWithPrefix ((void *) el_ptr, prefix_size) == 0) {

		return 1;
	}
	return 0;
}
