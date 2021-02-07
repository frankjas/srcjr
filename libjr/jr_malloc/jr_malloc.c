#include "project.h"

static jr_MallocStatStruct		jr_MallocStatInfo [1];
static jr_MallocDiagStruct		jr_MallocDiagInfo [1];

static jr_int					jr_MallocClearOnFree				= 1 ;
static unsigned jr_int			jr_MallocMaxRequestSize				= jr_MALLOC_DEFAULT_MAX_REQUEST;
static unsigned jr_int			jr_MallocNumCalls					= 0;
static unsigned jr_int			jr_MallocNumUnRecordedCalls			= 0;
static unsigned jr_int			jr_MallocNumBoundsMarkedPtrs		= 0;


unsigned jr_int					jr_FreeNumCalls						= 0 ;
size_t							jr_MallocBytesInUse					= 0 ;
size_t							jr_MallocMaxBytesInUse				= 0 ;

jr_int							jr_MallocMaxLeakTrapPrint			= jr_MALLOC_MAX_LEAK_TRAP_PRINT;

jr_int							jr_MallocOtherResourceLeak			= 0;

unsigned jr_int					jr_MallocSuffixBoundsMarkerMultiple	= 0 ;
unsigned jr_int					jr_MallocPrefixBoundsMarkerMultiple	= 0 ;

static void						jr_MallocDiagLock (jr_MallocDiagStruct *mdp);
static void						jr_MallocDiagUnLock (jr_MallocDiagStruct *mdp);

static void						jr_MallocStatLock ();
static void						jr_MallocStatUnLock ();


void *jr_malloc (amt_requested)
	size_t				amt_requested ;
{
	void *				new_ptr ;
	size_t				bytes_needed				= amt_requested;
	jr_int				needs_bounds_marker			= 0;


	if (	jr_do_trace (jr_malloc_calls) || jr_do_trace (jr_malloc_trap)
		||	jr_MallocDiagIsDoingBookkeeping (jr_MallocDiagInfo)) {
		/*
		** 4/6/07: if we're recording ptrs (as part of C++ global object init, for ex.)
		** we wind up doing bookkeeping pointers, which always have bounds markers.
		** One bounds marker before, one after, the one before to catch container prefix errors
		*/
		needs_bounds_marker		= 1;
		bytes_needed			+= jr_malloc_prefix_bounds_marker_size ()
								+ jr_malloc_suffix_bounds_marker_size (amt_requested);

		if (amt_requested > jr_MallocMaxRequestSize) {
			fprintf(stderr,"Illegally large jr_malloc(%ld)\n", (long) amt_requested) ;
			jr_coredump() ;
		}
	}

	new_ptr		= malloc (bytes_needed);

	if (new_ptr == 0) {
		if (jr_MallocNoMemoryHandler) {
			(*jr_MallocNoMemoryHandler) (bytes_needed);
		}
		return 0;
	}
	
	if (jr_do_trace (jr_malloc_stats)) {
		jr_MallocStatLock ();
		{
			jr_MallocStatRecordMalloc(
				jr_MallocStatInfo, new_ptr, jr_malloc_usable_size_macro (new_ptr, 0)
			);
			/*
			** 4/07/07: Use 0 as the size so that if we're just doing stats and
			** the platform doesn't have a "usable size" implementation, we'll count
			** everything as size 0. Otherwise we can't account for free() properly.
			** If we're tracing calls, we could use the amt_requested info on free.
			*/
		}
		jr_MallocStatUnLock();
	}

	if (needs_bounds_marker) {
		new_ptr = (char *) new_ptr + jr_malloc_prefix_bounds_marker_size (); 
	}

	if (jr_do_trace (jr_malloc_calls) || jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
		jr_MallocDiagLock(jr_MallocDiagInfo);
		{
			jr_MallocDiagRecordMalloc (jr_MallocDiagInfo, new_ptr, amt_requested, needs_bounds_marker);

			if (jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
				/*
				** 5/1/08: If pointer is free()ed during the C++ translation unit execution,
				** before argments and "jr_malloc_trap" has been set, is later leaked,
				** the occurence count will be off, since the first occurence won't be counted.
				** In that case, record the occurence even if "jr_malloc_trap" isn't set.
				** This is most likely going to be a bookkeeping pointer.
				**
				** Note that as a result, a leak in a C++ translation unit static allocations
				** will trap even without setting "jr_malloc_trap", as long as the
				** trap file exists. The trap file can only get created if "jr_malloc_calls"
				** is set, this typically implies memory leaks are being sought out,
				** and trapping on the following run with "jr_malloc_calls"
				** will probably not be too much of an incovenience.
				*/
				jr_MallocDiagDoMallocTrap (jr_MallocDiagInfo, new_ptr);
			}
			if (needs_bounds_marker) {
				jr_MallocNumBoundsMarkedPtrs++;
			}
		}
		jr_MallocDiagUnLock(jr_MallocDiagInfo);
	}
	else {
		jr_MallocNumUnRecordedCalls++;
	}

	jr_MallocNumCalls++ ;
	/*
	 * 7/28/2005: This increment must follow the diagnostics, otherwise it thinks
	 * malloc() has been called before diagnostics are started.
	 */
	return(new_ptr) ;
}

