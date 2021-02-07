#include "project.h"

jr_AList *jr_AListCreate (objsize)
	jr_int objsize ;
{
	jr_AList *list = (jr_AList *) jr_malloc (sizeof (jr_AList));

	jr_AListInit (list, objsize);

	return (list);
}

void jr_AListInit (list, objsize)
	jr_AList *			list;
	jr_int				objsize;
{
	list->buffer				= 0;

	list->buffer_capacity		= 0;
	list->free_ptr				= 0;
	list->free_index			= 0;

	list->objsize				= objsize;
	list->prefix_size			= 0;
}

void jr_AListDestroy (list)
	jr_AList *list;
{
	jr_AListUndo (list);
	jr_free (list);
}

void jr_AListUndo (list)
	jr_AList *list;
{
	if (	jr_AListHasPrefixTransformFn (list)
		&&	jr_AListPrefixTransformFn (list)  ==  (void (*)()) jr_PrefixArrayTransformElement) {

		jr_PrefixArrayDestroy (jr_AListPrefixTransformArg (list));
	}
	if (list->buffer) {
		if (jr_AListPrefixSize (list)  >  0) {
			jr_free (list->buffer  -  sizeof (jr_AListPrefixTransformStruct));
		}
		else {
			jr_free (list->buffer);
		}
	}
}

void jr_AListEmpty (list)
	jr_AList *list;
{
	list->free_ptr		= list->buffer;
	list->free_index	= 0;
}

void  *jr_AListExtractBuffer (list)
	jr_AList *list;
{
	void *				tptr;

	if (jr_AListPrefixSize (list)  >  0) {
		void *			new_buffer;
		jr_int			num_bytes			= list->free_ptr - list->buffer;

		new_buffer		= jr_malloc (num_bytes);
		memcpy (new_buffer, list->buffer, num_bytes);

		jr_free (list->buffer  -  sizeof (jr_AListPrefixTransformStruct));

		tptr			= new_buffer;
	}
	else {
		tptr			= list->buffer;
	}

	list->buffer			= 0;
	list->buffer_capacity	= 0;
	list->free_ptr			= 0;
	list->free_index		= 0;

	return tptr;
}

void jr_AListSetPrefixSize (list, new_prefix_size, prefix_transform_fn, prefix_transform_arg)
	jr_AList *		list;
	jr_int			new_prefix_size;
	void			(*prefix_transform_fn) ();
	void *			prefix_transform_arg;
{
	jr_int			malloced_num_bytes			= 0;
	char *			malloced_ptr				= 0;

	/*
	 * Add an extra 8 byte struct to the beginning of the list buffer.
	 * This will store the transform function pointer and arg.
	 * If the buffer isn't allocated, and the transform function pointer
	 * is 0, then don't allocate a buffer yet.
	 *
	 * The other code will need to take into account the fact that
	 * the transform function macro can only be used if prefix size > 0
	 * and the buffer is non-zero.
	 */

	assert (jr_AListIsEmpty (list));

	if (list->buffer) {
		malloced_num_bytes		= list->buffer_capacity * list->objsize;
		malloced_ptr			= list->buffer;

		if (list->prefix_size  >  0) {
			malloced_num_bytes	+= sizeof (jr_AListPrefixTransformStruct);
			malloced_ptr		-= sizeof (jr_AListPrefixTransformStruct);
		}
	}

	if (list->prefix_size  >  0) {
		list->objsize			-= list->prefix_size;
	}

	if (new_prefix_size  >  0) {
		list->objsize			+= new_prefix_size;
		list->prefix_size		= new_prefix_size;

		if (malloced_ptr) {
			if (malloced_num_bytes  <  (jr_int) sizeof (jr_AListPrefixTransformStruct)) {
				jr_free (malloced_ptr);
				malloced_num_bytes	= 0;
				malloced_ptr		= 0;
			}
		}

		if (malloced_ptr == 0  &&  prefix_transform_fn) {
			malloced_num_bytes	= sizeof (jr_AListPrefixTransformStruct);
			malloced_ptr		= jr_malloc (malloced_num_bytes);
		}
		/*
		 * Else we already have a malloc()ed buffer, or no transform function.
		 */

		if (malloced_ptr) {
			malloced_num_bytes	-= sizeof (jr_AListPrefixTransformStruct);
			malloced_ptr		+= sizeof (jr_AListPrefixTransformStruct);
		}
	}

	if (malloced_ptr) {
		list->buffer			= malloced_ptr;
		list->buffer_capacity	= malloced_num_bytes / list->objsize;
		list->free_ptr			= malloced_ptr;
	}

	if (new_prefix_size > 0  &&  malloced_ptr) {
		jr_AListPrefixTransformFn (list)	= prefix_transform_fn;
		jr_AListPrefixTransformArg (list)	= prefix_transform_arg;
	}

	if (list->prefix_size == list->objsize) {
		fprintf (stderr, "\n");
		fprintf (stderr, "jr_AList: programmer error: prefix size == total data size\n");
		fprintf (stderr, "jr_AList: foreach pointer loops will skip the last element\n");
		jr_coredump ();
	}
}


