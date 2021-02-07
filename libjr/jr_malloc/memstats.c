#include "project.h"


jr_int jr_MallocMaxPrintBucketVar = 10;

static unsigned jr_int InitialBucketSizes [MAX_MALLOC_BUCKETS + 1] = 
	MALLOC_BUCKET_SIZES;

void jr_MallocStatInit (msp)
	jr_MallocStatStruct *msp;
{
	jr_int b;

	memset (msp, 0, sizeof (jr_MallocStatStruct));

	msp->did_initialization = 1 ;

	for (b=0; b < MAX_MALLOC_BUCKETS; b++) {
		msp->s_bucket[b] = InitialBucketSizes [b];
		msp->n_malloc[b] = 0 ;
		msp->n_freedp[b] = 0 ;
		msp->n_maxnum[b] = 0 ;

		if (msp->nbuckets == 0  &&  msp->s_bucket [b] == 0) {
			msp->nbuckets = b - 1;
		}
	}
}

void jr_MallocStatUndo (msp)
	jr_MallocStatStruct *msp;
{
}

void jr_MallocStatReset (msp)
	jr_MallocStatStruct *msp;
{
	jr_MallocStatUndo (msp);
	jr_MallocStatInit (msp);
}

void jr_MallocStatRecordMalloc (msp, mptr, amt_allocated)
	jr_MallocStatStruct *		msp;
	void  *						mptr ;
	unsigned jr_int				amt_allocated;
{

	if (!msp->did_initialization) {
		jr_MallocStatInit (msp);
	}

	/* Record Bucket Statistics */ {
		register jr_int   b ;

		for (b=0; b < MAX_MALLOC_BUCKETS; b++) {
			if (b == msp->nbuckets) {
				/*
				 * add a bucket if the size is bigger than
				 * anything we keep track of
				 */
				jr_MallocStatBucketSize(msp, b) = amt_allocated ;
				msp->nbuckets++ ;
			}
			if (jr_MallocStatBucketSize(msp, b) >= amt_allocated) {
				msp->n_malloc[b]++ ;

				if (msp->n_malloc[b] - msp->n_freedp[b] > msp->n_maxnum[b]) {
					msp->n_maxnum[b] = msp->n_malloc[b] - msp->n_freedp[b] ;
					/*
					 * reset the maximum number of allocated at any one time 
					 * for this bucket
					 */
				}
				break;
			}
		}
	}
	jr_MallocBytesInUse += amt_allocated;

	if (jr_MallocBytesInUse > jr_MallocMaxBytesInUse) {
		jr_MallocMaxBytesInUse = jr_MallocBytesInUse;
	}

}

void jr_MallocStatRecordFree (msp, mptr, amt_allocated)
	jr_MallocStatStruct *	msp;
	const void *			mptr ;
	unsigned jr_int			amt_allocated;
{
	if (!msp->did_initialization) {
		jr_MallocStatInit (msp);
	}

	/* Record Bucket Statistics */ {
		register jr_int b ;

		for (b=0; b < MAX_MALLOC_BUCKETS; b++) {
			if (jr_MallocStatBucketSize(msp, b) >= amt_allocated) {
				msp->n_freedp[b]++ ;
				break;
			}
		}
	}

	jr_MallocBytesInUse -= amt_allocated;
}

void jr_MallocSetBucketSizes (s_bucket)
	unsigned jr_int *s_bucket;
{
	jr_int i;

	for (i=0; s_bucket [i]; i++) {
		InitialBucketSizes [i] = s_bucket [i];
	}
}

void jr_MallocStatAddBucketSize (msp, bucket_size)
	jr_MallocStatStruct *	msp;
	size_t					bucket_size ;
{
	register jr_int b ;
	jr_int position ;

	if (!msp->did_initialization) {
		jr_MallocStatInit (msp);
	}

	if (msp->nbuckets == MAX_MALLOC_BUCKETS) return;

	for (b=0; b < MAX_MALLOC_BUCKETS; b++) {
		if (b == msp->nbuckets) break ;
		if (jr_MallocStatBucketSize(msp, b) >= bucket_size) break ;
	}

	if (jr_MallocStatBucketSize(msp, b) == bucket_size) return;

	position = b ;

	for (b = msp->nbuckets; b > position; b--) {
		msp->s_bucket[b] = msp->s_bucket[b-1] ;
		msp->n_malloc[b] = msp->n_malloc[b-1] ;
		msp->n_freedp[b] = msp->n_freedp[b-1] ;
		msp->n_maxnum[b] = msp->n_maxnum[b-1] ;
	}
	msp->nbuckets++ ;

	msp->s_bucket[position] = bucket_size ;
	msp->n_malloc[position] = 0 ;
	msp->n_freedp[position] = 0 ;
	msp->n_maxnum[position] = 0 ;
}


