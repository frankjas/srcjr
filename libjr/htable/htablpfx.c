#include "project.h"

void jr_HTableSetContainers (
	jr_HTable *				htable, ...)
{
	va_list					arg_list;

	va_start (arg_list, htable);

	jr_HTableSetContainersFromVA_List (htable, arg_list);

	va_end (arg_list);
}


void jr_HTableSetContainersFromVA_List (htable, arg_list)
	jr_HTable *				htable;
	va_list					arg_list;
{
	jr_PrefixInfoType *		prefix_array;
	jr_int					prefix_size;
	jr_int					needs_transform_fn			= 1;


	prefix_array			= jr_PrefixArrayCreateFromVA_List	(arg_list, needs_transform_fn);
	prefix_size				= jr_PrefixArrayInitPrefixes		(prefix_array);

	if (htable) {
		jr_HTableSetPrefixSize (htable, prefix_size, jr_PrefixArrayTransformElement, prefix_array);
	}
}

