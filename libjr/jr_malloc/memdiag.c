#include "project.h"

/*
** 4/6/07: ok to use static vars since the code below is protected by mutexes.
**
** Note: do not use jr_esprintf() which could call jr_malloc().
*/

char *	jr_MallocTrapFileName 	= jr_MALLOC_TRAP_FILE_NAME;
char *	jr_MallocTraceFileName 	= jr_MALLOC_TRACE_FILE_NAME;
int		jr_MallocCalledByNew	= 0;

static char ErrorBuf[jr_ERROR_LEN];

static jr_MallocDiagStruct *	Mdp							= 0 ;

void jr_MallocDiagRecordInfo (
	jr_MallocDiagStruct *		mdp,
	const void *				mptr,
	size_t						amt_requested,
	jr_int						is_malloc,
	jr_int						is_bookkeeping,
	jr_int						has_bounds_marker,
	size_t *					amt_requested_ref,
	jr_int *					has_bounds_marker_ref)
{
	jr_MallocPtrInfoStruct *	ptr_info;
	jr_int						old_doing_bookkeeping;
	void *						calling_pc					= 0;


	if (!mdp->did_initialization) {
		Mdp = mdp;				/* DiagInit() may call functions using Mdp*/
		jr_MallocDiagInit (mdp);
	}

	if (is_malloc) {
		calling_pc	= jr_MallocGetGrandParentPC ();
	}

	if (mdp->DoingBookkeeping) {
		/*
		** 3/23/07: we always add bounds to bookkeeping pointers, since otherwise
		** we can't figure out what to do when realloc()ing bookkeeping pointers
		** while not doing bounds on application pointers.
		*/
		has_bounds_marker		= 1;
		is_bookkeeping			= 1;

		if (mdp->NextBookkeepingIndex == MAX_BOOKKEEPING_MALLOCS) {
			/*
			 * can't record it
			 */
			mdp->BookkeepingArrayOverflow = 1;
		}
		else {
			ptr_info					= mdp->BookkeepingArray + mdp->NextBookkeepingIndex;

			jr_MallocPtrInfoInit (
				ptr_info, mptr, amt_requested, calling_pc, mdp->heap_base_ptr,
				is_malloc, is_bookkeeping, has_bounds_marker
			);

			mdp->NextBookkeepingIndex++;
		}
		/*
		 * Can't do bounds checking if we're doing bookkeeping, because the pointer hash table
		 * is being manipulated, and may be in an inconsistent state.
		 */
		if (has_bounds_marker_ref) {
			*has_bounds_marker_ref	= has_bounds_marker;
		}
		if (amt_requested_ref) {
			*amt_requested_ref		= amt_requested;
		}
		return;
	}

	old_doing_bookkeeping	= mdp->DoingBookkeeping;
	mdp->DoingBookkeeping	= 1;

	ptr_info = jr_MallocDiagFindPtrInfo (mdp, mptr);

	if (ptr_info) {
		if (is_malloc) {
			if (jr_MallocPtrInfoIsMalloced (ptr_info)) {
				/*
				 * already allocated
				 */
				fprintf (stderr, 
					"===> jr_MallocDiagRecordInfo: jr_malloc of %p but already allocated\n", mptr
				);
				jr_MallocPtrMakeTrapFile (mptr, "already allocated");
				jr_coredump ();
			}
			else {
				jr_MallocPtrInfoMakeMalloced (ptr_info);

				if (is_bookkeeping) {
					jr_MallocPtrInfoMakeBookkeeping (ptr_info);
				}
				else {
					jr_MallocPtrInfoClearBookkeeping (ptr_info);
				}
				jr_MallocPtrInfoSetHasBoundsMarker (ptr_info, has_bounds_marker);

				jr_MallocPtrInfoSetRequestedSize (ptr_info, amt_requested);
				jr_MallocPtrInfoSetCallingPC (ptr_info, calling_pc);
				jr_MallocPtrInfoSetHeapBase (ptr_info, mdp->heap_base_ptr);
				jr_MallocPtrInfoOccurence (ptr_info) ++;
			}
		}
		else {
			/*
			 * is being jr_free()ed
			 */
			if (jr_MallocPtrInfoIsMalloced (ptr_info)) {
				/*
				 * allocated, don't reset is bookkeeping,
				 * that info is needed later in the bounds checking
				 */
				jr_MallocPtrInfoClearMalloced (ptr_info);
				has_bounds_marker	= jr_MallocPtrInfoHasBoundsMarker (ptr_info);
				amt_requested		= jr_MallocPtrInfoRequestedSize (ptr_info);
			}
			else {
				/*
				 * already jr_free()ed
				 */

				fprintf (stderr, 
					"===> jr_MallocDiagRecordInfo: jr_free(%p) already freed\n", mptr
				);
				jr_MallocPtrMakeTrapFile (mptr, "already freed");
				jr_coredump ();
			}
		}
	}
	else {
		if (is_malloc) {
			ptr_info	= jr_MallocDiagNewPtrInfo (
							mdp, mptr, amt_requested, calling_pc, mdp->heap_base_ptr,
							1, is_bookkeeping, has_bounds_marker
						);
		}
		else {
			/*
			 * never allocated
			 */
			fprintf (stderr, 
				"===> jr_MallocDiagRecordInfo: jr_free(%p) was never allocated.\n",
				mptr
			);
			jr_coredump ();
		}
	}

	if (! is_bookkeeping  &&  has_bounds_marker) {
		/* 
		 * need to bounds check before they are actually free()ed, but
		 * bookkeeping pointers have already been free()ed by the
		 * the time we execute the bounds check
		 */
		jr_int			status;
		char *			error_buf		= ErrorBuf;

		status	= jr_MallocPtrDoBounds (
					mptr, jr_MallocPtrInfoRequestedSize (ptr_info), is_malloc, error_buf
				);

		if (status != 0) {
			/*
			 * 7/23/2003 Only returns bad if is for free
			 */
			fprintf (stderr, "===> jr_free(%p): %s\n", mptr, error_buf);
			jr_MallocPtrMakeTrapFile (mptr, error_buf);
			jr_coredump ();
		}
	}

	/*
	 * now each pointer allocated as part of the 
	 * bookkeeping must be recorded
	 * turn off bookkeeping so that they will be inserted
	 */
	mdp->DoingBookkeeping = 0;

	if (mdp->NextBookkeepingIndex > 0) {
		jr_MallocPtrInfoStruct *	bk_ptr_info;

		if (mdp->FirstBookkeepingIndex == mdp->NextBookkeepingIndex) {
			/*
			 * we've recorded all the pointers in the array
			 * reset it to the beginning
			 */
			mdp->FirstBookkeepingIndex	= 0;
			mdp->NextBookkeepingIndex	= 0;
		}
		else {
			bk_ptr_info = mdp->BookkeepingArray + mdp->FirstBookkeepingIndex;
			mdp->FirstBookkeepingIndex++;

			jr_MallocDiagRecordInfo (
				mdp,
				jr_MallocPtrInfoPtr (bk_ptr_info),
				jr_MallocPtrInfoRequestedSize (bk_ptr_info),
				jr_MallocPtrInfoIsMalloced (bk_ptr_info),
				1,
				jr_MallocPtrInfoHasBoundsMarker (bk_ptr_info),
				0, 0
			);
			/*
			 * 1 since it is a bookkeeping pointer
			 * this will recursively process all the pointers
			 * in the bookkeeping array.
			 *
			 * Don't check for traps here, since each bookkeeping pointer
			 * was allocated via jr_malloc() or jr_realloc(), and those
			 * functions bumped the number of occurences already.
			 */
		}
	}
	mdp->DoingBookkeeping = old_doing_bookkeeping;

	if (jr_do_trace (jr_malloc_trace)) {
		/*
		** 6/27/08: do after the bounds are added, and after DoingBookkeeping is reset.
		*/
		jr_MallocDiagPrintTrace (mdp, ptr_info);
	}

	if (has_bounds_marker_ref) {
		*has_bounds_marker_ref	= has_bounds_marker;
	}
	if (amt_requested_ref) {
		*amt_requested_ref		= amt_requested;
	}
}


