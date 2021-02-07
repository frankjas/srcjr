#include "project.h"


void *jr_DListFindElementPtr (list, obj, cmpfn)
	jr_DList *		list;
	void *			obj;
	jr_int			(*cmpfn) ();
{
	char *			current;

	jr_DListForEachElementPtr (list, current) {
		if ((*cmpfn) (obj, current) == 0) return (current);
	}
	return (0);
}


jr_int jr_DListCmp (list1, list2, cmpfn)
	jr_DList *		list1;
	jr_DList *		list2;
	jr_int			(*cmpfn) ();
{
	void *			obj_p1;
	void *			obj_p2;
	jr_int			comp;

	obj_p1 = jr_DListHeadPtr (list1);
	obj_p2 = jr_DListHeadPtr (list2);

	while (obj_p1 && obj_p2) {
		comp = (*cmpfn) (obj_p1, obj_p2);
		if (comp) return (comp);

		obj_p1 = jr_DListNextElementPtr (list1, obj_p1);
		obj_p2 = jr_DListNextElementPtr (list2, obj_p2);
	}
	if (obj_p1 == 0  &&  obj_p2 == 0) {
		return 0;
	}
	if (obj_p1 == 0) {
		return -1;
	}
	if (obj_p2 == 0) {
		return 1;
	}
	return 0;
}

jr_int jr_DListHash (list, hashfn)
	jr_DList *		list;
	jr_int			(*hashfn) ();
{
	void *			element;
	jr_int			hash_value			= 0;


	jr_DListForEachElementPtr (list, element) {
		hash_value += hashfn (element);
	}

	return hash_value;
}
