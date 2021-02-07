#include "ezport.h"

#include "jr/intset.h"
#include "jr/malloc.h"

/*
	Author: Frank Jas
	Date: Febuary 17, 1987
*/

jr_IntSet *jr_IntSetCreate ()
{
	jr_IntSet *iset = jr_malloc (sizeof (jr_IntSet));

	jr_IntSetInit (iset);
	return (iset);
}

void jr_IntSetInit (iset)
	jr_IntSet *iset;
{
	iset->array = 0;
	iset->next_index = 0;
	iset->max_size = 0;
}

void jr_IntSetDestroy (iset)
	jr_IntSet *iset;
{
	if (iset->array) jr_free (iset->array);
	jr_free (iset);
}

void jr_IntSetUndo (iset)
	jr_IntSet *iset;
{
	if (iset->array) jr_free (iset->array);
}

void jr_IntSetIncreaseSize (iset, minelements)
	jr_IntSet *		iset;
	jr_int			minelements;
{
	jr_int			new_bytes;

	if (minelements < iset->max_size) return;

	if (minelements < 2 * iset->max_size) {
		minelements = 2 * iset->max_size;
	}

	new_bytes		= minelements * sizeof (jr_int);

	if (iset->array) {
		iset->array = jr_realloc (iset->array, new_bytes);
	}
	else {
		iset->array = jr_malloc (new_bytes);
	}

	iset->max_size = (jr_int) jr_malloc_usable_size (iset->array, new_bytes) / sizeof(jr_int);

	iset->max_size = minelements;
}

int jr_IntSetFind (iset, n)
	jr_IntSet *iset;
	jr_int n;
{
	jr_int i = jr_IntSetFindIndex(iset, n);

	if (i == jr_IntSetSize(iset)) return (0);

	if (jr_IntSetElement(iset, i) == n) return (1);
	return (0);
}

int jr_IntSetCmp (iseta, isetb)
	jr_IntSet *iseta, *isetb;
{
	jr_int i;

	for (i=0; i < jr_IntSetSize(iseta); i++) {
		if (iseta->array[i] != isetb->array[i]) return (1);
	}
	if (i == jr_IntSetSize(isetb)) return (0);
	return (1);
}

void jr_IntSetAddMax (iset, n)
	jr_IntSet *iset;
	jr_int n;
{
	if (iset->next_index == iset->max_size) {
		/* the number of elements in it equals the maximum number */
		jr_IntSetIncreaseSize (iset, iset->max_size + 1);
	}
	jr_IntSetElement(iset, iset->next_index) = n;
	iset->next_index ++;
}
	
void jr_IntSetAdd (iset, n)
	jr_IntSet *iset;
	jr_int n;
{
	jr_int j, i = jr_IntSetFindIndex (iset, n);

	if (i == jr_IntSetSize(iset)) {
		jr_IntSetAddMax (iset, n);
		return;
	}

	if (jr_IntSetElement(iset, i) == n) return;
	/* if its already there, do nothing */

	/* make space for the element */
	if (iset->next_index == iset->max_size) {
		/* the number of elements in iset equals the maximum number */
		jr_IntSetIncreaseSize (iset, iset->max_size + 1);
	}

	for (j=iset->next_index - 1; j >= i; j--) {
		jr_IntSetElement(iset, j+1) = jr_IntSetElement(iset, j);
	}
	jr_IntSetElement(iset, i) = n;
	iset->next_index ++;
}

void jr_IntSetDelete (iset, n)
	jr_IntSet *iset;
	jr_int n;
{
	jr_int i = jr_IntSetFindIndex (iset, n);

	if (i == jr_IntSetSize(iset)) return ;

	if (jr_IntSetElement(iset, i) != n) return;
		/* if it's not there, do nothing */

	for (; i < iset->next_index - 1; i++) {
		jr_IntSetElement(iset, i) = jr_IntSetElement(iset, i+1);
	}
	iset->next_index--;
}

int jr_IntSetFindIndex (iset, n)
	jr_IntSet *iset;
	jr_int n;
				/*
				high is either off the end, or mid
				from the last iteration. This means 
				high shouldn't be the comparison key.
				So if low and high are 0 apart, mid gets low.  
				*/
{
	jr_int mid, low, high;
	register jr_int *array = jr_IntSetArray(iset);

	/* do binary search */
	low = 0;
	high = jr_IntSetSize(iset);
	while (low != high) {
		mid = low + (high - low)/2;

		if (n < array[mid]) high = mid;
		else if (n > array[mid]) low = mid + 1;
		else {
			/* found match */
			return (mid);
		}
	}
	return (low);
}

