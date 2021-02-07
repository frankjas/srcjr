#ifndef _local_jr_malloc_h
#define _local_jr_malloc_h

#define _POSIX_C_SOURCE 200809L

#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "jr/error.h"
#include "jr/trace.h"
#include "jr/htable.h"
#include "jr/misc.h"
#include "jr/error.h"
#include "jr/syscalls.h"
#include "jr/apr.h"

#include "jr/malloc.h"



/***** OverHead Type and Pointer Sizes *****/

#ifdef has_malloc_size
#	ifdef ostype_linux
#		define jr_malloc_usable_size_macro(ptr, amt)		malloc_usable_size ((void *) ptr)
#	else
#		define jr_malloc_usable_size_macro(ptr, amt)		malloc_size ((void *) ptr)
#	endif

#	define jr_malloc_may_have_slop
#else

#	define jr_malloc_usable_size_macro(ptr, amt)			(amt)

#endif




/******** Trap and Other Diagnostic Support ********/

#define				MAX_BOOKKEEPING_MALLOCS 				64
#define				MAX_MALLOC_BUCKETS          			50 
					/*
					 * the maximum allowed, keep in mind the user can add buckets at will
					 */

#define				SUFFIX_BOUNDS_STRING					"BND "
#define				PREFIX_BOUNDS_STRING					"PBND"
#define				FIXED_BOUNDS_MARKER_SIZE				4
					/*
					 * the chars BND plus 1 for the null
					 *
					 * 3/23/07: can't be less than the jr_MallocSizeStruct, because:
					 * we use the prefix on bookkeeping pointers to store the size.
					 * Why? Here's the case:
					 * - malloc_calls isn't on, but recording ptrs only
					 * -> bookkeeping pointers are using bounds memory
					 * -> jr_malloc_usable_size() would return junk on bookkeeping pointers
					 *
					 * Note that bookkeeping always uses bounds memory, because when freeing
					 * a bookkeeping pointer we can't look it up to figure out whether it uses
					 * bounds or not, so it needs to be either always there or not.
					 * Note also that bookkeeping pointers don't verify the bounds, so we
					 * can put other information there.
					 *
					 * ToDo: on systems where we don't prepend the size, this will break.
					 * Such systems should generate a syntax error since
					 * jr_malloc_requested_size() will be undefined.
					 */

#define				jr_malloc_suffix_marker_char(i)			(SUFFIX_BOUNDS_STRING[(i) % 4])
#define				jr_malloc_prefix_marker_char(i)			(PREFIX_BOUNDS_STRING[(i) % 4])


#define				jr_MALLOC_MAX_BOUNDS_MARKER_MULTIPLE	8
					/*
					** 4/14/07: default is 4 bytes
					** multiple of 1 is 8 == 4*2, 2 is 16 == 4*4, 3 is 32, 5 is 128 == 4*32, etc.
					**                       4*2^1           4*2^2                     4*2^5
					*/

#define				jr_malloc_is_suffix_same(v)													\
					(jr_MallocSuffixBoundsMarkerMultiple	!= jr_get2power(v))

#define				jr_malloc_is_prefix_same(v)													\
					(jr_MallocPrefixBoundsMarkerMultiple	!= jr_get2power(v))

#define				jr_malloc_set_suffix_multiple(v)											\
					((v) > 0																	\
						? jr_MallocSuffixBoundsMarkerMultiple	= jr_get2power(v)				\
						: (unsigned jr_int) (v)													\
					)

#define				jr_malloc_set_prefix_multiple(v)											\
					((v) > 0																	\
						? jr_MallocPrefixBoundsMarkerMultiple	= jr_get2power(v)				\
						: (unsigned jr_int) (v)													\
					)

#define				jr_malloc_prefix_bounds_marker_size()										\
					(jr_MallocPrefixBoundsMarkerMultiple > 0									\
						? jr_MallocPrefixBoundsMarkerMultiple * FIXED_BOUNDS_MARKER_SIZE		\
						: FIXED_BOUNDS_MARKER_SIZE												\
					)
					/*
					 * 7/23/2003 Has to be constant, since given a malloc pointer, we need
					 * to be able to find the requested size part, i.e. skip over the prepended
					 * bounds.  So the prepended bounds size can't depend on the requested size.
					 */

