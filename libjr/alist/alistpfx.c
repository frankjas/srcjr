#include "project.h"

void jr_AListSetContainers (jr_AList *list, ...)
{
	va_list					arg_list;

	va_start (arg_list, list);

	jr_AListSetContainersFromVA_List (list, arg_list);

	va_end (arg_list);
}


void jr_AListSetContainersFromVA_List (list, arg_list)
	jr_AList *				list;
	va_list					arg_list;
{
	jr_PrefixInfoType *		prefix_array;
	jr_int					prefix_size;
	jr_int					needs_transform_fn			= 1;


	prefix_array			= jr_PrefixArrayCreateFromVA_List	(arg_list, needs_transform_fn);
	prefix_size				= jr_PrefixArrayInitPrefixes		(prefix_array);

	if (list) {
		jr_AListSetPrefixSize (
			list, prefix_size, (void (*) ()) jr_PrefixArrayTransformElement, prefix_array
		);
	}
}


void jr_AListTransformPrefixes (list, old_buffer_ptr, prefix_transform_fn, prefix_transform_arg)
	jr_AList *			list;
	void *				old_buffer_ptr;
	void				(*prefix_transform_fn) ();
	void *				prefix_transform_arg;
{
	void *				el_ptr;
	unsigned jr_int		magnitude;
	jr_int				which_way;
	jr_int				all_elements			= 1;

	if ((char *) old_buffer_ptr  >  list->buffer) {
		magnitude		= (char *) old_buffer_ptr - list->buffer;
		which_way		= -1;
	}
	else {
		magnitude		= list->buffer - (char *) old_buffer_ptr;
		which_way		= 1;
	}

	jr_AListForEachElementPtr (list, el_ptr) {
		(*prefix_transform_fn) (prefix_transform_arg, el_ptr, magnitude, which_way, all_elements);
	}
}


