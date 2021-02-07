#include "project.h"


void *jr_AListFindElementPtr (list, el_ptr, cmpfn) 
	jr_AList *			list;
	const void *		el_ptr;
	jr_int				(*cmpfn)();
{
	jr_int				diff;
	jr_int				mid_index;
	jr_int				low_index;
	jr_int				high_index;


	low_index	= 0;
	high_index	= jr_AListSize (list);

	while (low_index != high_index) {
		mid_index = low_index + (high_index - low_index) / 2;

		diff = (*cmpfn) (el_ptr, jr_AListElementPtr (list, mid_index));

		if (diff == 0) {
			return jr_AListElementPtr (list, mid_index);
		}

		if (diff > 0) {
			low_index	= mid_index + 1;
		}
		else {
			high_index	= mid_index;
		}
	}
	return 0;
}


void *jr_AListFindFirstElementPtr (list, el_ptr, cmpfn) 
	jr_AList *			list;
	const void *		el_ptr;
	jr_int				(*cmpfn)();
{
	const char *		found_el_ptr;

	found_el_ptr		= jr_AListFindElementPtr (list, el_ptr, cmpfn);

	if (found_el_ptr) {

		found_el_ptr	-= jr_AListElementSize (list);

		while ((void *) found_el_ptr  >=  jr_AListHeadPtr (list)) {

			if ((*cmpfn) (el_ptr, found_el_ptr) != 0) {
				break;
			}

			found_el_ptr	-= jr_AListElementSize (list);
		}

		found_el_ptr	+= jr_AListElementSize (list);
	}

	return (void *) found_el_ptr;
}


void *jr_AListFindInsertPtr (list, el_ptr, cmpfn)
	jr_AList *		list;
	const void *	el_ptr;
	jr_int			(*cmpfn) ();
{
	jr_int			diff;
	jr_int			mid_index;
	jr_int			low_index;
	jr_int			high_index;


	if (jr_AListSize (list) == 0) {
		jr_AListIncreaseCapacity (list, 1);

		return jr_AListHeadPtr (list);
	}

	low_index	= 0;
	high_index	= jr_AListSize (list);

	while (low_index != high_index) {
		mid_index = low_index + (high_index - low_index) / 2;

		diff = (*cmpfn) (el_ptr, jr_AListElementPtr (list, mid_index));

		if (diff == 0) {
			return jr_AListElementPtr (list, mid_index);
		}

		if (diff > 0) {
			low_index	= mid_index + 1;
		}
		else {
			high_index	= mid_index;
		}
	}

	return jr_AListElementPtr (list, low_index);
}


void *jr_AListSetNewTailUniquely (list, el_ptr, cmpfn)
	jr_AList *			list;
	const void *		el_ptr;
	jr_int				(*cmpfn) ();
{
	void *				found_el_ptr ;
	
	found_el_ptr		= jr_AListFindElementPtr (list, el_ptr, cmpfn);

	if (found_el_ptr) {
		return 0;
	}

	return jr_AListSetNewTail (list, el_ptr);
}