void jr_free (mptr)
	const void *		mptr ;
{
	size_t				amt_requested				= 0;
	jr_int				has_bounds_marker			= 0;

	if (mptr == 0) return ;

	if (jr_do_trace (jr_malloc_calls) || jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
		jr_MallocDiagLock (jr_MallocDiagInfo);
		{
			if (jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
				jr_MallocDiagDoFreeTrap (jr_MallocDiagInfo, mptr);
			}
			jr_MallocDiagRecordFree (jr_MallocDiagInfo, mptr, &amt_requested, &has_bounds_marker);
		}
		jr_MallocDiagUnLock (jr_MallocDiagInfo);

		if (has_bounds_marker) {
			mptr 			= (char *) mptr - jr_malloc_prefix_bounds_marker_size ();

			jr_MallocNumBoundsMarkedPtrs++;
		}
	}

	if (jr_do_trace(jr_malloc_stats)) {
		jr_MallocStatLock ();
		{
			jr_MallocStatRecordFree (jr_MallocStatInfo, mptr, jr_malloc_usable_size_macro (mptr, 0)) ;
		}
		jr_MallocStatUnLock();
	}

	if (amt_requested > 0  &&  jr_MallocClearOnFree) {
		size_t			usable_size;
	
		/*
		** 4/7/07: jr_malloc_usable_size_macro() can only be used after we adjust the pointer
		*/

		if (has_bounds_marker) {
			jr_malloc_destroy_data (
				(void *) mptr, amt_requested
				+ jr_malloc_prefix_bounds_marker_size ()
				+ jr_malloc_suffix_bounds_marker_size (amt_requested)
			);
		}
		else {
			usable_size		= jr_malloc_usable_size_macro (mptr, amt_requested);

			/*
			 * The usable size is actual memory available, and >= amt_requested
			 */
			jr_malloc_destroy_data ((void *) mptr, usable_size);
		}
	}

	jr_FreeNumCalls ++;

	free ((void *) mptr);
	/*
	 * do the real free() after the bookkeeping since
	 * free() may be destructive and wipe out our prepended size information
	 */
}