void jr_MallocDiagInit (mdp)
	jr_MallocDiagStruct *	mdp;
{
	FILE *					trap_fp;
	char					trap_file_name[256];


	jr_check_trace_macro (0) ; /* here to make buggy linker work */


	memset (mdp, 0, sizeof (*mdp));

	mdp->did_initialization		= 1;
	mdp->doing_initialization	= 1;

	mdp->heap_base_ptr			= jr_heap_base();

	/*
	 * Set the trap pointer first so that if the trap pointer
	 * was a bookkeeping pointer first, the number of occurences
	 * will be incremented correctly.  Otherwise early bookkeeping
	 * occurences of the trap pointer will not be counted, because
	 * the mdp->TrapPtr field is not set.
	 *
	 * The following code should not call jr_malloc() or jr_free()?
	 */

	jr_MakeDiagFileName (trap_file_name, sizeof (trap_file_name), jr_MallocTrapFileName);

	trap_fp = fopen (trap_file_name, "r");

	if (trap_fp == 0) {
		if (jr_do_trace (jr_malloc_trap)) {
			fprintf (stderr, "===> jr_MallocDiagInit: couldn't open %s: %s\n", 
				trap_file_name, strerror (errno)
			);
		}
	}
	else {
		char					buf[256];
		jr_int					nitems;
		void *					tmp_ptr;
		jr_int					num_bytes;
		jr_int					occurrence;
		void *					old_heapbase;

		while (fgets (buf, sizeof(buf), trap_fp)) {

			nitems = sscanf (buf, 
				"%p : from  jr_malloc(%d bytes)  occurrence %d (pc %*p, heap %p)", 
				&tmp_ptr, &num_bytes, &occurrence, &old_heapbase
			);
			if (nitems >= 3) {
				if (nitems >= 4) {
					/*
					** 6-8-2011: To account for possible heap randomization, map the old pointer into
					** the new heap's address range.
					*/
					tmp_ptr	= (char *) mdp->heap_base_ptr + ((char *) tmp_ptr -  (char *) old_heapbase);
				}
				mdp->TrapPtr			= tmp_ptr;
				mdp->TrapPtrAmount		= num_bytes;
				mdp->TrapOnOccurence	= occurrence;

				break;
			}

			mdp->TrapPtr				= 0;
			mdp->TrapPtrAmount			= 0;
			mdp->TrapOnOccurence		= 0;
		}
		fclose (trap_fp);
	}

	if (jr_do_trace (jr_malloc_trace)) {
		char						trace_file_name[256];

		jr_MakeDiagFileName (trace_file_name, sizeof (trace_file_name), jr_MallocTraceFileName);

		if (mdp->did_trace_file_empty) {
			mdp->did_trace_file_empty		= 1;

			mdp->trace_wfp	= fopen (trace_file_name, "a");
		}
		else {
			mdp->trace_wfp	= fopen (trace_file_name, "w");
		}

		if (mdp->trace_wfp == NULL) {
			fprintf (stderr, "===> jr_MallocDiagInit(): can't open '%s': %s\n",
				trace_file_name, strerror (errno)
			);
		}
	}

	if (	jr_do_trace (jr_malloc_calls)
		||  jr_do_trace (jr_malloc_trap)
		||	jr_malloc_record_ptrs_only()
	) {

		mdp->DoingBookkeeping = 1;
		{
			/*
			 * if in the initialize routines any jr_mallocs occur,
			 * we have to be sure not to use the pointer table
			 */

			jr_HTableInit (
				mdp->PointerTable, sizeof (jr_MallocPtrInfoStruct), 
				jr_MallocPtrInfoHash,
				jr_MallocPtrInfoCmp
			);

			jr_HTableIncreaseCapacity (mdp->PointerTable, 8);
			/*
			 * make sure the table will hold enough pointers to store 1 plus
			 * any bookkeeping pointers without needing to reallocate
			 * Otherwise a malloc diag error occurs when it trys to grow the pointer
			 * table while recording the orignal bookkeeping pointers.
			 */

			jr_MallocDiagInitExemptDescriptorArray (mdp);
		}
		mdp->DoingBookkeeping = 0;
	}
	mdp->doing_initialization	= 0;

	if (jr_do_trace( jr_malloc_trap)) {
		/*
		** 8/3/09: Application code may do things differently if jr_malloc_calls is on,
		** i.e. extra checking, etc. If we're trapping, this code should also execute.
		** Within jr_malloc, all logic checks for both appropriately, but application
		** code assumes calls is traced when trap is, which is in fact true.
		*/
		jr_set_trace( jr_malloc_calls);
	}
}


