#include "project.h"

void *jr_DListInsertElement (list, next_const_ptr, new_const_ptr)
	jr_DList *		list ;
	const void *	next_const_ptr ;
	const void *	new_const_ptr ;
{
	void *			next			= (void *) next_const_ptr;
	void *			new				= (void *) new_const_ptr;
	void *			prev;

	jr_DListNextElementPtr (list, new) = next;

	if (next == 0) {
		/* append to the tail */

		prev = jr_DListTailPtr(list);
		jr_DListTailPtr(list) = new;
	}
	else {
		prev = jr_DListPrevElementPtr (list,next);
		jr_DListPrevElementPtr (list, next) = new;
	}

	jr_DListPrevElementPtr (list,new) = prev;

	if (prev == 0) {
		/* insert at the head */

		jr_DListHeadPtr(list) = new;
	}
	else {
		jr_DListNextElementPtr (list,prev) = new;
	}
	return (void *) new;
}

void *jr_DListAppendElement (list, prev_const_ptr, new_const_ptr)
	jr_DList *		list;
	const void *	prev_const_ptr;
	const void *	new_const_ptr;
{
	void *			prev			= (void *) prev_const_ptr;
	void *			new				= (void *) new_const_ptr;
	void *			next;

	jr_DListPrevElementPtr (list, new) = prev;

	if (prev == 0) {
		/* insert to the head */

		next = jr_DListHeadPtr(list);
		jr_DListHeadPtr(list) = new;
	}
	else {
		next = jr_DListNextElementPtr (list, prev);
		jr_DListNextElementPtr (list, prev) = new;
	}

	jr_DListNextElementPtr (list,new) = next;

	if (next == 0) {
		/* add to tail */

		jr_DListTailPtr(list) = new;
	}
	else {
		jr_DListPrevElementPtr (list,next) = new;
	}

	return (void *) new;
}

