#include "ezport.h"

#ifdef ostype_winnt
#include <malloc.h>
#endif

#include "jr/misc.h"

/*
** 6-8-2011: Used by jr_malloc() diagnostics to compare pointers
** from different runs of a program.
*/


#ifdef ostype_winnt

void *jr_heap_base()
{
	return (void *) _get_heap_handle();
}

#else

void *jr_heap_base()
{
	return 0;
}

#endif

