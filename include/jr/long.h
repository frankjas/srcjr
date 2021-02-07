#ifndef _long_long_h__
#define _long_long_h__

#include "ezport.h"


/*
 * struct assignment and parameter passing
 * is allowed by most non-ANSI compilers.
 * K&R edition 1 even states (pg 209) that
 * allow forbidden, will probably be allowed
 * in the future.
 *
 * This is good news since it is probably most
 * practical to use the following types as if
 * they were built-in numerical types 
 * and to pass them by value.
 *
 * It is therefore assumed that normal assignment and 
 * may be used to copy these types.
 */


/*
 * the jr_Long is a 64 bit quantity
 *
 * Check the word align macros in misc.h for porting
 * Perhaps they should be written with a 4 hard-wired.
 */

#ifdef jr_IS_64_BIT

/*
 * This code should be tested before used.
 */

typedef uint64_t jr_ULong;


#define jr_ULongSetMSW(longp, v)	(*(longp) = (((jr_ULong) (v) << 32) | *(longp) & 0xffffffffL))
#define jr_ULongSetLSW(longp, v)	(*(longp) = (*(longp) & 0xffffffff00000000L) | (v) & 0xffffffff)

#define jr_ULongSet(resultp, msw, lsw)								\
		(*(resultp) = (((jr_ULong) (msw) << 32) | (lsw) & 0xffffffffL))

#define jr_ULongMSW(longv)			((uint32_t) (((longv) & 0xffffffff00000000L) >> 32))
#define jr_ULongLSW(longv)			((uint32_t) ((longv) & 0xffffffffL))


#define jr_ULongAsgnUInt(longp, i)					(*(longp) = (unsigned jr_int) (i))
#define jr_ULongAsgn(longp1, long2)					(*(longp1) = (long2))
#define jr_ULongAsgnLong(longp1, long2)				(*(longp1) = (uint64_t)(long2))

#define jr_ULongAsgnNetULong(resultp, net_long)							\
		jr_ULongSet(resultp, jr_NetULongMSW (net_long), jr_NetULongLSW (net_long))

#define jr_ULongAsgnZero(longp)						jr_ULongAsgnUInt (longp, 0)


#define jr_ULongIsUInt(longv)						((longv) <= jr_UINT_MAX)
#define jr_ULongEqualsUInt(longv, i)				((longv) == (i))
#define jr_ULongIsZero(longv)						((longv) == 0)
#define jr_ULongIsNonZero(longv)					((longv) != 0)

#define jr_ULongIsMaxValue(longv)					((longv) == UINT64_MAX)
#define jr_ULongAsgnMaxValue(longp)					(*(longp) = UINT64_MAX)

#define jr_ULongToUInt(longv)						((unsigned jr_int) (longv))
#define jr_ULongToHost(longv)						((uint64_t) (longv))
#define jr_ULongAsgnHost(longp, o)					(*(longp) = (o))
#define jr_ULongToFloat(longv)						((float) (longv))
#define jr_ULongAsgnFloat(longp, v)					(*(longp) = (v))


#define jr_ULongAdd(result, long1, long2)			(*(result) = (long1) + (long2))
#define jr_ULongSubtract(result, long1, long2) 		(*(result) = (long1) - (long2))
			
#define jr_ULongAddUInt(result, long1, uint_v)		(*(result) = (long1) + (uint_v))
#define jr_ULongSubtractUInt(result, long1, uint_v)	(*(result) = (long1) - (uint_v))
#define jr_ULongModUInt(long1, uint_v)				((uint32_t)((long1) % (uint32_t)(uint_v)))
#define jr_ULongDivideUInt(result, long1, uint_v)	(*(result) = (long1) / (uint_v))
#define jr_ULongMultUInt(result, long1, uint_v)		(*(result) = (long1) * (uint_v))
#define jr_ULongShiftInt(result, long1, int_v)		((int_v) > 0								\
														?  (*(result) = (long1) >> (int_v))  	\
														:  (*(result) = (long1) << -(int_v)))

#define jr_ULongAsgn64(longp1, long2)				jr_ULongAsgn (longp1, long2)
#define jr_ULongToHost64(longv)						(longv)

#else

/*
 * long is 32 bits
 */

typedef struct {
	unsigned jr_int	u_msw;
	unsigned jr_int	u_lsw;
} jr_ULong;


#define jr_ULongSetMSW(longp, v)		((longp)->u_msw = (v))
#define jr_ULongSetLSW(longp, v)		((longp)->u_lsw = (v))

#define jr_ULongSet(resultp, msw, lsw)								\
		((resultp)->u_msw = (msw), (resultp)->u_lsw = (lsw))