jr_int jr_AListIncreaseCapacity (list, new_size)
	jr_AList *		list;
	jr_int			new_size;
{
	jr_int			newbytesize;
	jr_int			free_offset;
	char *			old_buffer_ptr;
	char *			new_buffer_ptr;


	if (new_size <= list->buffer_capacity) {
		return (list->buffer_capacity) ;
	}

	if (list->buffer_capacity != 0  &&  new_size < 2 * list->buffer_capacity) {
		new_size = 2 * list->buffer_capacity;
	}

	free_offset		= list->free_ptr - list->buffer;

	newbytesize		= new_size * list->objsize;
	old_buffer_ptr	= list->buffer;

	if (list->prefix_size  >  0) {
		newbytesize		+= sizeof (jr_AListPrefixTransformStruct);

		if (old_buffer_ptr) {
			old_buffer_ptr	-= sizeof (jr_AListPrefixTransformStruct);
		}
	}

	if (list->buffer != 0) {
		new_buffer_ptr	= jr_realloc (old_buffer_ptr, newbytesize);
	}
	else {
		new_buffer_ptr	= jr_malloc (newbytesize);
	}

	newbytesize			= (jr_int) jr_malloc_usable_size (new_buffer_ptr, newbytesize) ;

	if (list->prefix_size  >  0) {
		newbytesize		-= sizeof (jr_AListPrefixTransformStruct);
		new_buffer_ptr	+= sizeof (jr_AListPrefixTransformStruct);

		if (old_buffer_ptr) {
			old_buffer_ptr	+= sizeof (jr_AListPrefixTransformStruct);
		}
	}

	list->buffer			= new_buffer_ptr;
	list->buffer_capacity	= newbytesize / list->objsize;
	list->free_ptr			= new_buffer_ptr + free_offset;


	if (old_buffer_ptr == 0  &&  list->prefix_size > 0) {
		/*
		 * Didn't allocate space for the transform info since 
		 * there were no transform functions.
		 */
		jr_AListPrefixTransformFn (list)	= 0;
		jr_AListPrefixTransformArg (list)	= 0;
	}


	if (jr_AListHasPrefixTransformFn (list)) {
		jr_AListTransformPrefixes (
			list, old_buffer_ptr,
			jr_AListPrefixTransformFn (list), jr_AListPrefixTransformArg (list)
		);
	}

	return list->buffer_capacity;
}


void  *jr_AListBufferDup (list)
	jr_AList *list;
{
	if (list->buffer) {
		void *tptr;

		tptr = jr_memdup (list->buffer, list->free_ptr - list->buffer);
		return (tptr);
	}
	else return (0);
}


void jr_AListSetTailIndex (list, index)
	jr_AList *list;
	jr_int index;
{
	if (index < -1) return;

	if (index > jr_AListTailIndex (list)) {
		jr_AListIncreaseCapacity (list, index + 1);
	}

	list->free_ptr = jr_AListElementPtr (list, index + 1);
	list->free_index = index + 1;
}


void jr_AListInitCopy (dest, src)
	jr_AList *dest;
	jr_AList *src;
{
	jr_AListInit (dest, src->objsize);

	if (jr_AListPrefixSize (src)) {
		if (jr_AListHasPrefixTransformFn (src)) {
			jr_AListSetPrefixSize (
				dest, src->prefix_size,
				jr_AListPrefixTransformFn (src), jr_AListPrefixTransformArg (src)
			);
		}
		else {
			jr_AListSetPrefixSize (dest, src->prefix_size, 0, 0);
		}
	}

	jr_AListSetSize (dest, jr_AListSize (src));

	memcpy (dest->buffer, src->buffer, src->free_index * src->objsize);

	if (jr_AListHasPrefixTransformFn (dest)) {
		jr_AListTransformPrefixes (
			dest, src->buffer,
			jr_AListPrefixTransformFn (dest), jr_AListPrefixTransformArg (dest)
		);
	}
}


jr_AList *jr_AListDup (list)
	jr_AList *list;
{
	jr_AList *new_list = jr_malloc (sizeof (jr_AList));

	jr_AListInitCopy (list, new_list);

	return (new_list);
}



static void *_jr_AListElementPtr (alist, i)
	jr_AList *		alist;
	jr_int			i;
{
	if (0) {
		_jr_AListElementPtr (alist, i);
	}
	return jr_AListElementPtr (alist, i);
}

void jr_AListReverse (list)
	jr_AList *		list;
{
	jr_int			old_index;
	jr_int			new_index;
	void *			old_ptr;
	void *			new_ptr;
	void *			tmp_ptr				= jr_malloc (jr_AListElementSize (list));


	jr_AListForEachElementIndex (list, old_index) {

		if (old_index >= jr_AListSize (list) / 2) {
			break;
		}

		new_index		= jr_AListSize (list) - old_index - 1;

		old_ptr			= jr_AListElementPtr (list, old_index);
		new_ptr			= jr_AListElementPtr (list, new_index);

		memcpy (tmp_ptr, new_ptr, jr_AListElementSize (list));
		memcpy (new_ptr, old_ptr, jr_AListElementSize (list));
		memcpy (old_ptr, tmp_ptr, jr_AListElementSize (list));
	}

	jr_free (tmp_ptr);
}

void **jr_AListVectorPtr (list)
	jr_AList *		list;
{
	void *			ptr;

	if (jr_AListSize (list) > 0) {
		ptr		= jr_AListNativeTail (list, void *);

		if (ptr == 0) {
			return jr_AListHeadPtr (list);
		}
	}

	/*
	 * 7/15/2005: Either it's empty or has no null.
	 * Don't increase the size of the array but add a null.
	 */

	jr_AListIncreaseCapacity (list, jr_AListSize (list) + 1);

	jr_AListNativeSetElement (list, jr_AListSize (list), 0, void *);

	return jr_AListHeadPtr (list);
}

void jr_AListAppendString(
	jr_AList *		char_list,
	const char *	string)
{
	for (; *string; string++) {
		jr_AListNativeSetNewTail( char_list, *string, char);
	}
	jr_AListNativeSetNewTail( char_list, 0, char);
}
