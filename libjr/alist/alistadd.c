#include "project.h"

void *jr_AListNewTailPtr (list)
	jr_AList *list;
{
    if (jr_AListSize(list) == list->buffer_capacity) {
		jr_AListIncreaseCapacity (list, jr_AListSize(list) + 1);
	}
	list->free_ptr += list->objsize;
	list->free_index++;

	return (list->free_ptr - list->objsize + list->prefix_size);
}

void *jr_AListNewPrevElementPtr (list, insert_ptr_arg)
	jr_AList *			list;
	void *				insert_ptr_arg;
{
	register char *		insert_ptr		= insert_ptr_arg ;


	if (insert_ptr) {
		assert	((insert_ptr >=  list->buffer) && (insert_ptr <= list->free_ptr));
		insert_ptr			-= list->prefix_size;
	}
	else {
		insert_ptr			= list->free_ptr;
	}


    if (jr_AListSize(list) == list->buffer_capacity) {
		jr_int		insert_offset;

		insert_offset	= insert_ptr - list->buffer;

		jr_AListIncreaseCapacity (list, jr_AListSize(list) + 1);

		insert_ptr		= list->buffer + insert_offset;
	}


	if (jr_AListHasPrefixTransformFn (list)) {
		register char *		curr_ptr;
		register jr_int		objsize			= list->objsize;
		register jr_int		i;

		for (
			curr_ptr  =   list->free_ptr - objsize;
			curr_ptr  >=  insert_ptr;
			curr_ptr  -=  objsize) {

			for (i = 0;  i < objsize;  i++) {
				curr_ptr[objsize + i]	= curr_ptr[i];
			}

			(*jr_AListPrefixTransformFn (list)) (
				jr_AListPrefixTransformArg (list),
				curr_ptr + objsize + list->prefix_size, objsize, 1, 0
			);
		}
	}
	else {
		memmove (insert_ptr + list->objsize, insert_ptr, list->free_ptr - insert_ptr);
	}

	list->free_ptr += list->objsize;
	list->free_index++;

	return (insert_ptr + list->prefix_size);
}


void *jr_AListNewNextElementPtr (list, prev_ptr_arg)
	jr_AList *			list;
	void *				prev_ptr_arg;
{
	char *				prev_ptr				= prev_ptr_arg;
	char *				insert_ptr;
	register char *		new_ptr;


	if (prev_ptr) {
		insert_ptr			= prev_ptr	+ list->objsize;
		assert	((prev_ptr >=  list->buffer) && (prev_ptr < list->free_ptr));
	}
	else {
		insert_ptr			= list->buffer + list->prefix_size;
	}

	new_ptr = jr_AListNewPrevElementPtr (list, insert_ptr);

	return new_ptr;
}