void jr_MallocDiagUndo (mdp)
	jr_MallocDiagStruct *	mdp;
{
	mdp->DoingBookkeeping 		= 1;
	{
		jr_HTableUndo (mdp->PointerTable);

		if (mdp->exempt_fd_array) {
			jr_free (mdp->exempt_fd_array);
		}

		mdp->did_initialization	= 0;
	}
	mdp->DoingBookkeeping		= 0;

	if (mdp->trace_wfp) {
		fclose (mdp->trace_wfp);
	}
}

void jr_MallocDiagReset (mdp)
	jr_MallocDiagStruct *	mdp;
{
	if (jr_do_trace (jr_malloc_calls)  ||  jr_do_trace (jr_malloc_trap)) {

		if (mdp->did_initialization) {
			jr_MallocDiagUndo (mdp);
			jr_malloc_set_num_calls (0) ; /* do this first to avoid jr_check_trace error */
			jr_MallocDiagInit (mdp);
		}
	}
}

void jr_MallocDiagPrintTrace (
	jr_MallocDiagStruct *		mdp,
	jr_MallocPtrInfoStruct *	ptr_info)
{
	char						trace_file_name[256];

	if (mdp->trace_wfp == 0) {
		jr_MakeDiagFileName (trace_file_name, sizeof (trace_file_name), jr_MallocTraceFileName);

		/*
		** 6/27/08: Can't use jr_Unlink() to empty the file, since that calls jr_malloc()
		** (it uses APR, which calls jr_malloc()), and this causes recursion back to here.
		*/
		if (mdp->did_trace_file_empty) {
			mdp->did_trace_file_empty		= 1;

			mdp->trace_wfp	= fopen (trace_file_name, "a");
		}
		else {
			mdp->trace_wfp	= fopen (trace_file_name, "w");
		}

		if (mdp->trace_wfp == NULL) {
			fprintf (stderr, "===> jr_MallocDiagPrintTrace(): can't open '%s': %s\n",
				trace_file_name, strerror (errno)
			);
			return;
		}
	}

	/*
	** 6-8-2011: used only for tracing all allocations, use simpler print to
	** instead of jr_MallocPtrInfoPrint() avoid extra work of checking bounds?
	** Or because of possible malloc recursion?
	*/
	fprintf (mdp->trace_wfp, "%p : from  jr_malloc(%d bytes)  occurrence %d (pc %p) %s%s%s\n",
		jr_MallocPtrInfoPtr (ptr_info),
		(jr_int) jr_MallocPtrInfoRequestedSize (ptr_info), 
		jr_MallocPtrInfoOccurence (ptr_info),
		jr_MallocPtrInfoCallingPC (ptr_info),
		jr_MallocPtrInfoIsMalloced (ptr_info)? "jr_malloc()ed" : "jr_free()ed",
		jr_MallocPtrInfoIsBookkeeping (ptr_info) ? " bookkeeping" : " ",
		jr_MallocPtrInfoHasBoundsMarker (ptr_info) ? " " : " (no bounds)"
	);
}

void jr_MallocPtrInfoInit (
	jr_MallocPtrInfoStruct *		mptr_info,
	const void *					mptr,
	size_t							amt_requested,
	const void *					calling_pc,
	const void *					heap_base_ptr,
	jr_int							is_malloced,
	jr_int							is_bookkeeping,
	jr_int							has_bounds_marker)
{
	mptr_info->ptr					= mptr;
	mptr_info->amt_requested		= amt_requested;
	mptr_info->calling_pc			= calling_pc;
	mptr_info->heap_base_ptr		= heap_base_ptr;
	mptr_info->is_malloced			= is_malloced;
	mptr_info->is_bookkeeping		= is_bookkeeping;
	mptr_info->has_bounds_marker	= has_bounds_marker;
	mptr_info->occurrence			= 1;
}

jr_int jr_MallocPtrInfoHash (void_arg_1)
	const void *					void_arg_1;
{
	const jr_MallocPtrInfoStruct *	ptr_info_1	= void_arg_1;

	return jr_ptrhash (ptr_info_1->ptr);
}

jr_int jr_MallocPtrInfoCmp (void_arg_1, void_arg_2)
	const void *					void_arg_1;
	const void *					void_arg_2;
{
	const jr_MallocPtrInfoStruct *	ptr_info_1	= void_arg_1;
	const jr_MallocPtrInfoStruct *	ptr_info_2	= void_arg_2;

	return jr_ptrcmp (ptr_info_1->ptr, ptr_info_2->ptr);
}


jr_MallocPtrInfoStruct *jr_MallocDiagFindPtrInfo (mdp, mptr)
	jr_MallocDiagStruct *		mdp;
	const void *				mptr;
{
	jr_MallocPtrInfoStruct		tmp_ptr_info[1];
	jr_MallocPtrInfoStruct *	ptr_info;

	jr_MallocPtrInfoInit (tmp_ptr_info, mptr, 0, 0, 0, 0, 0, 0);

	ptr_info = jr_HTableFindElementPtr (mdp->PointerTable, tmp_ptr_info);

	return ptr_info;
}
	
jr_MallocPtrInfoStruct *jr_MallocDiagNewPtrInfo (
	jr_MallocDiagStruct *			mdp,
	const void *					mptr,
	size_t							amt_requested,
	const void *					calling_pc,
	const void *					heap_base_ptr,
	jr_int							is_malloced,
	jr_int							is_bookkeeping,
	jr_int							has_bounds_marker)
{
	jr_MallocPtrInfoStruct			tmp_ptr_info[1];
	jr_MallocPtrInfoStruct *		ptr_info;

	unsigned jr_int					old_max_request_size	= jr_malloc_max_request_size ();

	jr_malloc_set_max_request_size (jr_UINT_MAX);

	jr_MallocPtrInfoInit (
		tmp_ptr_info, mptr, amt_requested, calling_pc, heap_base_ptr,
		is_malloced, is_bookkeeping, has_bounds_marker
	);

	ptr_info = jr_HTableSetNewElement (mdp->PointerTable, tmp_ptr_info);

	jr_malloc_set_max_request_size (old_max_request_size);

	return ptr_info;
}
	


