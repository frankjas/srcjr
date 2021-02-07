#include "ezport.h"

#include <stdio.h>
#include <ctype.h>

#include "jr/misc.h"

/*
 * Our convention is that if the search results in a ptr
 * which is really the non-zero place where the key SHOULD
 * be inserted even if it isn't there, then the function
 * should have 'InsertPtr' as part of its name.
 *
 * A simple Search function should return a pointer which is also
 * a boolean value that says whether the thing is in the list or not.
 * The simple Search functions are generally layered on top of the
 * more general SearchInsertPtr functions.
 */

void *	jr_BinarySearch(base_arg, num_items, sizeof_each_item, compf, search_key)
	const void *	base_arg ;
	jr_int			num_items ;
	jr_int			sizeof_each_item ;
	jr_int			(*compf)() ;
	const void *	search_key ;
{
	const char *	base = (const char *) base_arg ;
	char *			insertion_ptr ;
	jr_int			index ;

	insertion_ptr = jr_BinarySearchInsertPtr(
							base_arg,
							num_items,
							sizeof_each_item,
							compf,
							search_key
					) ;
	
	index = (insertion_ptr - base) / sizeof_each_item ;
	if (index < num_items) {
		if ((*compf)(search_key, insertion_ptr) == 0) {
			return((void *) insertion_ptr) ;
		}
	}

	return(0) ;
}

void *	jr_BinarySearchInsertPtr(base_arg, num_items, sizeof_each_item, compf, search_key)
	const void *	base_arg ;
	jr_int			num_items ;
	jr_int			sizeof_each_item ;
	jr_int			(*compf)() ;
	const void *	search_key ;
{
	const char *	base = (const char *) base_arg ;
	jr_int			compval ;
	jr_int			mid_index ;
	jr_int			byte_offset ;
	jr_int			low_index ;
	jr_int			high_index ;

	low_index = 0;
	high_index = num_items ;

	while (low_index != high_index) {
		mid_index		= low_index + (high_index - low_index)/2;
		byte_offset		= mid_index * sizeof_each_item ;

		compval = (*compf) (search_key, base + byte_offset) ;

		if (compval == 0) {
			return((void *)(base + byte_offset)) ;
		}

		if (compval > 0) {
			low_index = mid_index + 1;
		}
		else {
			high_index = mid_index ;
		}
	}

	byte_offset = low_index * sizeof_each_item ;
	return ((void *)(base + byte_offset));
}
