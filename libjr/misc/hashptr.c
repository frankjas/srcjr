#include "ezport.h"

#include "jr/misc.h"

jr_int jr_ptrhash (ptr)
	const void *	ptr;
{
	return (jr_int) (intptr_t) ptr;			// 10/1/07: double cast for 64-bit case
}


jr_int jr_ptrcmp (ptr1, ptr2)
	const void *ptr1;
	const void *ptr2;
{
	if (ptr1 > ptr2) {
		return 1;
	}
	if (ptr1 < ptr2) {
		return -1;
	}
	return 0;
}


jr_int jr_ptrptrcmp (pptr1, pptr2)
	const void **	pptr1;
	const void **	pptr2;
{
	const void *	ptr1		= *pptr1;
	const void *	ptr2		= *pptr2;

	if (ptr1 > ptr2) {
		return 1;
	}
	if (ptr1 < ptr2) {
		return -1;
	}
	return 0;
}