jr_int jr_MallocPtrDoBounds (mptr_arg, amt_requested, for_malloc, error_buf)
	const void *			mptr_arg ;
	size_t					amt_requested;
	jr_int					for_malloc;
	char *					error_buf;
{
	char *					mptr			= (char *) mptr_arg ;
	char *					cptr;
	jr_int					i;

	jr_int					suffix_bounds_marker_size;
	jr_int					prefix_bounds_marker_size;

	/*
	 * Assumes the amt_requested passed is is correct
	 */

	suffix_bounds_marker_size	= jr_malloc_suffix_bounds_marker_size (amt_requested);
	prefix_bounds_marker_size	= jr_malloc_prefix_bounds_marker_size ();

	if (for_malloc) {
		/*
		 * Put a marker at the end of the amt_requested to
		 * check for bounds errors upon jr_free()
		 */
		cptr = mptr + amt_requested;

		for (i = 0;  i < suffix_bounds_marker_size;  i++) {
			cptr [i] = jr_malloc_suffix_marker_char (i);
		}
		cptr [suffix_bounds_marker_size - 1] = 0;

		/*
		 * Put a marker before pointer to check for prefix errors upon jr_free().
		 */
		cptr = mptr - prefix_bounds_marker_size;

		for (i = 0;  i < prefix_bounds_marker_size;  i++) {
			cptr [i] = jr_malloc_prefix_marker_char (i);
		}
	}
	else {

		cptr = mptr + amt_requested;

		for (i = 0;  i < suffix_bounds_marker_size - 1;  i++) {
			if (cptr [i] != jr_malloc_suffix_marker_char (i)) {
				sprintf (error_buf, "bounds error, marker index %d", i);
				return -1;
			}
		}
		if (cptr [suffix_bounds_marker_size - 1] != 0) {
			sprintf (error_buf, "bounds error, marker index %d", suffix_bounds_marker_size - 1);
			return -1;
		}

		cptr = mptr - prefix_bounds_marker_size;

		for (i = 0;  i < prefix_bounds_marker_size;  i++) {
			if (cptr [i] != jr_malloc_prefix_marker_char (i)) {
				sprintf (error_buf, "prefix bounds error, marker index %d", i);
				return -1;
			}
		}
	}

	return 0;
}


void jr_MallocDiagDoTrap (mdp, mptr, is_malloc)
	jr_MallocDiagStruct *	mdp;
	const void *			mptr;
	jr_int					is_malloc;
{
	if (!mdp->did_initialization) {
		Mdp = mdp;				/* The init may call functions using Mdp*/
		jr_MallocDiagInit (mdp);
	}

	if (mptr == mdp->TrapPtr) {
		if (is_malloc) mdp->CurrOccurence++;
		if (mdp->CurrOccurence >= mdp->TrapOnOccurence  ||
			mdp->TrapOnOccurence == 0) {

			fprintf(stderr, "%s : trap file occurrence %d of %p found\n",
				is_malloc ? "jr_malloc()" : "jr_free()",
				mdp->CurrOccurence, mptr
			) ;
			jr_debugger_trap ();
		}
	}
}


void jr_MallocDiagDump (mdp)
	jr_MallocDiagStruct *		mdp;
{
	jr_MallocPtrInfoStruct *	ptr_info;
	jr_int						num_leaks				= 0;
	FILE *						trap_fp;
	char						trap_file_name[256];
	jr_int						bounds_status;

	if (!mdp->did_initialization) {
		return;
	}

	if ( !jr_do_trace(jr_malloc_calls)  &&  !jr_do_trace (jr_malloc_trap)) {
		fprintf(stderr, "===> Can't jr_MallocDiagDump without  jr_do_trace(jr_malloc_calls)  on.\n") ;
		return ;
	}

	if (mdp->BookkeepingArrayOverflow) {
		fprintf (stderr, "===> jr_Malloc checking internal overflow.\n");
		fprintf (stderr, "===> Increase MAX_BOOKKEEPING_MALLOCS in jr_malloc.c\n");
	}


	jr_MakeDiagFileName (trap_file_name, sizeof (trap_file_name), jr_MallocTrapFileName);

	trap_fp = fopen (trap_file_name, "w");

	if (trap_fp == 0) {
		fprintf (stderr, "===> jr_MallocDiagDump(): can't open '%s': %s\n",
			trap_file_name, strerror (errno)
		);
	}
	else {
		jr_HTableForEachElementPtr (mdp->PointerTable, ptr_info) {

			if (	jr_MallocPtrInfoIsMalloced (ptr_info)
				&&	!jr_MallocPtrInfoIsBookkeeping (ptr_info)) {

				if (jr_MallocPtrInfoHasBoundsMarker (ptr_info)) {
					bounds_status = jr_MallocPtrInfoCheckBounds (ptr_info, ErrorBuf);
				}
				else {
					bounds_status = 0;
				}

				if (bounds_status != 0) {
					jr_MallocPtrInfoPrint (ptr_info, trap_fp);
				}
				else if (num_leaks < jr_malloc_max_leak_trap_print ()) {
					jr_MallocPtrInfoPrint (ptr_info, trap_fp);
				}
				num_leaks ++;
			}
		}
		if (num_leaks) {
			fprintf (trap_fp, "Total still allocated: %d\n", num_leaks);

			if (jr_trace_level (jr_malloc_calls) < 2) {
				fprintf (trap_fp, "For valid PC values, set jr_malloc_calls level to 2 or higher\n");
			}
		}
		fclose (trap_fp);

		if (num_leaks == 0) {
			jr_UnlinkNoApr (trap_file_name, 0);
		}
	}
}


