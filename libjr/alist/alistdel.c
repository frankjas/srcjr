#include "project.h"

void *jr_AListDeletedTailPtr (list)
	jr_AList *list;
{
	if (jr_AListSize(list) == 0) return (0);

	list->free_ptr -= list->objsize;
	list->free_index--;
	return (list->free_ptr + list->prefix_size);
}
	
void jr_AListTruncate (list, i)
	jr_AList *	list;
	jr_int		i;
{
	if (i >= jr_AListSize (list)) {
		return;
	}
	if (i < 0) {
		i = 0;
	}

	list->free_index	= i;
	list->free_ptr		= jr_AListElementPtr (list, i);
}
	
void jr_AListDeleteRangeFromPtr (list, delete_ptr_arg, num_entries)
	jr_AList *			list;
	void *				delete_ptr_arg;
	jr_int				num_entries;
{
	register char *		delete_ptr			= delete_ptr_arg ;
	jr_int				num_move_bytes;
	jr_int				num_delete_bytes;


	if (num_entries <= 0) {
		return;
	}

	delete_ptr			-= list->prefix_size;

	assert	((delete_ptr >=  list->buffer) && (delete_ptr < list->free_ptr));

	num_delete_bytes	= list->objsize * num_entries;
	num_move_bytes		= list->free_ptr - delete_ptr - num_delete_bytes;


	if (jr_AListHasPrefixTransformFn (list)) {
		char *		curr_ptr;
		jr_int		i;

		for (
			curr_ptr  =  delete_ptr + num_delete_bytes;
			curr_ptr  <  list->free_ptr;
			curr_ptr  += list->objsize) {

			for (i = 0;  i < list->objsize;  i++) {
				curr_ptr[i - num_delete_bytes]	= curr_ptr[i];
			}

			(*jr_AListPrefixTransformFn (list)) (
				jr_AListPrefixTransformArg (list),
				curr_ptr - num_delete_bytes + list->prefix_size,
				num_delete_bytes, -1, 0
			);
		}
	}
	else {
		memmove (delete_ptr,  delete_ptr + num_delete_bytes,  num_move_bytes);
	}
	list->free_ptr		-= num_delete_bytes;
	list->free_index	-= num_entries;
}

