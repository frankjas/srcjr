#ifndef _jr_malloch
#define _jr_malloch

#include "ezport.h"

#include <stdio.h>  	/* required for args to jr_MallocStatPrint */

jr_EXTERN(void *)		jr_malloc					PROTO ((size_t amt_requested)) ;
jr_EXTERN(void *)		jr_calloc					PROTO ((size_t num_elem, size_t elem_size)) ;

#if defined (realloc) || defined (free)  ||  defined (jr_redefined_malloc)
	/*
	 * 'realloc' was most likely defined as 'jr_realloc', so the 'const' pointer declaration
	 * conflicts with the standard libraries definition
	 */
	jr_EXTERN(void *)	jr_realloc					PROTO ((void *old_ptr, size_t new_size)) ;
	jr_EXTERN(void)		jr_free						PROTO ((void *mptr)) ;
#else
	jr_EXTERN(void *)	jr_realloc					PROTO ((const void *old_ptr, size_t new_size)) ;
	jr_EXTERN(void)		jr_free						PROTO ((const void *mptr)) ;
#endif

jr_EXTERN(char *)		jr_strdup					PROTO ((const char *str));
jr_EXTERN(char *)		jr_strndup					PROTO ((const char *str, size_t max_length));

jr_EXTERN(void *)		jr_memdup					PROTO ((const void *, size_t)) ;

jr_EXTERN(void)			jr_exit						(jr_int status);
jr_EXTERN(void)			jr_exit_set_program_name	(const char *program_name);

jr_EXTERN(const char *)	jr_ProgramName;


extern size_t			jr_malloc_usable_size		PROTO ((const void *mptr, size_t amt_requested));
extern void				jr_malloc_good_size			PROTO ((size_t new_size));
extern void				jr_malloc_stats				PROTO ((FILE *wfp, char *message)) ;
extern void				jr_malloc_dump				PROTO ((void));
extern const char *		jr_malloc_type				PROTO ((const void *ptr));

extern jr_int			jr_malloc_check_leaks_and_bounds		PROTO ((char *error_buf));
extern jr_int			jr_malloc_num_open_files	PROTO ((FILE *opt_wfp, char *error_buf));

extern const void *		jr_malloc_check_bounds		PROTO ((char *error_buf));
extern void				jr_malloc_make_trap_file	PROTO ((const void *pointer));
extern int				jr_malloc_is_trap_ptr		(const void *mptr);


extern void				jr_malloc_reset_calls		PROTO ((void));
extern void				jr_malloc_reset_stats		PROTO ((void));


jr_EXTERN(void)			jr_malloc_set_bounds_marker_multiple	PROTO ((
							jr_int							bounds_marker_multiple
						));

extern void				jr_malloc_set_no_memory_handler (
							void							(*handler_fn) (size_t));

extern unsigned jr_int	jr_malloc_max_request_size (void);
jr_EXTERN(void)			jr_malloc_set_max_request_size (unsigned jr_int v);

extern jr_int			jr_malloc_has_clear_on_free (void);
jr_EXTERN(void)			jr_malloc_set_clear_on_free (jr_int v);

extern unsigned jr_int	jr_malloc_num_calls (void );
extern void				jr_malloc_set_num_calls (unsigned jr_int v);

extern unsigned jr_int	jr_malloc_num_unrecorded_calls (void);


						/*
						** 3/20/08: to correctly account for C++ dynamic initialization of statics
						** and globals, place a call to:
						**
						** 		jr_malloc_begin_cpp_trans_unit()
						**
						** at the top of every C++ translation unit that performs dynamic 
						** allocation as part of static/global object construction.
						** For dynamically loaded translations units, call:
						**
						**		jr_malloc_end_cpp_trans_unit()
						**
						** after all static/global variable declarations.
						**
						** Then calls to jr_malloc_dump(), and jr_malloc_check_leaks_and_bounds()
						** will be delayed until the last C++ translation unit has been deconstructed.
						**
						*/

#define					jr_malloc_begin_cpp_trans_unit()										\
						class jr_malloc_cpp_begin_trans_unit_t {								\
							public :															\
								jr_malloc_cpp_begin_trans_unit_t ()								\
								{																\
									jr_malloc_begin_cpp_trans_unit_constructor ();				\
								}																\
								~jr_malloc_cpp_begin_trans_unit_t ()							\
								{																\
									jr_malloc_begin_cpp_trans_unit_destructor ();				\
								}																\
						};																		\
						static jr_malloc_cpp_begin_trans_unit_t jr_MallocCppBeginTransUnit		\