jr_int jr_MallocDiagCheckLeaksAndBounds (mdp, error_buf)
	jr_MallocDiagStruct *		mdp;
	char *						error_buf;
{
	jr_MallocPtrInfoStruct *	ptr_info;
	jr_int						num_allocated		= 0;
	jr_int						num_bounds_errors	= 0;

	jr_int						status;


	if (!mdp->did_initialization) {
		return 0;
	}
	if ( !jr_do_trace(jr_malloc_calls)  &&  !jr_do_trace (jr_malloc_trap)) {
		sprintf (error_buf,
			"can't count jr_malloc() calls without jr_do_trace(jr_malloc_calls).\n"
		);
		return -1;
	}

	if (mdp->BookkeepingArrayOverflow) {
		sprintf (error_buf, "jr_Malloc checking internal overflow.");
		return -1;
	}

	jr_HTableForEachElementPtr (mdp->PointerTable, ptr_info) {
		if (jr_MallocPtrInfoIsBookkeeping (ptr_info)) {

			continue;
		}
		if (jr_MallocPtrInfoIsMalloced (ptr_info)) {
			/*
			 * check for the bounds error marker, bookkeeping mallocs aren't bounds checked
			 */
			num_allocated ++;

			if (jr_MallocPtrInfoHasBoundsMarker (ptr_info)) {
				status	= jr_MallocPtrInfoCheckBounds (ptr_info, error_buf);

				if (status != 0) {
					num_bounds_errors++;
				}
			}
		}
	}
	if (num_allocated  ||  num_bounds_errors) {
		if (num_bounds_errors == 0) {
			sprintf (error_buf, "%d still allocated", num_allocated);
		}
		else if (num_allocated == 0) {
			sprintf (error_buf, "%d bounds errors", num_bounds_errors);
		}
		else {
			sprintf (error_buf, "%d still allocated, %d bounds errors", 
				num_allocated, num_bounds_errors
			);
		}
		return -1;
	}

	return 0;
}


jr_int jr_MallocDiagNumLeaks (mdp)
	jr_MallocDiagStruct *		mdp;
{
	jr_MallocPtrInfoStruct *	ptr_info;
	jr_int						num_leaks				= 0;

	if (!mdp->did_initialization) {
		return 0;
	}

	if ( !jr_do_trace(jr_malloc_calls)  &&  !jr_do_trace (jr_malloc_trap)) {
		fprintf(stderr,
			"===> Can't jr_MallocDiagNumLeaks() without  jr_do_trace(jr_malloc_calls)  on.\n"
		) ;
		return 0;
	}

	jr_HTableForEachElementPtr (mdp->PointerTable, ptr_info) {

		if (	jr_MallocPtrInfoIsMalloced (ptr_info)
			&&	!jr_MallocPtrInfoIsBookkeeping (ptr_info)) {

			num_leaks ++;
		}
	}
	return num_leaks;
}


const char *jr_MallocDiagPtrType(mdp, pointer)
	jr_MallocDiagStruct *		mdp;
	const void *				pointer ;
{
	static char					sbuf[256] ;
	jr_MallocPtrInfoStruct *	ptr_info;

	if (! pointer) {
		snprintf(sbuf, sizeof(sbuf), "%p : (null address)", pointer) ;
	}
	else if (pointer > (void *) (&pointer))  {
		snprintf(sbuf, sizeof(sbuf), "%p : (stack address)", pointer) ;
	}
	else if (jr_do_trace(jr_malloc_calls)  ||  jr_do_trace(jr_malloc_trap)) {
		ptr_info = jr_MallocDiagFindPtrInfo (mdp, pointer) ;

		if (ptr_info) {
			jr_MallocPtrInfoFormatString (ptr_info, sbuf, sizeof(sbuf));
		}
		else {
			snprintf(sbuf, sizeof(sbuf), "%p : (unknown)", pointer) ;
		}
	}
	else {
		snprintf(sbuf, sizeof(sbuf),
			"%p : (unknown, not tracing jr_malloc)",
			pointer) ;
	}
	return(sbuf) ;
}


char *jr_MallocPtrInfoFormatString (ptr_info, sbuf, sbuf_size)
	jr_MallocPtrInfoStruct *	ptr_info;
	char *						sbuf ;
	jr_int						sbuf_size;
{
	jr_int						pid				= jr_getpid ();
	jr_int						bounds_status	= 0;
	jr_int						status;
	char						tmp_buf[jr_ERROR_LEN] ;


	if (jr_MallocPtrInfoIsMalloced (ptr_info)) {
		bounds_status = jr_MallocPtrInfoCheckBounds (ptr_info, tmp_buf);
	}

	status	= snprintf (sbuf, sbuf_size,
		"%p : from  jr_malloc(%d bytes)  occurrence %d (pc %p, heap %p) %s%s%s pid %d%s%s", 
		jr_MallocPtrInfoPtr (ptr_info),
		(jr_int) jr_MallocPtrInfoRequestedSize (ptr_info), 
		jr_MallocPtrInfoOccurence (ptr_info),
		jr_MallocPtrInfoCallingPC (ptr_info),
		jr_MallocPtrInfoHeapBase (ptr_info),
		jr_MallocPtrInfoIsMalloced (ptr_info)? "jr_malloc()ed" : "jr_free()ed",
		jr_MallocPtrInfoIsBookkeeping (ptr_info) ? " bookkeeping" : " ",
		jr_MallocPtrInfoHasBoundsMarker (ptr_info) ? " " : " (no bounds)",
		pid,
		bounds_status != 0 ? ",  "	: "",
		bounds_status != 0 ? tmp_buf	: ""
	);
	if (status >= sbuf_size ) {
		sbuf[ sbuf_size - 1 ] = 0;
	}
	return(sbuf) ;
}


void jr_MallocPtrInfoPrint (ptr_info, wfp)
	jr_MallocPtrInfoStruct *	ptr_info;
	FILE *						wfp ;
{
	char						sbuf[256] ;

	jr_MallocPtrInfoFormatString (ptr_info, sbuf, sizeof(sbuf)) ;

	if (wfp == 0) {
		wfp = stdout ;
	}

	fprintf (wfp, "%s\n", sbuf) ;
}


void jr_MallocPtrMakeTrapFile (pointer, error_mesg)
	const void *				pointer;
	const char *				error_mesg;
{
	jr_MallocPtrInfoStruct *	ptr_info;

	if (error_mesg == 0) {
		error_mesg = "0";
	}

	if (Mdp == 0) {
		fprintf (stderr, "===> jr_MallocPtrMakeTrapFile(%p, \"%s\"): malloc tracing not active\n",
			pointer, error_mesg
		);
		return;
	}
	ptr_info = jr_MallocDiagFindPtrInfo (Mdp, pointer);

	if (ptr_info == 0) {
		fprintf (stderr, "===> jr_MallocPtrMakeTrapFile(%p, \"%s\"): %s, not jr_malloc pointer\n",
			pointer, error_mesg, jr_MallocDiagPtrType (Mdp, pointer));
	}
	else {
		FILE *			trap_fp;
		char			trap_file_name[256];


		jr_MallocPtrInfoPrint (ptr_info, stderr);

		jr_MakeDiagFileName (trap_file_name, sizeof (trap_file_name), jr_MallocTrapFileName);

		trap_fp = fopen (trap_file_name, "w");

		if (trap_fp == 0) {
			fprintf (stderr, "===> jr_MallocPtrMakeTrapFile(): can't open '%s': %s\n",
				trap_file_name, strerror (errno)
			);
		}
		else {
			jr_MallocPtrInfoPrint (ptr_info, trap_fp);

			fprintf (trap_fp, "====> jr_MallocPtrMakeTrapFile (%p, \"%s\")\n",
				pointer, error_mesg
			);

			fclose (trap_fp);
		}
	}
}


