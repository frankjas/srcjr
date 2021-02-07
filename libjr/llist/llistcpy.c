#include "project.h"

jr_LList *jr_LListCopy (dest, src)
	jr_LList *dest;
	jr_LList *src;
{
	void *obj;

	jr_LListInit (dest, src->objsize);
	jr_LListSetPrefixSize (dest, jr_LListPrefixSize (src));

	jr_LListForEachElementPtr (src, obj) {
		jr_LListSetNewTail(dest, obj);
	}

	return (dest);
}
		
