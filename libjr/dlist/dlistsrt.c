#include "project.h"

void *jr_DListFindInsertPtr (list, obj, cmpfn)
	jr_DList *	list;
	void *		obj;
	jr_int		(*cmpfn) ();
{
	char *		current;

	jr_DListForEachElementPtr (list, current) {
		if ((*cmpfn) (obj, current) <= 0) return (current);
	}
	return (current);
}

void *jr_DListNewElementPtrInOrderWithSizeAndUniqueFlag (list, obj, cmpfn, size, unique)
	jr_DList *	list;
	void *		obj;
	jr_int		(*cmpfn) ();
	jr_int		size;
	jr_int		unique;
{
	void *		next;
	void *		new;


	next = jr_DListFindInsertPtr (list, obj, cmpfn);

	if (next && unique) {
		if ((*cmpfn) (obj, next)  ==  0) return (next);
		/* next is the same as obj */
	}

	new = (void *) jr_DListNewPrevElementPtrWithSize (list, next, size);

	return (new);
}

