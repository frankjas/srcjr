#include "project.h"

void jr_LListMergeWithUniqueFlag (list1, list2, cmpfn, unique)
	jr_LList *	list1;
	jr_LList *	list2;
	jr_int		(*cmpfn) ();
	jr_int		unique;
{
	void *		prev1;
	void *		curr1;
	void *		curr2;
	void *		new;
	jr_int		comp;

	prev1 = 0;
	curr1 = jr_LListHeadPtr (list1);
	curr2 = jr_LListHeadPtr (list2);

	while (curr2 && curr1) {
		comp = (*cmpfn) (curr1, curr2);

		if (comp < 0) {
			prev1 = curr1;
			curr1 = jr_LListNextElementPtr (list1, curr1);
			continue;
		}
		if (comp == 0  &&  unique) {
			curr2 = jr_LListNextElementPtr (list2, curr2);
			continue;
		}

		new = jr_LListAllocateElementWithSize (list2, jr_LListElementSize (list2));
		memcpy (new, curr2, jr_LListElementSize (list2));

		/* insert new before curr1 
		 * the tail is either curr1 or after curr1
		 * so we don't have to change it
		 */

		if (prev1 == 0) {
			/* make new head */
			jr_LListNextElementPtr (list1, new)	= jr_LListHeadPtr (list1);
			jr_LListHeadPtr (list1)				= new;
		}
		else {
			jr_LListNextElementPtr (list1, new)	= jr_LListNextElementPtr (list1, prev1);
			jr_LListNextElementPtr (list1, prev1)	= new;
		}
		prev1 = new;
		curr2 = jr_LListNextElementPtr (list2, curr2);
	}

	if (curr2 == 0) return ;
	/* ran out of the second list */


	if (jr_LListIsEmpty (list1)) {
		jr_LListSetNewTail (list1, curr2);

		curr2 = jr_LListNextElementPtr (list2, curr2);
	}

	for (; curr2; curr2 = jr_LListNextElementPtr (list2, curr2)) {
		if (!unique) {
			jr_LListSetNewTail (list1, curr2);
			continue;
		}

		comp = (*cmpfn) (jr_LListTailPtr(list1), curr2);

		if (comp == 0) continue;

		jr_LListSetNewTail (list1, curr2);
	}
}

