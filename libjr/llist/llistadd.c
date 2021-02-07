#include "project.h"

void *jr_LListAppendElement (list, prev, new)
	jr_LList *		list;
	void *			prev;
	void *			new;
{

	if (prev == 0) {
		/* do an add to head */

		jr_LListNextElementPtr (list, new) = jr_LListHeadPtr(list);
		jr_LListHeadPtr(list) = new;
	}
	else {
		jr_LListNextElementPtr (list, new) = jr_LListNextElementPtr (list, prev);
		jr_LListNextElementPtr (list, prev) = new;
	}
	if (prev == jr_LListTailPtr(list)) {
		/* 
		 * prev is the current tail 
		 * prev could be 0
		 */
		jr_LListTailPtr(list) = new;
	}
	return (new);
}

void *jr_LListExtractNextElement (list, prev)
	jr_LList *	list;
	void *		prev;
{
	void *		deleted_element;
	

	if (prev == 0) {
		/*
		 * do a delete from the head
		 */
		deleted_element				= jr_LListHeadPtr (list);

		if (deleted_element == 0) {
			return 0;
		}
		jr_LListHeadPtr (list)		= jr_LListNextElementPtr (list, deleted_element);
	}
	else {
		deleted_element				= jr_LListNextElementPtr (list, prev);

		if (deleted_element == 0) {
			/*
			 * prev was the tail, no deleted_element element to delete
			 * return 0 to show that no delete was possible
			 */
			return 0;
		}

		jr_LListNextElementPtr (list, prev)	= jr_LListNextElementPtr (
														list, deleted_element
													);
	}

	if (deleted_element == jr_LListTailPtr(list)) {
		jr_LListTailPtr (list)		= prev;
	}

	jr_LListMakeNullElement (list, deleted_element);
	/*
	 * so prefix transformations on this element don't affect anything
	 */

	return deleted_element;
}


void jr_LListDeleteNextElement (list, prev)
	jr_LList *			list;
	void *				prev;
{
	void *				el;


	el = jr_LListExtractNextElement (list, prev);

	if (jr_LListAllocatesElements (list)) {
		jr_LListFreeElement (list, el);
	}
}

