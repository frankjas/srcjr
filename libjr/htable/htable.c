#include "project.h"

jr_HTable *jr_HTableCreate (element_size, hashfn, cmpfn)
	jr_int		element_size;
	jr_int		(*hashfn) PROTO ((const void *));
	jr_int		(*cmpfn) PROTO ((const void *, const void *));
{
	jr_HTable *htable = (jr_HTable *) jr_malloc (sizeof (jr_HTable));

	jr_HTableInit (htable, element_size, hashfn, cmpfn);
	return(htable) ;
}

void jr_HTableInit (htable, element_size, hashfn, cmpfn)
	jr_HTable *	htable;
	jr_int		element_size;
	jr_int		(*hashfn) PROTO ((const void *));
	jr_int		(*cmpfn) PROTO ((const void *, const void *));
{
	htable->element_size		= element_size;

	htable->hashfn				= hashfn;
	htable->cmpfn				= (jr_int (*) (const void *, const void *, ...)) cmpfn;
	htable->cmp_arg				= 0;

	htable->buckets_array		= 0;
	htable->num_buckets			= 0;

	htable->delete_list			= -1;		/* end-of-list marker */
	htable->num_deletes			= 0;

	jr_AListInit (htable->entry_array, element_size);

	jr_AListSetPrefixSize (htable->entry_array, sizeof (jr_HTableElementStruct), 0, 0);
}

void jr_HTableDestroy (htable)
	jr_HTable *	htable;
{
	jr_HTableUndo (htable);
	jr_free (htable);
}

void jr_HTableUndo (htable)
	jr_HTable *		htable;
{
	jr_AListUndo (htable->entry_array);

	if (htable->buckets_array) {
		jr_free (htable->buckets_array);
	}
}

void jr_HTableSetPrefixSize (htable, prefix_size, prefix_transform_fn, prefix_transform_arg)
	jr_HTable *		htable;
	jr_int			prefix_size;
	void			(*prefix_transform_fn) ();
	void *			prefix_transform_arg;
{
	jr_AListSetPrefixSize (
		htable->entry_array,
		prefix_size + sizeof (jr_HTableElementStruct),
		prefix_transform_fn, prefix_transform_arg
	);
}

static void *_jr_HTableElementPtr (htable, i)
	jr_HTable *		htable;
	jr_int			i;
{
	if (0) {
		_jr_HTableElementPtr (htable, i);
	}
	return jr_HTableElementPtr (htable, i);
}
