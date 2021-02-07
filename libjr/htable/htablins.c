#include "project.h"

jr_int jr_HTableFindIndexGeneric (htable, key_ptr, is_ptr_value)
	jr_HTable *		htable;
	const void *	key_ptr;
	jr_int			is_ptr_value;
{
	jr_int			hash_value;
	jr_int			bucket;
	jr_int			curr_entry;
	jr_int			diff;


	if (htable->num_buckets == 0) {
		return -1;
	}

	hash_value = (*htable->hashfn) (key_ptr);

	if (hash_value < 0) {
		/* 
		 * can't have negative hash_values
		 */
		hash_value *= -1;
	}

	bucket		= hash_value % htable->num_buckets;
	curr_entry	= htable->buckets_array[bucket];

	while (curr_entry != -1) {
		if (is_ptr_value) {
			diff = (*htable->cmpfn) (
						jr_HTableKeyPtrValue (htable, curr_entry), key_ptr, htable->cmp_arg
					);
		}
		else {
			diff = (*htable->cmpfn) (
						jr_HTableElementPtr (htable, curr_entry), key_ptr, htable->cmp_arg
					);
		}
		if (diff == 0) {
			return curr_entry;
		}
		curr_entry = jr_HTableNextElementIndex(htable, curr_entry);
	}

	return -1;
}

void *jr_HTableFindPtrGeneric (htable, key_ptr, is_ptr_value)
	jr_HTable *		htable;
	const void *	key_ptr;
	jr_int			is_ptr_value;
{
	jr_int			index;

	index = jr_HTableFindIndexGeneric (htable, key_ptr, is_ptr_value);

	if (index == -1) {
		return 0;
	}

	if (is_ptr_value) {
		return jr_HTableKeyPtrValue (htable, index);
	}
	else {
		return jr_HTableElementPtr (htable, index);
	}
}


jr_int jr_HTableNewIndexGeneric (htable, key_ptr, is_ptr_value)
	jr_HTable *		htable;
	const void *	key_ptr;
	jr_int			is_ptr_value;
{
	jr_int			curr_index;
	jr_int			new_index;
	jr_int			hash_value;


	curr_index = jr_HTableFindIndexGeneric (htable, key_ptr, is_ptr_value);

	if (curr_index >= 0) {
		return -1;
	}


	new_index = jr_HTableAllocateNewIndex (htable);

	if (is_ptr_value) {
		jr_HTableKeyPtrValue (htable, new_index)	= (void *) key_ptr;
	}

	hash_value = (*htable->hashfn) (key_ptr);

	if (hash_value < 0) {
		hash_value *= -1;
	}

	jr_HTableHashValue(htable, new_index)			= hash_value;
	jr_HTableNextElementIndex(htable, new_index)	= -1;
	
	jr_HTableAddToBucketList (htable, new_index);

	if (jr_AListSize (htable->entry_array)  >  htable->num_buckets) {
		jr_HTableIncreaseNumBuckets (htable);
	}

	return new_index;
}

void *jr_HTableNewPtrGeneric (htable, key_ptr, is_ptr_value)
	jr_HTable *		htable;
	const void *	key_ptr;
	jr_int			is_ptr_value;
{
	jr_int			index;

	index = jr_HTableNewIndexGeneric (htable, key_ptr, is_ptr_value);

	if (index == -1) {
		return 0;
	}

	if (is_ptr_value) {
		return jr_HTableKeyPtrValue (htable, index);
	}
	else {
		return jr_HTableElementPtr (htable, index);
	}
}


void *jr_HTableSetNewElement (htable, key_ptr)
	jr_HTable *		htable;
	const void *	key_ptr;
{
	void *			new_el_ptr;

	new_el_ptr		= jr_HTableNewElementPtr (htable, key_ptr);

	if (new_el_ptr) {
		memcpy (new_el_ptr, key_ptr, jr_HTableElementSize (htable));
	}

	return new_el_ptr;
}