void *jr_realloc (old_ptr, amt_requested)
	const void *		old_ptr;
	size_t				amt_requested;
{
	void *				new_ptr;
	size_t				bytes_needed				= amt_requested;
	jr_int				has_bounds_marker			= 0;
	jr_int				needs_bounds_marker			= 0;
	size_t				old_amt_requested			= 0;

	if (old_ptr) {
		if (jr_do_trace (jr_malloc_calls) || jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
			jr_MallocDiagLock (jr_MallocDiagInfo);
			{
				if (jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
					jr_MallocDiagDoFreeTrap (jr_MallocDiagInfo, old_ptr);
				}
				jr_MallocDiagRecordFree (
					jr_MallocDiagInfo, old_ptr, &old_amt_requested, &has_bounds_marker
				);
			}
			jr_MallocDiagUnLock (jr_MallocDiagInfo);

			if (has_bounds_marker) {
				old_ptr = (char *) old_ptr - jr_malloc_prefix_bounds_marker_size ();
			}
		}

		if (jr_do_trace(jr_malloc_stats)) {
			jr_MallocStatLock ();
			{
				jr_MallocStatRecordFree(
					jr_MallocStatInfo, old_ptr, jr_malloc_usable_size_macro (old_ptr, 0)
				) ;
			}
			jr_MallocStatUnLock();
		}
	}

	if (	jr_do_trace (jr_malloc_calls) || jr_do_trace (jr_malloc_trap)
		||	jr_MallocDiagIsDoingBookkeeping (jr_MallocDiagInfo)) {
		/*
		** 4/6/07: if we're recording ptrs (as part of C++ global object init, for ex.)
		** we wind up doing bookkeeping pointers, which always have bounds markers.
		** One bounds marker before, one after, the one before to catch container prefix errors
		*/
		needs_bounds_marker		= 1;
		bytes_needed			+= jr_malloc_prefix_bounds_marker_size ()
								+ jr_malloc_suffix_bounds_marker_size (amt_requested);

		if (amt_requested > jr_MallocMaxRequestSize) {
			fprintf(stderr,"Illegally large jr_realloc(%ld)\n", (long) amt_requested) ;
			jr_coredump() ;
		}
	}

	if (old_ptr) {
		new_ptr = realloc ((void *) old_ptr, bytes_needed);
	}
	else {
		new_ptr = malloc (bytes_needed);
	}

	if (new_ptr == 0) {
		if (jr_MallocNoMemoryHandler) {
			(*jr_MallocNoMemoryHandler) (bytes_needed);
		}
		return 0;
	}
	
	if (jr_do_trace (jr_malloc_stats)) {
		jr_MallocStatLock ();
		{
			jr_MallocStatRecordMalloc(
				jr_MallocStatInfo, new_ptr, jr_malloc_usable_size_macro (new_ptr, 0)
			);
		}
		jr_MallocStatUnLock();
	}

	if (needs_bounds_marker) {
		new_ptr = (char *) new_ptr + jr_malloc_prefix_bounds_marker_size (); 
	}

	if (jr_do_trace (jr_malloc_calls) || jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
		jr_MallocDiagLock (jr_MallocDiagInfo);
		{
			jr_MallocDiagRecordMalloc (jr_MallocDiagInfo, new_ptr, amt_requested, needs_bounds_marker);

			if (jr_do_trace (jr_malloc_trap) || jr_MallocRecordPtrsOnly) {
				jr_MallocDiagDoMallocTrap (jr_MallocDiagInfo, new_ptr);
			}
		}
		jr_MallocDiagUnLock (jr_MallocDiagInfo);
	}
	else {
		jr_MallocNumUnRecordedCalls++;
	}

	jr_MallocNumCalls++ ;
	/*
	 * 7/28/2005: This increment must follow the diagnostics, otherwise it things
	 * malloc() has been called before diagnostics are started.
	 */

	return (new_ptr);
}

void *jr_calloc (num_elements, element_size)
	size_t		num_elements;
	size_t		element_size;
{
	size_t		num_bytes			= num_elements * element_size;
	void *		new_array;

	new_array			= jr_malloc (num_bytes);

	memset (new_array, 0, num_bytes);

	return new_array;
}


void jr_malloc_good_size (good_size)
	size_t good_size;
{
#	ifdef has_malloc_good_size
	{
		malloc_good_size (good_size);
	}
#	endif

	jr_MallocStatLock ();
	{
		jr_MallocStatAddBucketSize (jr_MallocStatInfo, good_size);
	}
	jr_MallocStatUnLock();

}