#define jr_ULongMSW(longv)				((longv).u_msw)
#define jr_ULongLSW(longv)				((longv).u_lsw)


#define jr_ULongAsgnUInt(longp, i)		((longp)->u_msw = 0,  (longp)->u_lsw = (i))

#define jr_ULongAsgn(longp1, longv2)										\
		(																	\
			jr_ULongSetMSW (longp1, jr_ULongMSW (longv2)),					\
			jr_ULongSetLSW (longp1, jr_ULongLSW (longv2))					\
		)

#define jr_ULongAsgnLong(longp1, longv2)									\
		(																	\
			jr_ULongSetMSW (longp1, jr_LongMSW (longv2)),					\
			jr_ULongSetLSW (longp1, jr_LongLSW (longv2))					\
		)

#define jr_ULongAsgnNetULong(longp, net_long)								\
		(																	\
			jr_ULongSetMSW (longp, jr_NetULongMSW (net_long)),				\
			jr_ULongSetLSW (longp, jr_NetULongLSW (net_long))				\
		)

		/*
		** 1/1/07: ToDo: technically this shouldn't be necessary,
		** since if there are 64 bit integers, the other jr_ULong implementation should be used.
		*/
#define	jr_ULongAsgn64(longp, v)		(	(longp)->u_msw	= (v) >> 32,		\
											(longp)->u_lsw = (v) & 0xffffffffL	\
										)
#define jr_ULongToHost64(longv)			(((long long) jr_ULongMSW (longv) << 32)	\
										| (long long) jr_ULongLSW (longv))

#define jr_ULongIsUInt(longv)			(jr_ULongMSW (longv) == 0)
#define jr_ULongEqualsUInt(longv, i)	(jr_ULongIsUInt (longv)  &&  (longv).u_lsw == (i))
#define jr_ULongIsZero(longv)			jr_ULongEqualsUInt(longv, 0)
#define jr_ULongIsNonZero(longv)		(! jr_ULongIsZero (longv))

#define jr_ULongIsMaxValue(longv)								\
		(jr_ULongMSW (longv) == jr_UINT_MAX  &&  jr_ULongLSW (longv) == jr_UINT_MAX)

#define jr_ULongAsgnMaxValue(longp)								\
		(jr_ULongSetMSW (longp, jr_UINT_MAX),  jr_ULongSetLSW (longp, jr_UINT_MAX))

#define jr_ULongToUInt(longv)			(jr_ULongLSW (longv))
#define jr_ULongToHost(longv)			(jr_ULongLSW (longv))
#define jr_ULongAsgnHost(longp, o)		jr_ULongAsgnUInt (longp, o)

#define jr_ULongAdd(result, long1, long2) \
		(																			\
			jr_ULongAddUInt (result, long1, (long2).u_lsw),							\
			(result)->u_msw += (long2).u_msw										\
		)
			

#define jr_ULongSubtract(result, long1, long2) \
		(																			\
			jr_ULongSubtractUInt (result, long1, (long2).u_lsw),					\
			(result)->u_msw -= (long2).u_msw										\
		)
			
extern void				jr_ULongAddUInt				PROTO ((
								jr_ULong *			result,
								jr_ULong			long1,
								unsigned jr_int		mod_value
						));

extern void				jr_ULongSubtractUInt				PROTO ((
								jr_ULong *			result,
								jr_ULong			long1,
								unsigned jr_int		mod_value
						));

extern void				jr_ULongShiftInt			PROTO ((
								jr_ULong *			result,
								jr_ULong			long1,
								jr_int				shift_value
						));

extern jr_int			jr_ULongModUInt				PROTO ((
								jr_ULong			long1,
								unsigned jr_int		mod_value
						));

extern void				jr_ULongDivideUInt			PROTO ((
							jr_ULong *				result_ptr,
							jr_ULong				long1,
							unsigned jr_int			div_value
						));

extern void				jr_ULongMultUInt			PROTO ((
							jr_ULong *				result_ptr,
							jr_ULong				long1,
							unsigned jr_int			div_value
						));

extern void				jr_ULongAsgnUIntMult		PROTO ((
							jr_ULong *					result_ptr,
							unsigned jr_int				u,
							unsigned jr_int				v
						));

/*
 * jr_ULongMult() doesn't make sense, because if both values > 2^32,
 * the the result won't fit in a jr_ULong
 */

extern double			jr_ULongToFloat				PROTO ((
							jr_ULong				ulong_value
						));

extern void				jr_ULongAsgnFloat			PROTO ((
							jr_ULong *				ulong_value_ptr,
							double					float_value
						));

#endif

extern jr_int			jr_ULongCmp					PROTO ((
								jr_ULong			long1,
								jr_ULong			long2
						));

