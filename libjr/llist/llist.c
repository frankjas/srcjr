#include "project.h"

jr_LList *jr_LListCreate (objsize)
	jr_int objsize;
{
	jr_LList *ptr = (jr_LList *) jr_malloc (sizeof (jr_LList));

	jr_LListInit (ptr, objsize);
	return (ptr);
}

void jr_LListInit (list, objsize)
	jr_LList *list;
	jr_int objsize;
{
	static jr_int		added_prefix_info		= 0;

	if (!added_prefix_info) {
		added_prefix_info		= 1;

		jr_PrefixAddContainerType (
			"jr_LList",
			jr_LListElementPrefixSize,
			jr_LListTransformPrefix,
			jr_LListSetPrefixSize,
			0
		);
	}

	list->head					= 0;
	list->tail					= 0;
	list->objsize				= objsize;
	list->prefix_size			= 0;
	list->allocates_elements	= 0;
}


void jr_LListDestroy (list)
	jr_LList *list;
{
	jr_LListEmpty (list);
	jr_free (list);
}

void jr_LListUndo (list)
	jr_LList *list;
{
	jr_LListEmpty (list);
}

void jr_LListEmpty (list)
	jr_LList *list;
{
	if (jr_LListAllocatesElements (list)) {
		void *		lep;
		void *		next;

		lep = jr_LListHeadPtr(list) ;
		while (lep) {
			next = jr_LListNextElementPtr (list, lep) ;

			jr_LListFreeElement (list, lep);
			lep = next ;
		}
	}
	jr_LListHeadPtr(list) = 0;
	jr_LListTailPtr(list) = 0;
}


jr_int jr_LListElementPrefixSize (list)
	jr_LList *		list;
{
	return sizeof (void *);
}
						

void jr_LListSetPrefixSize (list, prefix_size)
	jr_LList *		list;
	jr_int			prefix_size;
{
	assert (jr_LListIsEmpty (list));

	list->prefix_size	= prefix_size;
}

void *jr_LListAllocateElementWithSize (list, element_size)
	jr_LList *	list;
	jr_int		element_size;
{
	void *		new;
	
	list->allocates_elements	= 1;

	new			= jr_NewLListElementWithPrefix (element_size, list->prefix_size);

	return new;
}


void *jr_NewLListElementWithPrefix (element_size, prefix_size)
	jr_int		element_size;
	jr_int		prefix_size;
{
	void *		new;
	
	new			= jr_malloc (prefix_size + sizeof (void *)  +  element_size);

	return ((char *) new) + prefix_size + sizeof (void *);
}

