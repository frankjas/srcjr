#include "project.h"

jr_int jr_AListCmp (list1, list2, cmpfn)
		const jr_AList *	list1 ;
		const jr_AList *	list2 ;
		jr_int				(*cmpfn)() ;
{
	jr_int					n1 ;
	jr_int					n2 ;
	jr_int					shortest ;
	jr_int					diff ;
	jr_int					i ;

	void *					elem1 ;
	void *					elem2 ;

	n1 = jr_AListSize(list1) ;
	n2 = jr_AListSize(list2) ;

	shortest = (n1 < n2) ? n1 : n2 ;

	for (i = 0 ; i < shortest ; i++) {
		elem1 = jr_AListElementPtr(list1, i) ;
		elem2 = jr_AListElementPtr(list2, i) ;

		diff = (*cmpfn)(elem1, elem2) ;
		if (diff != 0) {
			return(diff) ;
		}
	}

	return (n1 - n2) ;
}

jr_int jr_AListHash (list, hash_fn)
	const jr_AList *	list;
	jr_int				(*hash_fn) ();
{
	void *				el_ptr;
	jr_int				hash_value		= 0;


	jr_AListForEachElementPtr (list, el_ptr) {
		hash_value	+= (*hash_fn) (el_ptr);
	}

	return hash_value;
}