extern jr_int			jr_ULongCmpUInt				PROTO ((
								jr_ULong			long1,
								unsigned jr_int		uint_value
						));



#ifdef jr_IS_64_BIT

/*
 * This code should be tested before used.
 */

typedef int64_t jr_Long;

extern void				jr_LongSetMSW(
							jr_Long *				longp,
							jr_int					value);

extern void				jr_LongSetLSW(
							jr_Long *				longp,
							jr_int					value);

#define jr_LongMSW(longv)			((int32_t)(((longv) & 0xffffffff00000000L) >> 32))
#define jr_LongLSW(longv)			((int32_t)((longv) & 0xffffffff))


#define jr_LongAsgnInt(longp, i)					(*(longp) = (i))
#define jr_LongAsgn(longp1, long2)					(*(longp1) = (long2))
#define jr_LongAsgnULong(longp1, ulong2)			(*(longp1) = (ulong2))

#define jr_LongAsgnNetLong(resultp, net_long)							\
		(jr_NetLongMSW (net_long) == 0 									\
			? (*(resultp) = jr_NetLongLSW (net_long))					\
			: (															\
				*(resultp) = jr_NetLongLSW (net_long),					\
				jr_LongSetMSW (resultp, jr_NetLongMSW (net_long)),0		\
			)															\
		) /*
		 * If the msw == 0, then the lsw may be negative.  A regular assignment
		 * takes care of the sign extension.
		 * In the case msw !=, the lsw is not negative.  Use regular assignment
		 * first, which is cheaper the the SetLSW macro, and then OR in the msw
		 */


#define jr_LongIsInt(longv)							((longv) < jr_INT_MAX)
#define jr_LongEqualsInt(longv, i)					((longv) == (i))
#define jr_LongIsZero(longv)						((longv) == 0)
#define jr_LongIsNonZero(longv)						((longv) != 0)
#define jr_LongIsPositive(longv)					((longv) >= 0)
#define jr_LongIsNegative(longv)					((longv) < 0)

#define jr_LongIsMaxValue(longv)					((longv) == LONG_MAX)
#define jr_LongAsgnMaxValue(longp)					(*(longp) = LONG_MAX)


#define jr_LongToHost(longv)						(longv)
#define jr_LongAsgnHost(longp, v)					(*(longp) = v)

#define jr_LongAsgnAbsValue(longp, longv)			((longv) < 0							\
														? (*(longp) = -(longv))				\
														: (*(longp) = (longv)))

#define jr_LongAdd(result, long1, long2)			(*(result) = (long1) + (long2))
#define jr_LongSubtract(result, long1, long2) 		(*(result) = (long1) - (long2))
			
#define jr_LongAddInt(result, long1, uint_v)		(*(result) = (long1) + (uint_v))
#define jr_LongSubtractInt(result, long1, uint_v)	(*(result) = (long1) - (uint_v))
#define jr_LongShiftInt(result, long1, int_v)		((int_v) > 0							\
														?  *(result) = (long1) >> (int_v)  	\
														:  *(result) = (long1) << -(int_v))


#else

typedef struct {
	jr_int			msw;
	jr_int			lsw;
} jr_Long;

/*
 * We need to use a sign bit in each 32 bit quantity
 * in order to store the numbers -1 - -LONG_MIN
 *
 * the lsw will only be < 0 if msw == 0
 */


/*
** 5-15-2018: MSW, LSW should only be used for debugging with signed longs.
** It's not well defined wrt all combinations of signed/unsigned values, set in any order
** for native 64 bit implementations.
*/
#define jr_LongSetMSW(longp, v)		((longp)->msw = (v))
#define jr_LongSetLSW(longp, v)		((longp)->lsw = (v))

#define jr_LongMSW(longv)			((longv).msw)
#define jr_LongLSW(longv)			((longv).lsw)

#define jr_LongAsgn(long1, long2)		((long1)->msw = (long2).msw,  (long1)->lsw = (long2).lsw)
#define jr_LongAsgnInt(longp, i)		((longp)->msw = 0,  (longp)->lsw = (i))
#define jr_LongAsgnULong(long1, ulong2)	\
		((long1)->msw = (ulong2).u_msw, (long1)->lsw = (ulong2).u_lsw)

#define jr_LongAsgnNetLong(result, net_long)								\
		(																	\
			jr_LongSetMSW (result, jr_NetLongMSW (net_long)),				\
			jr_LongSetLSW (result, jr_NetLongLSW (net_long))				\
		)																	\

#define jr_LongAsgnZero(longp)		jr_LongAsgnInt (longp, 0)