#define				jr_malloc_suffix_slop_divisor()			(1)
					/*
					** 4/8/07: if you want to use the requested_size as the bounds marker,
					** do the following:
					** - set the slop_divisor() to (jr_MallocSuffixBoundsMarkerMultiple + 1)
					** - make the suffix_bounds_marker() condition "requested_size" instead of 0 below.
					** - set suffix_multiple() to just "1".
					** -- could also set to "v" but this will increase total memory usage by that factor
					** -- getting a decent prefix bounds (32 bytes) would bump total memory too much
					** -- or require programs to set them separately, requiring another trace arg
					*/
					
#define				jr_malloc_suffix_bounds_marker_size(requested_size)							\
					(jr_MallocSuffixBoundsMarkerMultiple > 0									\
						? jr_MallocSuffixBoundsMarkerMultiple * 								\
							(0 ? requested_size : FIXED_BOUNDS_MARKER_SIZE)						\
						: FIXED_BOUNDS_MARKER_SIZE												\
					)


#define				jr_DESTRUCTIVE_FREE_STRING			"jr_free jr_free jr_free jr_free jr_free "
					/*
					 * Note the 8 char pattern, which generates regular values
					 * when viewed as an integer or pointer.
					 */


extern void			jr_malloc_destroy_data				PROTO ((
						void *							mptr,
						size_t							num_bytes
					));

extern jr_int		jr_malloc_record_ptrs_only ();

/******** Malloc Pointer State Information ********/

typedef struct {
	const void *		ptr;
	size_t				amt_requested;
	const void *		calling_pc;
	const void *		heap_base_ptr;
	jr_int				occurrence;
	unsigned			is_malloced				: 1;
	unsigned			is_bookkeeping			: 1;
	unsigned			has_bounds_marker		: 1;
} jr_MallocPtrInfoStruct;

extern void			jr_MallocPtrInfoInit			PROTO ((
						jr_MallocPtrInfoStruct *		mptr_info,
						const void *					mptr,
						size_t							amt_requested,
						const void *					calling_pc,
						const void *					heap_base_ptr,
						jr_int							is_malloc,
						jr_int							is_bookkeeping,
						jr_int							has_bounds_marker
					));

extern char *		jr_MallocPtrInfoFormatString		PROTO ((
						jr_MallocPtrInfoStruct *		ptr_info,
						char *							sbuf,
						jr_int							sbuf_size
					));


extern void			jr_MallocPtrInfoPrint				PROTO ((
						jr_MallocPtrInfoStruct *		ptr_info,
						FILE *							wfp
					));

extern jr_int		jr_MallocPtrInfoHash				PROTO ((
						const void *					void_arg_1
					));

extern jr_int		jr_MallocPtrInfoCmp					PROTO ((
						const void *					void_arg_1,
						const void *					void_arg_2
					));

extern jr_int		jr_MallocPtrInfoCheckBounds			PROTO ((
						jr_MallocPtrInfoStruct *		ptr_info,
						char *							error_buf
					));

#define				jr_MallocPtrInfoPtr(ptr_info)				((ptr_info)->ptr)
#define				jr_MallocPtrInfoOccurence(ptr_info)			((ptr_info)->occurrence)
#define				jr_MallocPtrInfoRequestedSize(ptr_info)		((ptr_info)->amt_requested)
#define				jr_MallocPtrInfoCallingPC(ptr_info)			((ptr_info)->calling_pc)
#define				jr_MallocPtrInfoHeapBase(ptr_info)			((ptr_info)->heap_base_ptr)
#define				jr_MallocPtrInfoIsMalloced(ptr_info)		((ptr_info)->is_malloced)
#define				jr_MallocPtrInfoIsBookkeeping(ptr_info)		((ptr_info)->is_bookkeeping)
#define				jr_MallocPtrInfoHasBoundsMarker(ptr_info)	((ptr_info)->has_bounds_marker)
#define				jr_MallocPtrInfoIsFreed(ptr_info)			(! (ptr_info)->is_malloced)

#define				jr_MallocPtrInfoSetRequestedSize(ptr_info, v)	((ptr_info)->amt_requested	= (v))
#define				jr_MallocPtrInfoSetCallingPC(ptr_info, v)		((ptr_info)->calling_pc	= (v))
#define				jr_MallocPtrInfoSetHeapBase(ptr_info, v)		((ptr_info)->heap_base_ptr	= (v))
#define				jr_MallocPtrInfoSetHasBoundsMarker(ptr_info, v) ((ptr_info)->has_bounds_marker = (v)!= 0)

