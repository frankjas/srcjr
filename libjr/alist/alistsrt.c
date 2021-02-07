#include "project.h"

void jr_AListSortRange (list, start_index, num_elements, cmpfn)
	jr_AList *			list;
	jr_int				start_index;
	jr_int				num_elements;
	jr_int				(*cmpfn) ();
	/*
	 * using a selection sort since the most 
	 * time-consuming operation is the swap 
	 */
{
	register char *		currptr;
	register char *		minptr;
	register char *		tailptr;
	register jr_int		objsize			= list->objsize;
	char *				switchptr;
	char *				tptr ;
	jr_int				diff;
	unsigned jr_int		magnitude;
	jr_int				which_way;
	
	if (start_index + num_elements > jr_AListSize (list)) {
		num_elements = jr_AListSize(list) - start_index;
	}
	if (num_elements <= 1) return ;

	tptr		= jr_malloc (objsize);

	switchptr = jr_AListElementPtr (list, start_index);
	tailptr   = jr_AListElementPtr (list, start_index + num_elements - 1);

	/*
	 * ignore the prefix stuff when switching
	 */

	for (;  switchptr < tailptr;  switchptr += objsize) {

		minptr = switchptr;

		for (currptr = minptr + objsize;  currptr <= tailptr;  currptr += objsize) {
			diff = (*cmpfn) (currptr + list->prefix_size, minptr + list->prefix_size);
			if (diff < 0) {
				minptr = currptr;
			}
		}

		if (minptr == switchptr) continue;


		memcpy (tptr, switchptr, objsize);

		if (jr_AListHasPrefixTransformFn (list)) {
			if (tptr > switchptr) {
				magnitude	= tptr - switchptr;
				which_way	= 1;
			}
			else {
				magnitude	= switchptr - tptr;
				which_way	= -1;
			}
			(*jr_AListPrefixTransformFn (list)) (
				jr_AListPrefixTransformArg (list),
				tptr + list->prefix_size, magnitude, which_way, 0
			);
		}


		memcpy (switchptr, minptr, objsize);

		if (jr_AListHasPrefixTransformFn (list)) {
			if (switchptr > minptr) {
				magnitude	= switchptr - minptr;
				which_way	= 1;
			}
			else {
				magnitude	= minptr - switchptr;
				which_way	= -1;
			}
			(*jr_AListPrefixTransformFn (list)) (
				jr_AListPrefixTransformArg (list),
				switchptr + list->prefix_size, magnitude, -which_way, 0
			);
		}


		memcpy (minptr, tptr, objsize);

		if (jr_AListHasPrefixTransformFn (list)) {
			if (minptr > tptr) {
				magnitude	= minptr - tptr;
				which_way	= 1;
			}
			else {
				magnitude	= tptr - minptr;
				which_way	= -1;
			}
			(*jr_AListPrefixTransformFn (list)) (
				jr_AListPrefixTransformArg (list),
				minptr + list->prefix_size, magnitude, which_way, 0
			);
		}
	}
	jr_free (tptr);
}


void jr_AListSortUniquelyRangeFreeingDeletes (
							list, start_index, num_elements, cmpfn, freefunc, freefunc_arg
						)
	jr_AList *			list;
	jr_int				start_index;
	jr_int				num_elements;
	jr_int				(*cmpfn) ();
	void				(*freefunc) ();
	void *				freefunc_arg;
{
	register char *		currptr;
	register char *		minptr;
	register char *		tailptr;
	register jr_int		objsize			= list->objsize;
	char *				switchptr;
	char *				startptr;
	char *				tptr;
	jr_int				diff;
	unsigned jr_int		magnitude;
	jr_int				which_way;


	if (start_index + num_elements > jr_AListSize (list)) {
		num_elements = jr_AListSize(list) - start_index;
	}
	if (num_elements <= 1) return ;

	tptr = jr_malloc (objsize);

	switchptr = jr_AListElementPtr (list, start_index);
	startptr  = switchptr;
	tailptr   = jr_AListElementPtr (list, start_index + num_elements - 1);


	for (; switchptr <= tailptr; switchptr += objsize) {
		/* <= since we have to check the tail for 
		 * duplicates, the test minptr == switchptr
		 * will prevent a copy over itself
		 */

		minptr = switchptr;
		for (currptr = minptr + objsize;  currptr <= tailptr;  currptr += objsize) {
			diff = (*cmpfn) (currptr + list->prefix_size, minptr + list->prefix_size);
			if (diff < 0) {
				minptr = currptr;
			}
		}
		/* found the minimum element */

		if (switchptr != startptr) {		/* not the first time through */
			/*
			 * check the min value for duplicates
			 */
			diff = (*cmpfn) (switchptr - objsize + list->prefix_size, minptr + list->prefix_size);

			if (diff == 0) {
				/* 
				 * copy tail of un-sorted portion over duplicate entry, then delete the tail
				 */

				if (freefunc) {
					(*freefunc) (minptr + list->prefix_size, freefunc_arg);
				}

				if (minptr != tailptr) {
					memcpy (minptr, tailptr, objsize);

					if (jr_AListHasPrefixTransformFn (list)) {
						magnitude	= tailptr - minptr;
						which_way	= -1;

						(*jr_AListPrefixTransformFn (list)) (
							jr_AListPrefixTransformArg (list),
							minptr + list->prefix_size, magnitude, which_way, 0
						);
					}
				}

				jr_AListDeleteRangeFromPtr (list, tailptr, 1);

				tailptr		-= objsize;		/* adjust tail down */
				switchptr	-= objsize;		/* decrement switch to offset the increment later */
			}
		}
		else {
			diff = 1;
		}
		if (diff != 0  &&  minptr != switchptr) {

			memcpy (tptr, switchptr, objsize);

			if (jr_AListHasPrefixTransformFn (list)) {
				if (tptr > switchptr) {
					magnitude	= tptr - switchptr;
					which_way	= 1;
				}
				else {
					magnitude	= switchptr - tptr;
					which_way	= -1;
				}
				(*jr_AListPrefixTransformFn (list)) (
					jr_AListPrefixTransformArg (list),
					tptr + list->prefix_size, magnitude, which_way, 0
				);
			}


			memcpy (switchptr, minptr, objsize);

			if (jr_AListHasPrefixTransformFn (list)) {
				if (switchptr > minptr) {
					magnitude	= switchptr - minptr;
					which_way	= 1;
				}
				else {
					magnitude	= minptr - switchptr;
					which_way	= -1;
				}
				(*jr_AListPrefixTransformFn (list)) (
					jr_AListPrefixTransformArg (list),
					switchptr + list->prefix_size, magnitude, -which_way, 0
				);
			}


			memcpy (minptr, tptr, objsize);

			if (jr_AListHasPrefixTransformFn (list)) {
				if (minptr > tptr) {
					magnitude	= minptr - tptr;
					which_way	= 1;
				}
				else {
					magnitude	= tptr - minptr;
					which_way	= -1;
				}
				(*jr_AListPrefixTransformFn (list)) (
					jr_AListPrefixTransformArg (list),
					minptr + list->prefix_size, magnitude, which_way, 0
				);
			}
		}
	}
	jr_free (tptr);
}