size_t jr_MallocPtrSizeRequested (mptr)
	void *							mptr;
{
	jr_MallocPtrInfoStruct *		ptr_info;
	jr_int							i;

	/*
	 * first look through all the bookkeeping pointers
	 * since any changes to the size not yet updated
	 * (in the case of jr_reallocs) is kept here
	 * only if in bookkeeping will this loop do anything
	 *
	 * start with the most recent information
	 */
	if (Mdp == 0) {
		fprintf (stderr, "===> jr_MallocPtrSizeRequested(%p): malloc tracing not active\n", mptr);
		return 0;
	}
	for (i = Mdp->NextBookkeepingIndex - 1;  i >= 0;  i--) {
		ptr_info = Mdp->BookkeepingArray + i;

		if (jr_MallocPtrInfoPtr (ptr_info) == mptr) {
			if (jr_MallocPtrInfoIsMalloced (ptr_info)) {
				return (jr_MallocPtrInfoRequestedSize (ptr_info));
			}
			else {
				/*
				 * most recent activity is a jr_free
				 */
				fprintf (stderr, "===> jr_MallocPtrSizeRequested (%p): ptr has been jr_free()ed\n",
					mptr
				);
				jr_coredump ();
			}
		}
	}
	if (Mdp->doing_initialization) {
		fprintf (stderr, "===> jr_MallocPtrSizeRequested (%p): can't find ptr while doing init\n",
			mptr
		);
		jr_coredump ();
	}

	ptr_info = jr_MallocDiagFindPtrInfo (Mdp, mptr);

	if (ptr_info) {
		fprintf (stderr, "===> jr_MallocPtrSizeRequested (%p): %s, not jr_malloc pointer\n",
			mptr, jr_MallocDiagPtrType (Mdp, mptr));
		jr_coredump ();
	}

	if (! jr_MallocPtrInfoIsMalloced (ptr_info)) {
		fprintf (stderr, "===> jr_MallocPtrSizeRequested (%p): not currently allocated\n",
			mptr
		);
		jr_coredump ();
	}

	return jr_MallocPtrInfoRequestedSize (ptr_info); 
}


void jr_MallocDiagSetSizeRequested (mdp, mptr, requested_size)
	jr_MallocDiagStruct *		mdp;
	void *						mptr;
	size_t						requested_size;
{
	jr_MallocPtrInfoStruct *	ptr_info;
	jr_int						i;

	jr_int						status;

	/*
	 * first look through all the bookkeeping pointers
	 * since any changes to the size not yet updated
	 * (in the case of jr_reallocs) is kept here
	 * only if in bookkeeping will this loop do anything
	 *
	 * start with the most recent information
	 */
	for (i = mdp->NextBookkeepingIndex - 1;  i >= 0;  i--) {
		ptr_info = mdp->BookkeepingArray + i;

		if (jr_MallocPtrInfoPtr (ptr_info) == mptr) {
			if (jr_MallocPtrInfoIsMalloced (ptr_info)) {
				jr_MallocPtrInfoSetRequestedSize (ptr_info, requested_size);
				return;
			}
			else {
				/*
				 * most recent activity is a jr_free
				 */
				fprintf (stderr,
					"===> jr_MallocDiagSetSizeRequested (%p): ptr has been jr_free()ed\n",
					mptr
				);
				jr_coredump ();
			}
		}
	}
	if (mdp->doing_initialization) {
		fprintf (stderr,
			"===> jr_MallocDiagSetSizeRequested (%p): can't find ptr while doing init\n",
			mptr
		);
		jr_coredump ();
	}

	ptr_info = jr_MallocDiagFindPtrInfo (mdp, mptr);

	if (ptr_info == 0) {
		fprintf (stderr,
			"===> jr_MallocDiagSetSizeRequested (%p): %s, not jr_malloc pointer\n",
			mptr, jr_MallocDiagPtrType (mdp, mptr));
		jr_coredump ();
	}

	if (! jr_MallocPtrInfoIsMalloced (ptr_info)) {
		fprintf (stderr,
			"===> jr_MallocDiagSetSizeRequested (%p): not currently allocated\n",
			mptr
		);
		jr_coredump ();
	}

	jr_MallocPtrInfoSetRequestedSize (ptr_info, requested_size); 

	status	= jr_MallocPtrDoBounds (mptr, requested_size, 1, ErrorBuf);

	if (status != 0) {
		fprintf (stderr, "===> jr_MallocDiagSetSizeRequested(%p): %s\n", mptr, ErrorBuf);
		jr_MallocPtrMakeTrapFile (mptr, ErrorBuf);
		jr_coredump ();
	}
}


jr_MallocPtrInfoStruct * jr_MallocPtrFindInfoPtr (pointer)
	void *						pointer;
{
	jr_MallocPtrInfoStruct *	ptr_info;

	if (Mdp == 0) {
		fprintf (stderr, "===> Can't run search without  jr_do_trace(jr_malloc_calls)  on.\n") ;
		return 0;
	}

	ptr_info = jr_MallocDiagFindPtrInfo (Mdp, pointer);


	return ptr_info;
}


void jr_MallocPtrPrintInfo (pointer)
	void *						pointer;
{
	jr_MallocPtrInfoStruct *	ptr_info;

	if (Mdp == 0) {
		fprintf (stderr, "===> Can't run search without  jr_do_trace(jr_malloc_calls)  on.\n") ;
		return ;
	}

	ptr_info = jr_MallocDiagFindPtrInfo (Mdp, pointer);

	if (ptr_info == 0) {
		fprintf (stderr, "===> jr_MallocPtrPrintInfo: %s, not jr_malloc pointer\n",
			jr_MallocDiagPtrType (Mdp, pointer)
		);
		return;
	}

	jr_MallocPtrInfoPrint( ptr_info, stderr);
}