#define				jr_MallocPtrInfoMakeMalloced(ptr_info)		((ptr_info)->is_malloced		= 1)
#define				jr_MallocPtrInfoMakeBookkeeping(ptr_info)	((ptr_info)->is_bookkeeping		= 1)

#define				jr_MallocPtrInfoClearMalloced(ptr_info)		((ptr_info)->is_malloced		= 0)
#define				jr_MallocPtrInfoClearBookkeeping(ptr_info)	((ptr_info)->is_bookkeeping		= 0)



/***** Malloc Stats *****/

typedef struct {
	jr_int				nbuckets;
	jr_int				did_initialization;

	size_t				s_bucket[MAX_MALLOC_BUCKETS+1];
	jr_int				n_malloc[MAX_MALLOC_BUCKETS+1] ;
	jr_int				n_freedp[MAX_MALLOC_BUCKETS+1] ;
	jr_int				n_maxnum[MAX_MALLOC_BUCKETS+1] ;
} jr_MallocStatStruct;

extern void			jr_MallocStatInit				PROTO ((
						jr_MallocStatStruct *		msp
					));

extern void			jr_MallocStatUndo				PROTO ((
						jr_MallocStatStruct *		msp
					));

extern void			jr_MallocStatReset				PROTO ((
						jr_MallocStatStruct *		msp
					));


extern void			jr_MallocStatPrint  			PROTO ((
						FILE *						wfp,
						jr_MallocStatStruct *		msp,
						char *						message
					)) ;

extern jr_int		jr_MallocStatNumAllocated		PROTO ((
						jr_MallocStatStruct *		msp
					));

extern void			jr_MallocStatRecordMalloc 		PROTO ((
						jr_MallocStatStruct *		msp,
						void *						mptr,
						unsigned jr_int				amt_allocated
					)) ;

extern void			jr_MallocStatRecordFree   		PROTO ((
						jr_MallocStatStruct *		msp,
						const void *				mptr,
						unsigned jr_int				amt_allocated
					)) ;

extern void			jr_MallocSetBucketSizes			PROTO ((unsigned jr_int *s_bucket));

extern void			jr_MallocStatAddBucketSize  	PROTO ((
						jr_MallocStatStruct *		msp,
						size_t						bucket_size
					)) ;


/***** Bucket Sizes (derived once experimentally) *****/

#define				MALLOC_BUCKET_SIZES \
					{16,32,64,128,252,340,508,680,1020,1360, 2044,2724,4088,8192,16384,24576,32768, 0}

#define				jr_MallocStatBucketSize(msp, b)	((msp)->s_bucket[b])

extern void *		jr_MallocGetGrandParentPC		PROTO ((void));


/******** Malloc Diag Stuff ********/

typedef struct {
	jr_int		did_initialization;
	jr_int		doing_initialization;

	void *		TrapPtr;
	jr_int		TrapOnOccurence;
	jr_int		TrapPtrAmount;
	jr_int		TrapPtrMallocIndexIsBookkeeping;

	jr_int		CurrOccurence;

	/*
	 * allocation checking fields
	 */
	jr_HTable	PointerTable[1];
	jr_int		DoingBookkeeping;

	jr_int		NextBookkeepingIndex;
	jr_int		FirstBookkeepingIndex;
	jr_int		BookkeepingArrayOverflow;


	jr_MallocPtrInfoStruct BookkeepingArray [MAX_BOOKKEEPING_MALLOCS];
	/*
	 * we are using an jr_HTable to store all the bookkeeping info
	 * this data structure grows dynamically, and as such will need
	 * to call jr_malloc while doing bookkeeping.  The above
	 * fields keep track off whether we are doing bookkeeping
	 * If we are bookkeeping, then temporarily put the pointers 
	 * being jr_malloced for the bookkeeping data structures
	 * in a book keeping array
	 */

	void *		heap_base_ptr;

	jr_int *	exempt_fd_array;

	FILE *		trace_wfp;

	unsigned	did_trace_file_empty			: 1;
} jr_MallocDiagStruct;

extern void  		jr_MallocDiagInit			PROTO((jr_MallocDiagStruct *mdp));
extern void  		jr_MallocDiagUndo			PROTO((jr_MallocDiagStruct *mdp));
extern void  		jr_MallocDiagReset			PROTO((jr_MallocDiagStruct *mdp));

extern void			jr_MallocDiagPrintTrace (
						jr_MallocDiagStruct *		mdp,
						jr_MallocPtrInfoStruct *	ptr_info);