jr_int jr_HTableSetNewElementIndex (htable, key_ptr)
	jr_HTable *		htable;
	const void *	key_ptr;
{
	jr_int			index;

	index			= jr_HTableNewElementIndex (htable, key_ptr);

	if (index >= 0) {
		memcpy (jr_HTableElementPtr (htable, index), key_ptr, jr_HTableElementSize (htable));
	}

	return index;
}

jr_int jr_HTableAllocateNewIndex (htable)
	jr_HTable *		htable;
{
	jr_int			new_index;

	if (htable->num_deletes) {
		new_index			= htable->delete_list;
		htable->delete_list = jr_HTableNextElementIndex(htable, htable->delete_list);
		htable->num_deletes --;
	}
	else {
		jr_AListNewTailPtr (htable->entry_array);

		new_index = jr_AListTailIndex (htable->entry_array);
	}

	return new_index;
}


#ifdef compile_primes_xxx
/*
 * From PHP's hash table implementation
 */

unsigned jr_int jr_PrimesForDoubling [] =
	{	5, 			11,			19,			53,			107,
		223,		463,		983,		1979,		3907,
		7963,		16229,		32531,		65407,		130987,
		262237,		524521,		1048793,	2097397,	4194103,
		8388857,	16777447,	33554201,	67108961,	134217487,
		268435697,	536870683,	1073741621,	2147483399
	};

unsigned jr_int jr_NumPrimesForDoubling = sizeof(jr_PrimesForDoubling) / sizeof(unsigned jr_int);

#endif


void jr_HTableIncreaseNumBuckets (htable)
	jr_HTable *		htable;
{
	jr_int			i;

	if  (htable->num_buckets >= jr_AListSize (htable->entry_array)) {
		return;
	}
	/* else double the number of entries */

	while  (htable->num_buckets < jr_AListSize (htable->entry_array)) {
		if (htable->num_buckets == 0) {
			htable->num_buckets		= 8 + 1;
		}
		else {
			/*
			 * use a fairly prime number, i.e. a power of 2, + 1, since
			 * more numbers will be relatively prime (i.e. not share any
			 * divisors) and therefore won't hash to the same buckets
			 */
			htable->num_buckets		= 2 * (htable->num_buckets - 1) + 1;
		}
	}

	if (htable->buckets_array) {
		jr_free (htable->buckets_array);
	}

	htable->buckets_array	= jr_malloc (htable->num_buckets * sizeof(jr_int));

	for (i = 0;  i < htable->num_buckets;  i++) {
		htable->buckets_array[i]		= -1;
		/*
		 * initialize each bucket with the end-of-list value, -1
		 */
	}

	jr_HTableForEachElementIndex (htable, i) {
		jr_HTableAddToBucketList (htable, i);
	}
}

void jr_HTableAddToBucketList (htable, index)
	jr_HTable *		htable;
	jr_int			index;
{
	jr_int			hash_value;
	jr_int			bucket;


	if (htable->num_buckets == 0) {
		return;
	}

	hash_value		= jr_HTableHashValue(htable, index);
	bucket			= hash_value % htable->num_buckets;

	jr_HTableNextElementIndex(htable, index)	= htable->buckets_array [bucket];
	htable->buckets_array [bucket]				= index;
}

void jr_HTableIncreaseCapacity (htable, new_capacity)
	jr_HTable *			htable;
	jr_int				new_capacity;
{
	jr_AListIncreaseCapacity (htable->entry_array, new_capacity);
}

static jr_HTableElementStruct *jr_HTableEntryPtr (htable, index)
	jr_HTable *		htable;
	jr_int			index;
{
	if (0) {
		/*
		 * avoid complier non-use error
		 */
		jr_HTableEntryPtr (htable, 0);
	}
	return jr_HTableElementInfoPtrWithPrefix (htable, index, jr_HTablePrefixSize (htable));
}
