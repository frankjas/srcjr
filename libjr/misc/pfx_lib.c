#include "pfx_proj.h"


void jr_SetContainers (
	char *					container_name,
	void *					container_ptr,
							...)
{
	va_list					arg_list;
	jr_PrefixInfoType *		prefix_array;
	jr_PrefixInfoType		prefix_info [1];
	jr_int					prefix_size;

	va_start (arg_list, container_ptr);

	jr_PrefixInfoInit (prefix_info, container_name, container_ptr);

	if (jr_PrefixInfoNeedsTransformFn (prefix_info)) {
		fprintf (stderr,
			"jr_SetContainers(): can't set prefixes for type '%s', use %sSetContainers()\n", 
			container_name,
			container_name
		);
		abort ();
	}

	prefix_array			= jr_PrefixArrayCreateFromVA_List	(arg_list, 0);
	prefix_size				= jr_PrefixArrayInitPrefixes		(prefix_array);

	jr_PrefixInfoSetPrefixSize (prefix_info, prefix_size);
	jr_PrefixInfoInitPrefix (prefix_info);

	jr_PrefixInfoUndo (prefix_info);
	jr_PrefixArrayDestroy (prefix_array);
}

	

jr_PrefixInfoType *	jr_PrefixArrayCreateFromVA_List (arg_list, needs_transform_fn)
	va_list					arg_list;
	jr_int					needs_transform_fn;
{
	jr_AList				prefix_list[1];
	jr_PrefixInfoType *		prefix_info;

	char *					container_name;
	void *					container_ptr;

	jr_int					prefix_size;
	jr_int					element_prefix_size;


	jr_AListInit (prefix_list, sizeof (jr_PrefixInfoType));


	for (	container_name = va_arg (arg_list, char *);
			container_name != 0;
			container_name = va_arg (arg_list, char *)
		) {

		container_ptr			= va_arg (arg_list, void *);

		prefix_info				= jr_AListNewTailPtr (prefix_list);

		jr_PrefixInfoInit (prefix_info, container_name, container_ptr);

		if (needs_transform_fn  &&  jr_PrefixInfoTransformFn (prefix_info)  ==  0) {
			fprintf (stderr,
				"jr_PrefixInfoInit(): type '%s' cannot be used in this parent container.\n",
				container_name
			);
			abort ();
		}
	}

	prefix_size = 0;

	jr_AListForEachElementPtrRev (prefix_list, prefix_info) {

		element_prefix_size		= jr_PrefixInfoElementPrefixSize (prefix_info);

		if (element_prefix_size == 0) {
			/*
			 * use the custom prefix size stored in the prefix_size field
			 */
			element_prefix_size		= jr_PrefixInfoPrefixSize (prefix_info);
		}

		jr_PrefixInfoSetPrefixSize (prefix_info, prefix_size);

		prefix_size					+= element_prefix_size;
	}

	prefix_info						= jr_AListNewTailPtr (prefix_list);
	prefix_info->container_name		= 0;
	prefix_info->prefix_size		= prefix_size;
	/*
	 * store the total prefix size in the null elements prefix_size field
	 */

	prefix_info	= jr_AListBufferDup (prefix_list);

	jr_AListUndo (prefix_list);

	return prefix_info;
}


void jr_PrefixArrayDestroy (prefix_array)
	jr_PrefixInfoType *		prefix_array;
{
	jr_PrefixInfoType *		prefix_info;

	jr_PrefixArrayForEachInfoPtr (prefix_array, prefix_info) {
		jr_PrefixInfoUndo (prefix_info);
	}
	jr_free (prefix_array);
}


jr_int jr_PrefixArrayInitPrefixes (prefix_array)
	jr_PrefixInfoType *		prefix_array;
{
	jr_PrefixInfoType *		prefix_info;


	jr_PrefixArrayForEachInfoPtr (prefix_array, prefix_info) {
		jr_PrefixInfoInitPrefix (prefix_info);
	}

	/*
	 * the total prefix size of the array is stored in the null element's
	 * prefix_size field
	 */

	return jr_PrefixInfoPrefixSize (prefix_info);
}



void jr_PrefixInfoInit (prefix_info, container_name, container_ptr)
	jr_PrefixInfoType *				prefix_info;
	char *							container_name;
	void *							container_ptr;
{
	jr_PrefixInfoType *				static_prefix_info;

	memset (prefix_info, 0, sizeof (jr_PrefixInfoType));

	prefix_info->container_name		= jr_strdup (container_name);
	prefix_info->container_ptr		= container_ptr;
	prefix_info->prefix_size		= 0;


	jr_PrefixArrayForEachInfoPtr (jr_PrefixInfoArray, static_prefix_info) {
		if (strcmp (container_name, static_prefix_info->container_name) == 0) {
			prefix_info->prefix_id				= static_prefix_info->prefix_id;
			prefix_info->prefix_transform_fn	= static_prefix_info->prefix_transform_fn;
			prefix_info->needs_transform_fn		= static_prefix_info->needs_transform_fn;
			return;
		}
	}

	fprintf (stderr, "jr_PrefixInfoInit(): can't use '%s' in this way. Use %sSetContainers()\n",
		container_name,
		container_name
	);
	abort ();
}


void jr_PrefixInfoUndo (prefix_info)
	jr_PrefixInfoType *				prefix_info;
{
	jr_free (prefix_info->container_name);
}


void jr_PrefixInfoInitPrefix (prefix_info)
	jr_PrefixInfoType *			prefix_info;
{
	void						(*set_prefix_size_fn) ();


	if (prefix_info->container_ptr == 0) {
		return;
	}

	set_prefix_size_fn			= jr_PrefixInfoArray [prefix_info->prefix_id].set_prefix_size_fn;

	if (set_prefix_size_fn == 0) {
		fprintf (stderr,
			"jr_PrefixInfoSetPrefixSize(): container type '%s' can't set the prefix size\n",
			jr_PrefixInfoArray [prefix_info->prefix_id].container_name
		);
		abort ();
	}

	(*set_prefix_size_fn) (prefix_info->container_ptr, prefix_info->prefix_size);
}

jr_int jr_PrefixInfoElementPrefixSize (prefix_info)
	jr_PrefixInfoType *			prefix_info;
{
	jr_int						(*element_prefix_size_fn) ();
	jr_PrefixInfoType *			static_prefix_info;

	static_prefix_info			= jr_PrefixInfoArray + prefix_info->prefix_id;
	element_prefix_size_fn		= static_prefix_info->element_prefix_size_fn;

	return (*element_prefix_size_fn) (prefix_info->container_ptr);
}


void jr_PrefixArrayTransformElement (prefix_array, el_ptr, magnitude, which_way, all_elements)
	jr_PrefixInfoType *		prefix_array;
	void *					el_ptr;
	unsigned jr_int			magnitude;
	jr_int					which_way;
	jr_int					all_elements;
{
	jr_PrefixInfoType *		prefix_info;

	jr_PrefixArrayForEachInfoPtr (prefix_array, prefix_info) {

		if (	jr_PrefixInfoContainerPtr (prefix_info)
			&&	jr_PrefixInfoTransformFn (prefix_info)) {

			(*jr_PrefixInfoTransformFn (prefix_info)) (
				jr_PrefixInfoContainerPtr (prefix_info),
				el_ptr, magnitude, which_way, all_elements
			);
		}
	}
}



#ifdef links_on_unresolved_functions_only

void jr_PrefixDummyLinkFn()
{
	jr_PrefixAddContainerType ("dummy_name", 0, 0, 0, 0);
}

#endif
