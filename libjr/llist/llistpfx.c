#include "project.h"

void jr_LListSetContainers (
	jr_LList *				list, ...)
{
	va_list					arg_list;

	va_start (arg_list, list);

	jr_LListSetContainersFromVA_List (list, arg_list);

	va_end (arg_list);
}


void jr_LListSetContainersFromVA_List (list, arg_list)
	jr_LList *				list;
	va_list					arg_list;
{
	jr_PrefixInfoType *		prefix_array;
	jr_int					prefix_size;
	jr_int					needs_transform_fn			= 0;


	prefix_array			= jr_PrefixArrayCreateFromVA_List	(arg_list, needs_transform_fn);
	prefix_size				= jr_PrefixArrayInitPrefixes		(prefix_array);

	jr_LListSetPrefixSize (list, prefix_size);

	jr_PrefixArrayDestroy (prefix_array);
}

void jr_LListTransformPrefix (list, el_ptr, magnitude, which_way, all_elements)
	jr_LList *			list;
	void *				el_ptr;
	unsigned jr_int		magnitude;
	jr_int				which_way;
	jr_int				all_elements;
{
	void *				next_ptr;
	void *				old_el_ptr;


	old_el_ptr			= jr_UnTransformAListPtr (el_ptr, magnitude, which_way);


	if (old_el_ptr  ==  jr_LListHeadPtr (list)) {
		jr_LListHeadPtr (list)	= el_ptr;
	}

	if (old_el_ptr  ==  jr_LListTailPtr (list)) {
		jr_LListTailPtr (list)	= el_ptr;
	}

	
	if (all_elements) {
		/*
		 * do the next pointer, or the head and tail pointers
		 */
		next_ptr			= jr_LListNextElementPtr (list, el_ptr);

		if (next_ptr) {
			next_ptr		= jr_TransformAListPtr (next_ptr, magnitude, which_way);
			jr_LListNextElementPtr (list, el_ptr)		= next_ptr;
		}
	}
	else {
		/*
		 * have to find reference to old element
		 */
		void *			prev_ptr;

		prev_ptr		= 0;
		jr_LListForEachElementPtr (list, next_ptr) {
			if (next_ptr == old_el_ptr) {
				if (prev_ptr) {
					jr_LListNextElementPtr (list, prev_ptr)	= el_ptr;
				}
				break;
			}
			prev_ptr	= next_ptr;
		}
	}
}

void jr_LListMakeNullElement (list, el_ptr)
	jr_LList *				list;
	void *					el_ptr;
{
	jr_LListNextElementPtr (list, el_ptr)		= 0;
}