const void *jr_MallocPtrPrintPrevInfo (search_pointer)
	const void *				search_pointer ;
{
	jr_MallocPtrInfoStruct *	ptr_info;
	const void *				current_pointer ;

	jr_MallocPtrInfoStruct *	best_ptr_info;
	jr_MallocPtrInfoStruct *	other_ptr_info;

	const void *				current_allocated_guess		= 0 ;
	jr_MallocPtrInfoStruct * 	current_allocated_info		= 0;

	const void *				current_free_guess			= 0 ;
	jr_MallocPtrInfoStruct * 	current_free_info			= 0;

	char						trap_file_name[256];
	FILE *						trap_fp;


	if (Mdp == 0) {
		fprintf(stderr, "===> Can't run search without  jr_do_trace(jr_malloc_calls)  on.\n") ;
		return 0;
	}

	jr_HTableForEachElementPtr (Mdp->PointerTable, ptr_info) {
		current_pointer = jr_MallocPtrInfoPtr (ptr_info);

		if (jr_MallocPtrInfoIsMalloced (ptr_info)) {
			if ((current_pointer > current_allocated_guess) &&
				(current_pointer < search_pointer)) {

				current_allocated_guess = current_pointer ;
				current_allocated_info	= ptr_info;
			}
		}
		else {
			if ((current_pointer > current_free_guess) &&
				(current_pointer < search_pointer)) {

				current_free_guess = current_pointer ;
				current_free_info	= ptr_info;
			}
		}
	}

	if (current_free_guess == 0 && current_allocated_guess == 0) {
		fprintf(stderr,"===> No previous pointers encountered.\n") ;
		return 0;
	}

	if (current_allocated_guess > current_free_guess) {
		best_ptr_info		= current_allocated_info ;
		other_ptr_info		= current_free_info ;
	}
	else {
		best_ptr_info		= current_free_info ;
		other_ptr_info		= current_allocated_info ;
	}

	fprintf (stderr,
		"Previous pointers of %p:\n", search_pointer
	);

	if (best_ptr_info) {
		jr_MallocPtrInfoPrint (best_ptr_info, stderr);
	}
	if (other_ptr_info) {
		jr_MallocPtrInfoPrint (other_ptr_info, stderr);
	}

	ptr_info = jr_MallocDiagFindPtrInfo (Mdp, search_pointer);

	if (ptr_info) {
		fprintf (stderr, "Original pointer %p is a jr_malloc()ed pointer\n", search_pointer);
	}


	jr_MakeDiagFileName (trap_file_name, sizeof (trap_file_name), jr_MallocTrapFileName);

	trap_fp = fopen (trap_file_name, "w");

	if (trap_fp == 0) {
		fprintf (stderr, "===> jr_MallocPtrPrintPrevInfo(): can't open '%s': %s\n",
			trap_file_name, strerror (errno)
		);
	}
	else {
		if (best_ptr_info) {
			jr_MallocPtrInfoPrint (best_ptr_info, trap_fp);
		}
		if (other_ptr_info) {
			jr_MallocPtrInfoPrint (other_ptr_info, trap_fp);
		}

		fprintf (trap_fp, "Previous pointers of %p\n", search_pointer);
		fclose (trap_fp);

		fprintf (stderr, "Made trap file.\n");
	}
	return jr_MallocPtrInfoPtr (best_ptr_info);
}


jr_int jr_MallocPtrCheckBounds (pointer_arg)
	const void *				pointer_arg;
{
	const char *				pointer				= pointer_arg ;
	jr_MallocPtrInfoStruct *	ptr_info;

	jr_int						status;


	if (Mdp == 0) {
		fprintf (stderr, "Can't check bounds without  jr_do_trace(jr_malloc_calls)  on\n") ;
		return -1;
	}

	ptr_info = jr_MallocDiagFindPtrInfo (Mdp, pointer);

	if (ptr_info == 0) {
		fprintf (stderr, "%p not jr_malloc() pointer", pointer);
		return -1;
	}

	if (jr_MallocPtrInfoIsBookkeeping (ptr_info)) {
		fprintf (stderr, "%p is a jr_malloc() bookkeeping pointer", pointer);
		return -1;
	}

	if (!jr_MallocPtrInfoHasBoundsMarker (ptr_info)) {
		fprintf (stderr, "%p has no bounds marker", pointer);
		return -1;
	}

	if (jr_MallocPtrInfoIsFreed (ptr_info)) {
		fprintf (stderr, "%p is already jr_free()ed (size %d)",
			pointer, (jr_int) jr_MallocPtrInfoRequestedSize (ptr_info)
		);
		return -1;
	}

	/*
	 * check for the bounds error marker
	 */

	status	= jr_MallocPtrInfoCheckBounds (ptr_info, ErrorBuf);

	if (status != 0) {
		fprintf (stderr, "%p bounds error: %s\n", pointer, ErrorBuf);
		jr_MallocPtrMakeTrapFile (pointer, ErrorBuf);
		fprintf (stderr, "Made trap file.\n");
	}
	else {
		fprintf (stderr, "%p bounds ok\n", pointer);
	}

	return status;
}


jr_int jr_MallocPtrInfoCheckBounds (ptr_info, error_buf)
	jr_MallocPtrInfoStruct *	ptr_info;
	char *						error_buf;
{
	const void *				mptr;
	size_t						requested_size;

	jr_int						status;


	mptr			= jr_MallocPtrInfoPtr (ptr_info);
	requested_size	= jr_MallocPtrInfoRequestedSize (ptr_info);

	status	= jr_MallocPtrDoBoundsCheck (mptr, requested_size, error_buf);

	return status;
}


jr_int jr_MallocCheckBounds ()
{
	const void *				bad_ptr;

	bad_ptr	= jr_malloc_check_bounds (ErrorBuf);

	if (bad_ptr != 0) {
		fprintf (stderr, "jr_MallocCheckBounds(): %s\n", ErrorBuf);

		if (bad_ptr != (const void *) 0x1) {
			jr_MallocPtrMakeTrapFile (bad_ptr, ErrorBuf);
			fprintf (stderr, "Made trap file for %p.\n", bad_ptr);
		}
		return -1;
	}
	fprintf (stderr, "jr_MallocCheckBounds(): bounds ok\n");
	return 0;
}
		