void jr_MallocStatPrint(wfp, msp, message)
	FILE *					wfp ;
	jr_MallocStatStruct *	msp;
	char *					message;
{
  	register jr_int			b;
	size_t					size ;
  	size_t					total_bytes_free;
	size_t					total_bytes_used;
	size_t					max_bytes_used ;
	jr_int					num_ptrs_still_allocated;
	jr_int					needblank ;

  	fprintf(wfp,"        %s\n",message) ;

	if (!msp->did_initialization) {
		jr_MallocStatInit (msp);
	}

	needblank = 1 ;
	fprintf(wfp,"     size:");

  	for (b = 0; b < jr_MallocMaxPrintBucketVar ; b++) {
		size = jr_MallocStatBucketSize (msp, b);

		if (size >= 1024) {
			if (needblank) {
				putc(' ',wfp) ;
				needblank = 0 ;
			}
			fprintf (wfp, " %3dK", (jr_int) size / 1024);
		}
		else fprintf(wfp, " %4d", (jr_int) size);
  	}

	if (needblank) fprintf(wfp," ") ;
	fprintf(wfp," |  totals   \n") ;

  	fprintf(wfp, "----------");

  	for (b = 0; b < MAX_MALLOC_BUCKETS; b++) {
  		if (b < jr_MallocMaxPrintBucketVar) fprintf(wfp, "-----") ;
  	}

	fprintf(wfp,"--|-----------\n") ;

	max_bytes_used					= 0 ;
	total_bytes_used				= 0 ;
	num_ptrs_still_allocated		= 0;

  	fprintf(wfp, "max. used:");

  	for (b = 0; b < MAX_MALLOC_BUCKETS; b++) {
  		if (b < jr_MallocMaxPrintBucketVar) fprintf(wfp, " %4d", msp->n_maxnum[b]);

  		max_bytes_used		+= msp->n_maxnum[b] * jr_MallocStatBucketSize (msp, b);
		total_bytes_used	+= msp->n_malloc[b] * jr_MallocStatBucketSize (msp, b); 
  	}
	if (max_bytes_used >= 1024) {
		fprintf (wfp, "  | %4dK+%4d\n", (jr_int) max_bytes_used / 1024, (jr_int) max_bytes_used % 1024);
	}
	else {
		fprintf(wfp, "  | %4d      \n", (jr_int) max_bytes_used);
	}

	total_bytes_free = 0 ;
	fprintf(wfp,"tot. call:") ;

  	for (b = 0; b < MAX_MALLOC_BUCKETS; b++) {
  		if (b < jr_MallocMaxPrintBucketVar) {
			fprintf(wfp, " %4d", msp->n_freedp[b]);
		}
  		total_bytes_free += msp->n_freedp[b] * jr_MallocStatBucketSize (msp, b);
  	}
	if (total_bytes_free >= 1024) {
		fprintf (wfp, "  | %4dK+%4d\n", (jr_int) total_bytes_free / 1024, (jr_int) total_bytes_free % 1024);
	}
	else {
		fprintf(wfp, "  | %4d      \n", (jr_int) total_bytes_free);
	}

  	fprintf(wfp, "----------");
  	for (b = 0; b < MAX_MALLOC_BUCKETS; b++) {
  		if (b < jr_MallocMaxPrintBucketVar) {
			fprintf(wfp, "-----") ;
		}
  	}
	fprintf(wfp,"--|-----------\n") ;

	fprintf(wfp,"cur. used:") ;
  	for (b = 0; b < MAX_MALLOC_BUCKETS; b++) {
  		if (b < jr_MallocMaxPrintBucketVar) {
			fprintf(wfp, " %4d", msp->n_malloc[b] - msp->n_freedp[b]) ;
		}
		num_ptrs_still_allocated += msp->n_malloc[b] - msp->n_freedp[b];
  	}
	total_bytes_used -= total_bytes_free ;
	if (total_bytes_used >= 1024) {
		fprintf (wfp, "  | %4dK+%4d\n", (jr_int) total_bytes_used / 1024, (jr_int) total_bytes_used % 1024);
	}
	else {
		fprintf(wfp, "  | %4d      \n", (jr_int) total_bytes_used);
	}

	fprintf (wfp,"\n%d pointers still allocated\n", num_ptrs_still_allocated);

	if (jr_do_trace(jr_malloc_calls)  ||  jr_do_trace(jr_malloc_trap)) {
		fprintf(wfp,"\t(expect  ~3  jr_malloc_calls book-keeping pointers)\n") ;
	}
}


jr_int jr_MallocStatNumAllocated (msp)
	jr_MallocStatStruct *	msp;
{
	jr_int					num_ptrs_still_allocated			= 0;
	jr_int					b;


	if (!msp->did_initialization) {
		jr_MallocStatInit (msp);
	}

  	for (b = 0;  b < MAX_MALLOC_BUCKETS;  b++) {
		num_ptrs_still_allocated += msp->n_malloc[b] - msp->n_freedp[b];
  	}

	return num_ptrs_still_allocated;
}

