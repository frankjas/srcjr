#include "ezport.h"

#include "jr/misc.h"
#include "jr/malloc.h"

void jr_freeref(sa)
	char **		sa ;
{
	jr_free(*sa) ;
}
