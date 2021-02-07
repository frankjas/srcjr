#include "project.h"

jr_DList *jr_DListCreate (objsize)
	jr_int objsize;
{
	jr_DList *ptr = (jr_DList *) jr_malloc (sizeof (jr_DList));

	jr_DListInit (ptr, objsize);
	return (ptr);
}

void jr_DListInit (list, objsize)
	jr_DList *			list;
	jr_int				objsize;
{
	static jr_int		added_prefix_info		= 0;

	if (!added_prefix_info) {
		added_prefix_info		= 1;

		jr_PrefixAddContainerType (
			"jr_DList",
			jr_DListElementPrefixSize,
			jr_DListTransformPrefix,
			jr_DListSetPrefixSize,
			0
		);
	}

	list->tail					= 0;
	list->head					= 0;
	list->objsize				= objsize;
	list->prefix_size			= 0;
	list->allocates_elements	= 0;
}

void jr_DListDestroy (list)
	jr_DList *list;
{
	jr_DListEmpty (list);
	jr_free (list);
}

void jr_DListUndo (list)
	jr_DList *list;
{
	jr_DListEmpty (list);
}

void jr_DListEmpty (list)
	jr_DList *list;
{
	if (jr_DListAllocatesElements (list)) {
		void *		lep;
		void *		next ;

		lep = jr_DListHeadPtr(list) ;
		while (lep) {
			next = jr_DListNextElementPtr (list,lep) ;
			jr_DListFreeElement (list, lep);
			lep = next ;
		}
	}
	list->head = 0;
	list->tail = 0;
}

jr_int jr_DListElementPrefixSize (list)
	jr_DList *		list;
{
	return 2 * sizeof (void *);
}
						
void jr_DListSetPrefixSize (list, prefix_size)
	jr_DList *		list;
	jr_int			prefix_size;
{
	assert (jr_DListIsEmpty (list));

	list->prefix_size	= prefix_size;
}


void *jr_DListAllocateElementWithSize (list, size)
	jr_DList *		list;
	jr_int			size;
{
	char *			new;

	list->allocates_elements = 1;

	new = jr_malloc (list->prefix_size + 2 * sizeof (void *) + size);

	return new + list->prefix_size + 2 * sizeof (void *);
}


void *jr_DListAllocateTmpElementWithSize (list, size)
	jr_DList *		list;
	jr_int			size;
{
	void *			new;
	jr_int			old_allocates_elements;

	old_allocates_elements	= list->allocates_elements;

	new = jr_DListAllocateElementWithSize (list, size);
	
	if (old_allocates_elements == 0) {
		list->allocates_elements = 0;
	}

	return new;
}


void jr_DListFreeElement (list, obj_ptr)
	jr_DList *		list;
	void *			obj_ptr;
{
	if (obj_ptr  &&  list->allocates_elements) {
		jr_FreeDListElementWithPrefix (obj_ptr, list->prefix_size);
	}
}


void *_jr_DListNextElementPtr (list, obj_ptr)
	jr_DList *		list;
	void *			obj_ptr;
{
	return jr_DListNextElementPtr (list, obj_ptr);
}

void *_jr_DListPrevElementPtr (list, obj_ptr)
	jr_DList *		list;
	void *			obj_ptr;
{
	return jr_DListPrevElementPtr (list, obj_ptr);
}

