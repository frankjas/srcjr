#include "ezport.h"

#include <string.h>

#include "jr/malloc.h"

void *	jr_memdup (p, n)
	const void *	p;
	size_t			n;
{
	void *	np ;
	
	np = (void *) jr_malloc (n);

	memcpy(np, p, n) ;

	return(np) ;
}
