#include "project.h"

void jr_HTableDeleteIndex (htable, delete_index)
	jr_HTable *		htable;
	jr_int			delete_index;
{
	assert (jr_HTableIsValidIndex (htable, delete_index));
	/*
	 * do an assert since deleting an invalid index could create
	 * a htable with weird bucket lists which would cause
	 * error symptoms difficult to backtrack to a bad delete
	 */

	jr_HTableDeleteFromBucketList (htable, delete_index);
	jr_HTableAddToDeleteList (htable, delete_index);

	jr_HTableHashValue (htable, delete_index) = -1;
	/*
	 * reset the hash value to indicate the element is deleted.
	 * the hash value is used when deleting from the bucket list
	 */
}


void *jr_HTableDeleteElement (htable, el_ptr)
	jr_HTable *				htable;
	void *					el_ptr;
{
	jr_int					index;

	index = jr_HTableFindElementIndex (htable, el_ptr);

	if (index < 0) {
		return 0;
	}

	jr_HTableDeleteIndex (htable, index);

	return jr_HTableElementPtr (htable, index);
}


void *jr_HTableDeletePtrValue (htable, el_ptr)
	jr_HTable *				htable;
	void *					el_ptr;
{
	jr_int					index;

	index = jr_HTableFindPtrValueIndex (htable, el_ptr);

	if (index < 0) {
		return 0;
	}

	jr_HTableDeleteIndex (htable, index);

	return jr_HTableKeyPtrValue (htable, index);
}



void jr_HTableDeleteFromBucketList (htable, delete_index)
	jr_HTable *		htable;
	jr_int			delete_index;
{
	jr_int			hash_value;
	jr_int			bucket;
	jr_int			curr_index;
	jr_int			prev_index;


	if (htable->num_buckets == 0) {
		return;
	}

	hash_value = jr_HTableHashValue (htable, delete_index);

	/*
	 * need to know the index of the previous
	 * previous element in the bucket list
	 * in order to delete from the bucket list
	 */
	bucket		= hash_value % htable->num_buckets;
	curr_index	= htable->buckets_array[bucket];
	prev_index	= curr_index;

	while (curr_index != -1) {
		if (curr_index == delete_index) {
			break;
		}
		prev_index = curr_index;
		curr_index = jr_HTableNextElementIndex (htable, curr_index);
	}

	if (prev_index == curr_index) {
		htable->buckets_array[bucket]
			= jr_HTableNextElementIndex (htable, curr_index);
	}
	else {
		jr_HTableNextElementIndex (htable, prev_index)
			= jr_HTableNextElementIndex (htable, curr_index);
	}
}


void jr_HTableAddToDeleteList (htable, delete_index)
	jr_HTable *		htable;
	jr_int			delete_index;
{
	jr_HTableNextElementIndex (htable, delete_index) = htable->delete_list;
	htable->delete_list = delete_index;

	htable->num_deletes ++;
}
