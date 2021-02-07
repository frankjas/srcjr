#include "project.h"

void *jr_LListFindElementPtr (list, obj, cmpfn)
	jr_LList *list;
	void *obj;
	jr_int (*cmpfn) ();
{
	void *lep;

	jr_LListForEachElementPtr (list, lep) {
		
		if (! (*cmpfn) (obj, lep)) return (lep);
	}
	return (0);
}

void *jr_LListFindAppendPtr (list, obj, cmpfn)
	jr_LList *list;
	void *obj;
	jr_int (*cmpfn) ();
{
	void *last, *current;
	jr_int cmp;

	last = 0;

	jr_LListForEachElementPtr (list, current) {
		cmp = (*cmpfn) (obj, current);

		if (cmp <= 0) {
			return last;
		}

		last = current;
	}
	return (last);
}

void *jr_LListNewElementPtrInOrderWithSizeAndUniqueFlag (list, obj, cmpfn, size, unique)
	jr_LList *		list;
	void *			obj;
	jr_int			(*cmpfn) ();
	jr_int			size;
	jr_int			unique;
{
	void *prev, *new, *next;

	prev = jr_LListFindAppendPtr (list, obj, cmpfn);

	if (unique && !jr_LListIsEmpty(list)) {
		if (prev == 0) {
			next = jr_LListHeadPtr(list);
		}
		else {
			next = jr_LListNextElementPtr (list, prev);
		}
		if (next) {
			if ((*cmpfn) (obj, next) == 0) return 0;
			/* next is the same as obj */
		}
	}

	new = jr_LListNewNextElementPtrWithSize (list, prev, size);

	return (new);
}

void *jr_LListSetNewElementUniquely (list, obj, cmpfn)
	jr_LList *		list;
	void *			obj;
	jr_int			(*cmpfn) ();
{
	void *			new;

	new = jr_LListNewElementPtrUniquely (list, obj, cmpfn);

	if (new) {
		memcpy (new, obj, jr_LListElementSize (list));
	}

	return new;
}