size_t jr_malloc_usable_size (
	const void *		mptr,
	size_t				amt_requested)
{
	size_t				usable_size;


	if (	jr_do_trace (jr_malloc_calls) || jr_do_trace (jr_malloc_trap)
		||	jr_MallocDiagIsDoingBookkeeping (jr_MallocDiagInfo)) {

		/*
		** 4/7/07: Assume we'll only be called right after a malloc() if we're bookkeeping
		*/
		usable_size		= jr_malloc_usable_size_macro (
							(char *) mptr - jr_malloc_prefix_bounds_marker_size (),
							amt_requested
						);

		if (usable_size > amt_requested) {
			/*
			 * Assume the application is going to use the extra
			 * space, so update the diagnostic's idea of how
			 * much space is available at this pointer (stats
			 * recording already uses the real available size).
			 *
			 * Adjust the returned available size to accomodate the bounds markers.
			 *
			 * 4/7/07: Note that the extra space needs to be split between the usable size and the
			 * suffix bounds marker. If the bounds marker multiple is 1, then split evenly (by 2),
			 * if greater than one (e.g. 3) then give 1/4 to the usable space.
			 */
			size_t				memory_size;
			size_t				slop_size;
			size_t				new_total_size;
			jr_int				divisor			= jr_malloc_suffix_slop_divisor();

			memory_size		= jr_malloc_prefix_bounds_marker_size ()
								+ amt_requested
								+ jr_malloc_suffix_bounds_marker_size (amt_requested);

			slop_size		= usable_size - memory_size;

			if (memory_size < usable_size && slop_size / divisor > 0) { 

				new_total_size	= jr_malloc_prefix_bounds_marker_size()
								+ amt_requested + slop_size/divisor
								+ jr_malloc_suffix_bounds_marker_size (amt_requested + slop_size/divisor);

				if (new_total_size <= usable_size) {
					jr_MallocDiagLock (jr_MallocDiagInfo);
					{
						jr_MallocDiagSetSizeRequested (
							jr_MallocDiagInfo, (void *) mptr, amt_requested + slop_size/divisor
						);
					}
					jr_MallocDiagUnLock (jr_MallocDiagInfo);
				}
				usable_size	= amt_requested + slop_size/divisor;
			}
			else {
				usable_size	= amt_requested;
			}
		}
	}
	else {
		usable_size		= jr_malloc_usable_size_macro (mptr, amt_requested);
	}
		
	return usable_size;
}


void  jr_malloc_stats (wfp, message)
	FILE *wfp;
	char *message;
{
	if (! jr_do_trace(jr_malloc_stats)) {
		fprintf(wfp,"=====> no memory traces were run <======\n") ;
		return ;
	}

	jr_MallocStatLock ();
	{
		jr_MallocStatPrint (wfp, jr_MallocStatInfo, message);
	}
	jr_MallocStatUnLock();

	/*
	 * check that all file descriptors are closed
	 */
	if (jr_do_trace(jr_malloc_calls)) {
		jr_MallocDiagLock (jr_MallocDiagInfo);
		{
			jr_MallocDiagNumOpenFiles (jr_MallocDiagInfo, wfp, 0);
		}
		jr_MallocDiagUnLock (jr_MallocDiagInfo);
	}
}

const char *jr_malloc_type (mptr)
	const void *	mptr;
{
	const char *	type_string;
	
	jr_MallocDiagLock (jr_MallocDiagInfo);
	{
		type_string = jr_MallocDiagPtrType (jr_MallocDiagInfo, mptr);
	}
	jr_MallocDiagUnLock (jr_MallocDiagInfo);

	return type_string;
}

void jr_malloc_reset_calls ()
{
	jr_MallocDiagLock (jr_MallocDiagInfo);
	{
		jr_MallocDiagReset (jr_MallocDiagInfo);
	}
	jr_MallocDiagUnLock (jr_MallocDiagInfo);
}

void jr_malloc_reset_stats ()
{
	jr_MallocStatLock ();
	{
		jr_MallocStatReset (jr_MallocStatInfo);
	}
	jr_MallocStatUnLock();
}


void jr_malloc_make_trap_file (mptr)
	const void *					mptr;
{
	jr_MallocPtrMakeTrapFile (mptr, "");
}

int jr_malloc_is_trap_ptr (
	const void *					mptr)
{
	if (jr_MallocDiagTrapPtr (jr_MallocDiagInfo) == mptr) {
		return 1;
	}
	return 0;
}

void jr_malloc_destroy_data (mptr, num_bytes)
	void *							mptr;
	size_t							num_bytes;
{
	size_t							j;
	char *							mptr_str			= mptr;
	const char *					new_data_str		= jr_DESTRUCTIVE_FREE_STRING;
	const char *					curr_char_ptr		= new_data_str;

	for (j=0;  j < num_bytes;  j++) {
		mptr_str[j]	= *curr_char_ptr;

		curr_char_ptr++;

		if (*curr_char_ptr == 0) {
			curr_char_ptr	= new_data_str;
		}
	}
}