#define					jr_malloc_end_cpp_trans_unit()											\
						class jr_malloc_cpp_end_trans_unit_t {									\
							public :															\
								jr_malloc_cpp_end_trans_unit_t ()								\
								{																\
									jr_malloc_end_cpp_trans_unit_constructor ();				\
								}																\
						};																		\
						static jr_malloc_cpp_end_trans_unit_t jr_MallocCppEndTransUnit			\


jr_EXTERN(void)			jr_malloc_begin_cpp_main();

jr_EXTERN(void)			jr_malloc_begin_cpp_trans_unit_constructor ();
jr_EXTERN(void)			jr_malloc_begin_cpp_trans_unit_destructor ();
jr_EXTERN(void)			jr_malloc_end_cpp_trans_unit_constructor ();
jr_EXTERN(jr_int)		jr_malloc_num_cpp_trans_units();

jr_EXTERN(jr_int)		jr_malloc_dump_was_called();


#ifndef					jr_MALLOC_NO_MEMORY_EXCEPTION
#	define				jr_MALLOC_NO_MEMORY_EXCEPTION			"jr_malloc:no memory"
#endif
#ifndef					jr_MALLOC_TRAP_FILE_NAME
#	define				jr_MALLOC_TRAP_FILE_NAME				"trap.jr_mem"
#endif
#ifndef					jr_MALLOC_TRACE_FILE_NAME
#	define				jr_MALLOC_TRACE_FILE_NAME				"trace.jr_mem"
#endif
#ifndef					jr_MALLOC_DEFAULT_MAX_REQUEST
#	define				jr_MALLOC_DEFAULT_MAX_REQUEST			jr_UINT_MAX
#endif
#ifndef					jr_MALLOC_MAX_LEAK_TRAP_PRINT
#	define				jr_MALLOC_MAX_LEAK_TRAP_PRINT			20
#endif
#ifndef					jr_MALLOC_BEGIN_CPP_INIT_VALUE
#	define				jr_MALLOC_BEGIN_CPP_INIT_VALUE			0
#endif


/*
** 3/23/07: The following should be converted to use a function call-based API
** to avoid MS DLL export/import issues.
*/


extern jr_int			jr_MallocOtherResourceLeak;
extern jr_int			jr_MallocMaxPrintBucketVar;
extern char *			jr_MallocTrapFileName;
extern char *			jr_MallocTraceFileName;
extern unsigned jr_int	jr_MallocSuffixBoundsMarkerMultiple;
extern unsigned jr_int	jr_MallocPrefixBoundsMarkerMultiple;
extern jr_int			jr_MallocMaxLeakTrapPrint;
extern size_t			jr_MallocBytesInUse;
extern size_t			jr_MallocMaxBytesInUse;
extern void				(*jr_MallocNoMemoryHandler) (size_t);
extern jr_int			jr_MallocCalledByNew;
jr_EXTERN(jr_int)		jr_MallocRecordPtrsOnly;

#define					jr_malloc_bytes_in_use()				(jr_MallocBytesInUse)
#define					jr_malloc_max_bytes_in_use()			(jr_MallocMaxBytesInUse)
#define					jr_malloc_max_leak_trap_print()			(jr_MallocMaxLeakTrapPrint)
#define					jr_malloc_has_resource_leak()			(jr_MallocOtherResourceLeak)
#define					jr_malloc_max_print_bucket()			(jr_MallocMaxPrintBucketVar)
#define					jr_malloc_called_by_new()				(jr_MallocCalledByNew)

#define					jr_malloc_set_trap_file_name(str)		(jr_MallocTrapFileName = (str))
#define					jr_malloc_set_trace_file_name(str)		(jr_MallocTraceFileName = (str))
#define					jr_malloc_set_resource_leak()			(jr_MallocOtherResourceLeak = 1)
#define					jr_malloc_clear_resource_leak()			(jr_MallocOtherResourceLeak = 0)
#define					jr_malloc_set_max_leak_trap_print(v)	(jr_MallocMaxLeakTrapPrint = (v))
#define					jr_malloc_no_memory_handler()			(jr_MallocNoMemoryHandler)
#define					jr_malloc_set_max_print_bucket(v)		(jr_MallocMaxPrintBucketVar = (v))
#define					jr_malloc_set_called_by_new(v)			(jr_MallocCalledByNew = (v) != 0)


#endif