#define jr_LongIsInt(longv)			((longv).msw == 0)
#define jr_LongEqualsInt(longv, i)	(jr_LongIsInt (longv)  &&  (longv).lsw == (i))


#define jr_LongIsZero(longv)		jr_LongEqualsInt(longv, 0)
#define jr_LongIsNonZero(longv)		(! jr_LongIsZero (longv))
#define jr_LongIsPositive(longv)	(! jr_LongIsNegative (longv))
#define jr_LongIsNegative(longv)	((longv).msw < 0  ||  (longv).lsw < 0)
		/*
		 * the lsw will only be < 0 if msw == 0
		 */

#define jr_LongIsMaxValue(longv)		((longv).msw == jr_INT_MAX  &&  (longv).lsw == jr_INT_MAX)
#define jr_LongAsgnMaxValue(longp)		((longp)->msw = jr_INT_MAX,  (longp)->lsw = jr_INT_MAX)

#define jr_LongIsMinValue(longv)		((longv).msw == jr_INT_MIN  &&  (longv).lsw == jr_INT_MAX)
#define jr_LongAsgnMinValue(longp)		((longp)->msw = jr_INT_MIN,  (longp)->lsw = jr_INT_MAX)


#define jr_LongToHost(longv)		(jr_LongLSW (longv))
#define jr_LongAsgnHost(longp, v)	jr_LongAsgnInt (longp, v)


#define jr_LongCmpIntMacro(long1, i)	\
		(						\
			(long1).msw ?		\
			(long1).msw :		\
			(long1).lsw - (i)	\
		)

extern jr_int jr_LongCmpInt		PROTO ((const jr_Long *	long1, long int_value));
extern jr_int jr_LongCmpRev		PROTO ((const jr_Long *	long1, const jr_Long *	long2));

extern void			jr_LongAddInt			PROTO ((
						jr_Long *			result,
						jr_Long				long1,
						jr_int				int_value
					));

#define jr_LongSubtractInt(result, long1, int_value) \
		jr_LongAddInt (result, long1, -(int_value))

extern void			jr_LongAdd				PROTO ((
						jr_Long *			result,
						jr_Long				long1,
						jr_Long				long2
					));

extern void			jr_LongSubtract			PROTO ((
						jr_Long *			result,
						jr_Long				long1,
						jr_Long				long2
					));


extern jr_int		jr_LongModUInt			PROTO ((
						jr_Long				long1,
						jr_int				mod_value
					));

extern void			jr_LongAsgnAbsValue		PROTO ((
						jr_Long *			result_ptr,
						jr_Long				long1
					));

extern void			jr_LongDivideUInt		PROTO ((
						jr_Long *			result_ptr,
						jr_Long				long1,
						jr_int				div_value
					));

extern void			jr_LongMultUInt			PROTO ((
						jr_Long *			result_ptr,
						jr_Long				long1,
						jr_int				div_value
					));

extern void			jr_LongAsgnUIntMult		PROTO ((
						jr_Long *			result_ptr,
						jr_int				u,
						jr_int				v
					));


#endif

extern jr_int jr_LongCmp		PROTO ((const jr_Long	long1, const jr_Long	long2));
extern jr_int jr_LongPtrCmp		PROTO ((const void *	long1, const void *	long2));

#define					jr_ULONG_BASE32_STRING_LENGTH	30
						/*
						** 4/15/08: every 5 bits is 1 char, 64 bits yields 13 chars
						*/



extern jr_int			jr_ULongPtrCmp				PROTO ((
								const void *		void_arg_1,
								const void *		void_arg_2
						));

extern jr_int			jr_ULongPtrHash				PROTO ((
								const void *		void_arg_1
						));

extern char *			jr_ULongToBase32String		PROTO ((
							jr_ULong				ulong_value,
							char *					number_string,
							jr_int					use_lower_case
						));

extern jr_int			jr_ULongInitFromBase32String		PROTO ((
							jr_ULong *				ulong_ptr,
							const char *			number_string,
							char *					error_buf
						));


#define					jr_ULongMaskedLSW(ulong, mask)											\
						(jr_ULongLSW (ulong) & (mask))

#define					jr_ULongMaskedMSW(ulong, mask)											\
						(jr_ULongMSW (ulong) & (mask))

#define					jr_ULongSetMaskedLSW(ulongp, mask, v)									\
						jr_ULongSetLSW (														\
							ulongp, ((jr_ULongLSW (*(ulongp)) & ~(mask))  |  ((v) & (mask)))	\
						)

#define					jr_ULongSetMaskedMSW(ulongp, mask, v)									\
						jr_ULongSetMSW (														\
							ulongp, ((jr_ULongMSW (*(ulongp)) & ~(mask))  |  ((v) & (mask)))	\
						)


#endif
