#include "project.h"

jr_int jr_LListCmp (list1, list2, cmpfn)
	jr_LList *list1, *list2;
	jr_int (*cmpfn) ();
{
	char *obj_p1, *obj_p2;
	jr_int comp;

	obj_p1 = jr_LListHeadPtr (list1);
	obj_p2 = jr_LListHeadPtr (list2);

	while (obj_p1 && obj_p2) {
		comp = (*cmpfn) (obj_p1, obj_p2);
		if (comp != 0) {
			return (comp);
		}

		obj_p1 = jr_LListNextElementPtr (list1, obj_p1);
		obj_p2 = jr_LListNextElementPtr (list2, obj_p2);
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

jr_int jr_LListHash (list, hashfn)
	jr_LList *		list;
	jr_int			(*hashfn) ();
{
	jr_int			hash_value			= 0;
	void *			el_ptr;


	jr_LListForEachElementPtr (list, el_ptr) {
		hash_value += hashfn (el_ptr);
	}

	return hash_value;
}