extern void			jr_MallocDiagInitExemptDescriptorArray	PROTO ((
						jr_MallocDiagStruct *	mdp
					));

extern jr_int		jr_MallocDiagNumOpenFiles		PROTO ((
						jr_MallocDiagStruct *		mdp,
						FILE *						wfp,
						char *						error_buf
					));

extern void  		jr_MallocDiagDump   			PROTO((jr_MallocDiagStruct *mdp));

extern const char *	jr_MallocDiagPtrType     		PROTO((
						jr_MallocDiagStruct *		mdp,
						const void *				ptr
					));



extern void			jr_MallocDiagDoTrap				PROTO ((
						jr_MallocDiagStruct *		mdp,
						const void *				mptr,
						jr_int						is_malloc
					));

extern void			jr_MallocDiagRecordInfo			PROTO ((
						jr_MallocDiagStruct *		mdp,
						const void *				mptr,
						size_t						amt_requested, 
						jr_int						is_malloc,
						jr_int						is_bookkeeping,
						jr_int						is_ptr_only,
						size_t *					amt_ref,
						jr_int *					has_bounds_ref
					));

#define				jr_MallocDiagDoMallocTrap(mdp, mptr) \
					jr_MallocDiagDoTrap (mdp, mptr, 1)

#define				jr_MallocDiagDoFreeTrap(mdp, mptr) \
					jr_MallocDiagDoTrap (mdp, mptr, 0)

#define				jr_MallocDiagRecordMalloc(mdp, mptr, amt, ptr_only) \
					jr_MallocDiagRecordInfo (mdp, mptr, amt, 1, 0, ptr_only, 0, 0)

#define				jr_MallocDiagRecordFree(mdp, mptr, amt_ref, has_bounds_ref) \
					jr_MallocDiagRecordInfo (mdp, mptr, 0, 0, 0, 0, amt_ref, has_bounds_ref)



extern jr_MallocPtrInfoStruct *jr_MallocDiagFindPtrInfo	PROTO ((
						jr_MallocDiagStruct *			mdp,
						const void *					mptr
					));

extern jr_MallocPtrInfoStruct *jr_MallocDiagNewPtrInfo	PROTO ((
						jr_MallocDiagStruct *			mdp,
						const void *					mptr,
						size_t							amt_requested,
						const void *					calling_pc,
						const void *					heap_base_ptr,
						jr_int							is_malloced,
						jr_int							is_bookkeeping,
						jr_int							has_bounds_marker
					));

extern jr_int		jr_MallocDiagCheckLeaksAndBounds	PROTO ((
						jr_MallocDiagStruct *		mdp,
						char *						error_buf
					));

extern jr_int		jr_MallocDiagNumLeaks			PROTO ((
						jr_MallocDiagStruct *		mdp
					));

extern void			jr_MallocDiagSetSizeRequested	PROTO ((
						jr_MallocDiagStruct *		mdp,
						void *						mptr,
						size_t						usable_size
					));

#define				jr_MallocDiagIsDoingBookkeeping(mdp)			\
					((mdp)->DoingBookkeeping)

#define				jr_MallocDiagTrapPtr(mdp)						\
					((mdp)->TrapPtr)


/******** Malloc Pointer Interface (uses global MallocDiag state) ********/

extern jr_int		jr_MallocPtrDoBounds			PROTO ((
						const void *				mptr,
						size_t						amt_requested,
						jr_int						for_malloc,
						char *						error_buf
					));

#define				jr_MallocPtrDoBoundsCheck(mptr, amt_requested, error_buf)				\
					jr_MallocPtrDoBounds (mptr, amt_requested, 0, error_buf)

extern void			jr_MallocPtrMakeTrapFile			PROTO ((
															const void *pointer,
															const char *error_mesg
														));



extern void			jr_MallocPtrPrintInfo				PROTO ((void *pointer));
extern void			jr_MallocPtrDumpPrevPtrInfo			PROTO ((void *pointer));
extern size_t		jr_MallocPtrSizeRequested			PROTO ((void *pointer));


extern jr_int		jr_MallocCheckBounds				PROTO ((void));
extern jr_int		jr_MallocPrintOpenFiles				PROTO ((void));
extern void			jr_MallocPrintFunctions				PROTO ((void));

extern jr_int		jr_MallocPtrCheckBounds				PROTO ((const void *mptr));

#endif
