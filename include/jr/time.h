#ifndef _timeh
#define _timeh

#include "ezport.h"

#include "jr/long.h"

/*
** 7/31/06: Provide a portable time interface
** Define a jr_time_t to be an integer type of some kind, i.e. can be passed by value,
** compared to other integer types.
*/

typedef jr_int			jr_seconds_t;			/* 2/20/07: seconds since 1/1/70 */
typedef jr_ULong		jr_useconds_t;			/* 2/20/07: microseconds since 1/1/70 */


#define					jr_SECONDS_MAX			jr_INT_MAX

typedef struct {
	jr_int				second;
	jr_int				minute;
	jr_int				hour;
	jr_int				month_day;
	jr_int				month;
	jr_int				year;
	jr_int				week_day;
	jr_int				year_day;
	jr_int				gmt_off;
	unsigned			is_dst	: 1;
	unsigned			is_gmt	: 1;
} jr_TimeStruct;


#define					jr_TIME_MAX						jr_INT_MAX
#define					jr_CTIME_STRING_LENGTH			32
						/*
						** According to docs, should be at least 25 bytes.
						*/

extern jr_seconds_t		jr_time_seconds (void);
extern jr_useconds_t	jr_time_useconds (void);

extern jr_int			jr_time_gmt_offset();

extern unsigned jr_int	jr_useconds_to_seconds (
							jr_useconds_t				useconds);

extern jr_useconds_t	jr_seconds_to_useconds(
							jr_seconds_t				seconds);

#define					jr_useconds_diff( ulong_ptr, t1, t2 )							\
						jr_ULongSubtract( ulong_ptr, t1, t2 )

extern char *			jr_ctime (
							jr_seconds_t				curr_time_seconds,
							char *						buf);
							/*
							** 4/11/09: no trailing newline
							*/

extern void				jr_TimeInit (
							jr_TimeStruct *				time_ptr,
							jr_seconds_t				time_seconds);

extern jr_int			jr_TimeInitFromString (
							jr_TimeStruct *				time_ptr,
							const char *				time_string,
							char *						error_buf);

#define					jr_TIME_STRING_LENGTH			32


						/*
						** 7/31/06: understands RFC 822 as updated by 1123 (i.e. w/2 digit day)
						** Sun, 06 Nov 1994 08:49:37 GMT
						*/
extern char *			jr_TimeToString (
							jr_TimeStruct *				time_ptr,
							char *						string,
							jr_int						max_length);

extern jr_int			jr_TimeToSecondsUTC (
							jr_TimeStruct *				time_ptr);


#define					jr_SECONDS_PER_DAY				(60 * 60 * 24)
#define					jr_SECONDS_PER_YEAR				(jr_SECONDS_PER_DAY * 365)

extern void				jr_useconds_init (
							jr_useconds_t *				interval_ptr,
							unsigned jr_int				seconds,
							unsigned jr_int				useconds);

extern void				jr_useconds_subtract(
							jr_useconds_t *				dest_ref,
							jr_useconds_t				v1,
							jr_useconds_t				v2);

#define					jr_useconds_asgn( dest_ref, src )	\
						jr_ULongAsgn( dest_ref, src )

#define					jr_useconds_add( dest_ref, v1, v2 )	\
						jr_ULongAdd( dest_ref, v1, v2 )

#define					jr_useconds_is_zero( value )	\
						jr_ULongIsZero( value )

#define					jr_useconds_cmp( v1, v2 )			\
						jr_ULongCmp( v1, v2 )

#define					jr_useconds_to_64(interval_ptr)		\
						jr_ULongToHost64 (*(interval_ptr))

#endif
