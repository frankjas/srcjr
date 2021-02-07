#include "ezport.h"

#include <stdio.h>
#include <ctype.h>

#include "jr/string.h"

void *jr_LinearSearch(base_arg, num_items, sizeof_each_item, compf, key_or_pattern)
	const void *	base_arg ;
	jr_int			num_items ;
	jr_int			sizeof_each_item ;
	jr_int			(*compf)() ;
	const void *	key_or_pattern ;
{
	const char *	base = (const char *) base_arg ;
	jr_int			byte_offset ;
	jr_int			i ;
	jr_int			compval ;

	for (i=0; i < num_items; i++) {
		byte_offset = (i * sizeof_each_item) ;

		compval = (*compf) (key_or_pattern, base + byte_offset) ;
		if (compval == 0) return((void *) (base + byte_offset)) ;
	}

	return(0) ;
}
