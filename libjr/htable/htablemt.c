#include "project.h"

void jr_HTableEmpty (htable)
	jr_HTable *	htable ;
{
	jr_int			i;

	for (i = 0;  i < htable->num_buckets;  i++) {
		htable->buckets_array[i]		= -1;
	}

	jr_AListEmpty (htable->entry_array);

	htable->delete_list = -1 ;			/* end-of-list marker */
	htable->num_deletes = 0 ;
}