void jr_malloc_set_bounds_marker_multiple (bounds_marker_multiple)
	jr_int							bounds_marker_multiple;
{
	if (	jr_malloc_is_suffix_same (bounds_marker_multiple)
		&&	jr_malloc_is_prefix_same (bounds_marker_multiple)
	) {
		/*
		** 9/4/07: to avoid the 'change' error below
		*/
		return;
	}

	if (jr_MallocNumBoundsMarkedPtrs > 0) {
		fprintf (stderr, "\n%s: already allocated bounds marked pointers, can't change multiple now\n",
			"jr_malloc_set_bounds_marker_multiple()"
		);
		jr_coredump() ;
	}

	if (bounds_marker_multiple < 0  ||  bounds_marker_multiple > jr_MALLOC_MAX_BOUNDS_MARKER_MULTIPLE) {
		fprintf (stderr, "\n%s: multiple %d not in [0, %d]\n",
			"jr_malloc_set_bounds_marker_multiple()",
			bounds_marker_multiple, jr_MALLOC_MAX_BOUNDS_MARKER_MULTIPLE
		);
		jr_coredump() ;
	}

	jr_malloc_set_suffix_multiple (bounds_marker_multiple);
	jr_malloc_set_prefix_multiple (bounds_marker_multiple);
}

void jr_malloc_set_no_memory_handler (
	void							(*handler_fn) (size_t))
{
	jr_MallocNoMemoryHandler = handler_fn;
}


void jr_MallocDefaultNoMemoryHandler (
	size_t							bytes_needed)
{
	fprintf(stderr, "Out of memory.\n") ;
	exit(1) ;
}

unsigned jr_int jr_malloc_max_request_size()
{
	return jr_MallocMaxRequestSize;
}

void jr_malloc_set_max_request_size (unsigned jr_int v)
{
	jr_MallocMaxRequestSize = v;
}

jr_int jr_malloc_has_clear_on_free()
{
	return jr_MallocClearOnFree;
}

void jr_malloc_set_clear_on_free (jr_int v)
{
	jr_MallocClearOnFree = v != 0;
}

unsigned jr_int jr_malloc_num_calls()
{
	return jr_MallocNumCalls;
}

void jr_malloc_set_num_calls (unsigned jr_int v)
{
	jr_MallocNumCalls = v;
}

unsigned jr_int jr_malloc_num_unrecorded_calls()
{
	return jr_MallocNumUnRecordedCalls;
}

static jr_int	jr_MallocNumCppTranslationUnits	= 0;
static jr_int	jr_MallocDumpWasDelayed			= 0;
static jr_int	jr_MallocCheckLeaksWasDelayed	= 0;

void jr_malloc_begin_cpp_trans_unit_constructor ()
{
	jr_MallocRecordPtrsOnly		= 1;
	jr_MallocNumCppTranslationUnits ++;
}

void jr_malloc_begin_cpp_trans_unit_destructor ()
{
	char		error_buf[jr_ERROR_LEN];
	jr_int		status;

	jr_MallocNumCppTranslationUnits --;

	if (jr_MallocNumCppTranslationUnits == 0) {

		if (jr_MallocCheckLeaksWasDelayed) {

			status = jr_malloc_check_leaks_and_bounds (error_buf);

			if (status != 0) {
				fprintf (stderr, "jr_malloc() error: %s\n", error_buf);
			}
		}
		if (jr_MallocDumpWasDelayed) {
			jr_malloc_dump();
		}
	}
}

void jr_malloc_end_cpp_trans_unit_constructor ()
{
	jr_MallocRecordPtrsOnly		= 0;
}

jr_int jr_malloc_num_cpp_trans_units()
{
	return jr_MallocNumCppTranslationUnits;
}

jr_int jr_malloc_record_ptrs_only ()
{
	return jr_MallocRecordPtrsOnly;
}