jr_int jr_MallocNumLeaks ()
{
	jr_int			num_leaks;

	if (Mdp == 0) {
		fprintf (stderr, "jr_MallocNumLeaks(): jr_do_trace(jr_malloc_calls)  not set.\n") ;
		return 0;
	}

	num_leaks	= jr_MallocDiagNumLeaks (Mdp);

	return num_leaks;
}

jr_int jr_MallocPrintOpenFiles ()
{
	jr_int			num_files;

	if (Mdp == 0) {
		fprintf (stderr, "jr_MallocPrintOpenFiles(): jr_do_trace(jr_malloc_calls)  not set.\n") ;
		return -1;
	}

	num_files	= jr_MallocDiagNumOpenFiles (Mdp, stderr, 0);

	return num_files;
}


const void *jr_malloc_check_bounds (error_buf)
	char *							error_buf;
{
	/*
	 * Must be in this file since it uses static variable 'Mdp'.
	 */
	jr_MallocPtrInfoStruct *		ptr_info;
	jr_int							status;


	if (Mdp == 0) {
		sprintf (error_buf, "can't do bounds check without  jr_do_trace(jr_malloc_calls)  on.") ;
		return (void *) 0x1;
	}


	jr_HTableForEachElementPtr (Mdp->PointerTable, ptr_info) {
		if (	!jr_MallocPtrInfoIsMalloced (ptr_info)
			||  !jr_MallocPtrInfoHasBoundsMarker (ptr_info)
			||  jr_MallocPtrInfoIsBookkeeping (ptr_info)) {
			continue;
		}
		/*
		 * check for the bounds error marker
		 */

		status	= jr_MallocPtrInfoCheckBounds (ptr_info, error_buf);

		if (status != 0) {
			return jr_MallocPtrInfoPtr (ptr_info);
		}
	}
	return 0;
}

void jr_MallocPrintFunctions ()
{
	fprintf (stderr, "void   jr_MallocCheckBounds         (void)\n");
	fprintf (stderr, "void   jr_MallocNumLeaks            (void)\n");
	fprintf (stderr, "void   jr_MallocPrintOpenFiles      (void)\n");
	fprintf (stderr, "void   jr_MallocPtrCheckBounds      (void *mptr)\n");
	fprintf (stderr, "void   jr_MallocPtrPrintInfo        (void *mptr)\n");
	fprintf (stderr, "void   jr_MallocPtrPrintPrevInfo    (void *mptr)\n");
	fprintf (stderr, "void   jr_MallocPtrMakeTrapFile     (void *mptr, char *error_msg)\n");
	fprintf (stderr, "void   jr_MallocPtrSizeRequested    (void *mptr)\n");
	fprintf (stderr, "void   jr_MallocPtrSetTrap          (void *mptr, int occurence)\n");
	fprintf (stderr, "\n");
	fprintf (stderr, "jr_MallocPtrInfoStruct * jr_MallocPtrFindInfoPtr (void *mptr)\n");

	if ( !jr_do_trace(jr_malloc_calls)  &&  !jr_do_trace (jr_malloc_trap)) {
		fprintf (stderr, "WARNING: jr_do_trace(jr_malloc_calls)  is not on.\n") ;
	}
}

void jr_MallocDebuggerFunctions ()
{
	jr_MallocPrintFunctions ();
}

void jr_MallocFunctions ()
{
	jr_MallocPrintFunctions ();
}

void jr_MallocPtrSetTrap (mptr, occurence)
	void *			mptr;
	jr_int			occurence;
{
	if (Mdp == 0) {
		fprintf(stderr, "jr_MallocPtrSetTrap(): jr_malloc() tracing is not active.\n") ;
		return;
	}

	Mdp->TrapPtr			= mptr;
	Mdp->TrapOnOccurence	= occurence;
}


void *jr_MallocGetGrandParentPC ()
{
	void *		pc_array[6];
	int			trace_size;

	if (jr_trace_level (jr_malloc_calls) > 1  ||  jr_trace_level (jr_malloc_trap) > 1) {

		trace_size	= jr_backtrace (pc_array, sizeof (pc_array) / sizeof (void *));
		/*
		 * 3/31/05: pc_array[0] is the line above, pc_array[1] is caller's line.
		 * so pc_array[2] is caller's parent, and pc_array[3] is caller's grand parent.
		 */

		if (jr_malloc_called_by_new()) {
			if (trace_size > 4) {
				return pc_array[4];
			}
		}
		else {
			if (trace_size > 3) {
				return pc_array[3];
			}
		}
	}
	return 0;
}


#ifdef ostype_linux

#	ifndef INTERNAL_SIZE_T
#	define INTERNAL_SIZE_T size_t
#	endif

	struct malloc_chunk
	{
	  INTERNAL_SIZE_T prev_size; /* Size of previous chunk (if free). */
	  INTERNAL_SIZE_T size;      /* Size in bytes, including overhead. */
	  struct malloc_chunk* fd;   /* double links -- used only if free. */
	  struct malloc_chunk* bk;
	};

	typedef struct malloc_chunk* mchunkptr;

#	define NAV             128   /* number of bins */

#	define mutex_t			jr_int


	typedef struct malloc_chunk* mbinptr;

	/* An arena is a configuration of malloc_chunks together with an array
	   of bins.  With multiple threads, it must be locked via a mutex
	   before changing its data structures.  One or more `heaps' are
	   associated with each arena, except for the main_arena, which is
	   associated only with the `main heap', i.e.  the conventional free
	   store obtained with calls to MORECORE() (usually sbrk).  The `av'
	   array is never mentioned directly in the code, but instead used via
	   bin access macros. */

	typedef struct _arena {
	  mbinptr av[2*NAV + 2];
	  struct _arena *next;
	  size_t size;
#if THREAD_STATS
	  long stat_lock_direct, stat_lock_loop, stat_lock_wait;
#endif
	  mutex_t mutex;
	} arena;

#endif

void jr_MallocDummyFunction ()
{
#	ifdef ostype_linux
		mchunkptr			dummy_ptr;
#	else
		void *				dummy_ptr;
#	endif

	printf( "%d\n", (jr_int) sizeof(dummy_ptr));
}