void jr_IntSetAddSet (iseta, isetb)
	jr_IntSet *iseta, *isetb;
{
	jr_IntSet *isetc = jr_IntSetCreate ();

	jr_IntSetUnion (isetc, iseta, isetb);
	jr_free (iseta->array);
	iseta->array = isetc->array;
	iseta->next_index = isetc->next_index;
	iseta->max_size = isetc->max_size;
	jr_free (isetc);
}

void jr_IntSetUnion (isetc, iseta, isetb)
	jr_IntSet *iseta, *isetb, *isetc;
{
	jr_int a, b, comp;
	jr_int size_a, size_b;

	jr_IntSetEmpty(isetc);

	a = b = 0;
	size_a = jr_IntSetSize(iseta);
	size_b = jr_IntSetSize(isetb);
	while (a < size_a && b < size_b) {
		comp = iseta->array[a] - isetb->array[b];
		if (comp < 0) {
			jr_IntSetAddMax (isetc, iseta->array[a]);
			a++;
		}
		else if (comp > 0) {
			jr_IntSetAddMax (isetc, isetb->array[b]);
			b++;
		}
		else {
			jr_IntSetAddMax (isetc, iseta->array[a]);
			a++;
			b++;
		}
	}

	if (a == size_a) {
		for (; b < size_b; b++) {
			jr_IntSetAddMax (isetc, isetb->array[b]);
		}
	}
	else if (b == size_b) {
		for (; a < size_a; a++) {
			jr_IntSetAddMax (isetc, iseta->array[a]);
		}
	}
}

void jr_IntSetIntersection (isetc, iseta, isetb)
	jr_IntSet *iseta, *isetb, *isetc;
{
	jr_int a, b;

	jr_IntSetEmpty(isetc);

	a = b = 0;
	while (a < jr_IntSetSize(iseta) && b < jr_IntSetSize(isetb)) {
		if (iseta->array[a] < isetb->array[b]) a++;
		else if (iseta->array[a] > isetb->array[b]) b++;
		else { /* they're equal */
			jr_IntSetAddMax (isetc, iseta->array[a++]);
			b++;
		}
	}
}

#ifdef INT_STRING_CONVERSIONS

int *jr_IntSetToIntStr (iset)
	jr_IntSet *iset;
{
	jr_int *tp;

	jr_IntSetAddMax (iset, jr_INT_NULL);
	tp = jr_memdup (jr_IntSetArray(iset), iset->next_index * sizeof(jr_int));
	iset->next_index --;
	/* delete the intnull from it */
	return (tp);
}

jr_IntSet *jr_IntSetCreateFromIntStr (ia)
	jr_int *ia;
{
	jr_IntSet *iset = jr_IntSetCreate ();
	jr_int min, last, i, next, size, *tp;

	for (size=0; ia[size] != jr_INT_NULL; size++);
	tp = (jr_int *) jr_malloc (size * sizeof(jr_int));
	for (i=0; i < size; i++) tp[i] = ia[i];
	ia = tp;
	  /* make a copy of the ia. Could sort into tp, but
	  would take approx. (n^2)/2 times longer, since that
	  would be n^2 and selection is (n^2)/2 */

	next = 0;
	while (next < size) {
		min = next;
		for (i = min+1; i < size; i++) {
			if (ia[i] < ia[min]) min = i;
		}
		if (next == 0 || ia[min] > ia[next-1]) {
		  /* if there are duplicates then we only add min if
		  it is greater than the last one added */
			i = ia[next];
			ia[next] = ia[min];
			next++;
		}
		else { /* min is a duplicate */
			size--;
			i = ia[size];
			ia[size] = ia[min];
			  /* put min in the end slot, this discards it
			  since size is deccremented */
		}
		ia[min] = i;
	}

	jr_IntSetIncreaseSize (iset, next);
	for (i=0; i < next; i++) jr_IntSetElement(iset, i) = ia[i];
	iset->next_index = next;
	jr_free(tp) ;
	return (iset);
}
#endif