void jr_malloc_dump ()
{
	if (jr_malloc_num_cpp_trans_units() > 0) {
		jr_MallocDumpWasDelayed = 1;
		return;
	}

	jr_MallocDiagLock (jr_MallocDiagInfo);
	{
		jr_MallocDiagDump (jr_MallocDiagInfo);
	}
	jr_MallocDiagUnLock (jr_MallocDiagInfo);
}

jr_int jr_malloc_check_leaks_and_bounds (error_buf)
	char *			error_buf;
{
	jr_int			num_open_files;
	jr_int			status;


	if (jr_malloc_num_cpp_trans_units() > 0) {
		jr_MallocCheckLeaksWasDelayed 	= 1;
		return 0;
	}

	jr_MallocDiagLock (jr_MallocDiagInfo);

	if ( !jr_do_trace(jr_malloc_calls) && !jr_do_trace(jr_malloc_trap)) {
		jr_esprintf (error_buf, "can't check for leaks: jr_do_trace (jr_malloc_calls) not set");

		status	= -1;
		goto return_status;
	}


	num_open_files	= jr_MallocDiagNumOpenFiles (jr_MallocDiagInfo, 0 /* wfp */, error_buf);

	if (num_open_files < 0) {
		status	= -1;
		goto return_status;
	}


	status			= jr_MallocDiagCheckLeaksAndBounds (jr_MallocDiagInfo, error_buf);

	if (status == 0) {
		if (num_open_files > 0) {
			jr_esprintf (error_buf, "%d files still open: (%s)", num_open_files, error_buf);
			status = -1;
		}
	}
	else {
		if (num_open_files > 0) {
			jr_esprintf (error_buf, "%.150s, %d files still open", error_buf, num_open_files);
		}
		else {
		}
	}


	if (jr_malloc_has_resource_leak ()) {

		if (status == 0) {
			jr_esprintf (error_buf, "application resource leak");
		}
		else {
			jr_esprintf (error_buf, "%.150s, and application resource leak", error_buf); 
		}
		status = -1;
	}

	return_status : {
		jr_MallocDiagUnLock (jr_MallocDiagInfo);
	}
	return status;
}

jr_int jr_malloc_num_open_files (opt_wfp, error_buf)
	FILE *			opt_wfp;
	char *			error_buf;
{
	jr_int			num_open_files;

	jr_MallocDiagLock (jr_MallocDiagInfo);
	{
		num_open_files	= jr_MallocDiagNumOpenFiles (jr_MallocDiagInfo, opt_wfp, error_buf);
	}
	jr_MallocDiagUnLock (jr_MallocDiagInfo);

	if (num_open_files < 0) {
		return -1;
	}
	return num_open_files;
}


void (*jr_MallocNoMemoryHandler) (size_t)	= jr_MallocDefaultNoMemoryHandler;


static jr_mutex_t	jr_MallocDiagMutex[1];
static jr_mutex_t	jr_MallocStatMutex[1];

jr_thread_once_declare_and_init (DiagThreadOnceControl);
jr_thread_once_declare_and_init (StatThreadOnceControl);


static void jr_MallocDiagMutexInit ()
{
	jr_mutex_init (jr_MallocDiagMutex, 1);
	/*
	** 3/23/07: can be recursive, since bookkeeping allocations and jr_MallocDiagDump()
	** may call jr_malloc. The latter is ok to call MallocDiag routines since it
	** will only do so when unlinking the trap file, and the former already
	** has logic to deal with reentrant bookkeeping calls.
	*/
}

static void jr_MallocStatMutexInit ()
{
	jr_mutex_init (jr_MallocStatMutex, 0);
}

static void jr_MallocDiagLock (jr_MallocDiagStruct *mdp)
{
	jr_thread_once_call (&DiagThreadOnceControl, jr_MallocDiagMutexInit);
	jr_mutex_lock (jr_MallocDiagMutex);
}

static void jr_MallocDiagUnLock (jr_MallocDiagStruct *mdp)
{
	jr_mutex_unlock (jr_MallocDiagMutex);
}

static void jr_MallocStatLock ()
{
	jr_thread_once_call (&StatThreadOnceControl, jr_MallocStatMutexInit);
	jr_mutex_lock (jr_MallocStatMutex);
}

static void jr_MallocStatUnLock ()
{
	jr_mutex_unlock (jr_MallocStatMutex);
}
