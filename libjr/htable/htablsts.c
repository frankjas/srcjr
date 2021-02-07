#include "project.h"

void jr_HTableHashDist (htable, wfp, heading_string)
	jr_HTable *		htable;
	FILE *			wfp;
	char *			heading_string;
{
	jr_int *			hashdist;
	jr_int			allzeros;
	jr_int			bucket;
	jr_int			curr_index;
	jr_int			num_elements;
	jr_int			i;


	hashdist = (jr_int *) jr_malloc (htable->num_buckets * sizeof(jr_int));

	for (bucket=0; bucket < htable->num_buckets; bucket++) {
		curr_index = htable->buckets_array[bucket];

		for (num_elements = 0; curr_index != -1; num_elements++)  {
			curr_index = jr_HTableNextElementIndex (htable, curr_index);
		}
		hashdist[bucket] = num_elements;
	}

	/*
	 * print  50 elements on a line
	 */
	for (bucket=0; bucket < htable->num_buckets; bucket += 50) {

		fprintf (wfp, "         __________________%s______________________\n", heading_string);
		fprintf (wfp, "%3d      >0123456789-123456789-123456789-123456789-123456789<      %3d\n",
			bucket, bucket+49
		);

		allzeros = 0;
		/* print out a histogram sideways
		 * keep printing lines until one line has no stars on it
		 */
		while (!allzeros) {
			fprintf (wfp, "          ");

			for (i=0, allzeros = 1; i < 50; i++) {

				if (bucket+i < htable->num_buckets && hashdist[bucket+i] > 0) {
					allzeros = 0;
					putc('*', wfp);
					hashdist[bucket+i]--;
				}
				else putc(' ', wfp);
			}
			putc('\n', wfp);
		}
	}
	jr_free (hashdist);
}
