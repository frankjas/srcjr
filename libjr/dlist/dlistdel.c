#include "project.h"

void *jr_DListExtractElement (list, el)
	jr_DList *		list;
	const void *	el;
{
	void *			next = jr_DListNextElementPtr (list, el);
	void *			prev = jr_DListPrevElementPtr (list, el);

	if (next) {
		jr_DListPrevElementPtr (list, next) = prev;
	}
	else if (jr_DListTailPtr (list) == el) {
		jr_DListTailPtr(list) = prev;
	}
	/*
	** 8/15/2005: else it wasn't on this list, extra check
	** lets us delete an null element without effect.
	*/

	if (prev) {
		jr_DListNextElementPtr (list, prev) = next;
	}
	else if (jr_DListHeadPtr (list) == el) {
		jr_DListHeadPtr(list) = next;
	}
	/*
	** 8/15/2005: else it wasn't on this list, extra check
	** lets us delete an null element without effect.
	*/

	jr_DListMakeNullElement (list, el);
	/*
	 * so prefix transformations on this element don't affect anything
	 */

	return (void *) el;
}

void jr_DListDeleteElement (list, el)
	jr_DList *			list;
	void *				el;
{
	jr_DListExtractElement (list, el);

	if (jr_DListAllocatesElements (list)) {
		jr_DListFreeElement (list, el);
	}
}

