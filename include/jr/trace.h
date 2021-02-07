#ifndef _jr_traceh
#define _jr_traceh

#include "ezport.h"


#define		jr_malloc_calls_TRACE			10
#define		jr_malloc_trap_TRACE			11
#define		jr_malloc_bounds_TRACE			12
#define		jr_malloc_stats_TRACE			13
#define		jr_malloc_trace_TRACE			14

#define		jr_io_TRACE						20
#define		jr_string_TRACE					21
#define		jr_network_TRACE				22
#define		jr_exceptions_TRACE				23
#define		jr_locks_TRACE					24

#define		jr_graphics_TRACE				30
#define		jr_cgi_TRACE					31
#define		jr_nfa_dfa_TRACE				32
#define		jr_fcache_TRACE					33
#define		jr_macro_preprocessor_TRACE		34
#define		jr_build_update_TRACE			35
#define		jr_http_TRACE					36

#define		jr_matrix_TRACE					40
#define		jr_matrix_regions_TRACE			41
#define		jr_static_matrix_TRACE			42

#define		g2c_TRACE						50
#define		g2c_repair_TRACE				51
#define		g2c_parser_TRACE				52
#define		s2c_TRACE						53

#define		otree_file_TRACE				60
#define		otree_server_TRACE				61
#define		otree_client_TRACE				62
#define		otree_adapter_TRACE				63

#define		jr_TRACE_NUM_VALUES				100


typedef		char jr_trace_t;

jr_EXTERN(jr_trace_t) jr_TraceValues[];

#define jr_set_trace(t)						(	jr_check_trace_macro(t##_TRACE),					\
												jr_TraceValues[t##_TRACE] = 1						\
											)
#define jr_rm_trace(t)						(jr_TraceValues[t##_TRACE] = 0)
#define jr_set_trace_level(t, level)		(	jr_check_trace_macro(t##_TRACE),					\
												jr_TraceValues[t##_TRACE] |= 1 << ((level)-1)		\
											)

#ifdef NDEBUG
#define jr_NO_TRACE
#endif

#ifdef jr_NO_TRACE
	/*
	 * When compiled with an optimizing compiler conditional statements
	 * using "if (jr_do_trace(category)" will be optimized out of the executable
	 */
#	define jr_do_trace(t)					(0)
#	define jr_trace_level(t)				(0)
#	define jr_max_trace_level(t)			(0)
#	define jr_is_trace_at_level(t, level)	(0)
#	define jr_has_trace(t, level)			(0)
#else
#	define jr_do_trace(t)					(jr_TraceValues[t##_TRACE])
#	define jr_trace_level(t)				(jr_TraceValues[t##_TRACE])
#	define jr_max_trace_level(t)			jr_trace_bit (jr_TraceValues[t##_TRACE])
#	define jr_is_trace_at_level(t, level)	((level) > 0											\
												? jr_TraceValues[t##_TRACE] >= (1 << ((level) - 1))	\
												: jr_TraceValues[t##_TRACE] == 0)

#	define jr_has_trace(t, level)			((level) > 0											\
												? jr_TraceValues[t##_TRACE] & (1 << ((level) - 1))	\
												: 0)
#endif


#ifdef compiler_has_ansi_support
#	define				jr_check_trace_macro(t)		(jr_check_trace(t, __FILE__, __LINE__))
#else
#	define				jr_check_trace_macro(t)		(jr_check_trace (0, 0, 0))
						/*
						** no quoting macro args under non-ansi C, so no checking possible
						*/
#endif


jr_EXTERN(void)			jr_check_trace 		PROTO((
							jr_int			trace_num,
							const char *	file_name,
							jr_int			line_number
						)) ;

jr_EXTERN(jr_int)		jr_trace_bit		(jr_int	trace_value);


#define					jr_trace_foreach_level(t, k)									\
						for ((k)=1;  (k) <= (jr_int) sizeof (jr_int) * 8;  (k)++)		\
							if (jr_trace_level (t)  &  (1 << (k) - 1))

#endif
