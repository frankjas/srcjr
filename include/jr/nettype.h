#ifndef _net_inth
#define _net_inth

#include "ezport.h"

#include <sys/types.h>


#include "jr/long.h"
#include "jr/misc.h"


extern unsigned jr_int			jr_bswap_32				(unsigned jr_int value);
extern unsigned jr_int			jr_bswap_16				(unsigned jr_short value);

extern unsigned jr_int			jr_inet_host_number		(unsigned jr_int ip_number);
extern unsigned jr_int			jr_inet_network_number	(unsigned jr_int ip_number);

#ifdef replace_byte_order_funcs

#	ifdef ostype_winnt
#		define LITTLE_ENDIAN	1234
#		define BIG_ENDIAN		4321

#		if defined(_M_IX86) || defined(_M_IX64) || defined(_M_X86) || defined(_M_X64)
#			define BYTE_ORDER	LITTLE_ENDIAN
#		else
#			error "No endian defined"
#		endif
#	else
#		include <endian.h>
#	endif

#	ifndef BYTE_ORDER
#		error "No BYTE_ORDER defined (big, little endian)"
#	endif

#	if BYTE_ORDER == BIG_ENDIAN
#		define jr_ntohl(x)	(x)
#		define jr_ntohs(x)	(x)
#		define jr_htonl(x)	(x)
#		define jr_htons(x)	(x)
#	endif

#	if BYTE_ORDER == LITTLE_ENDIAN
#		define jr_ntohl(x)	jr_bswap_32 (x)
#		define jr_ntohs(x)	jr_bswap_16 (x)
#		define jr_htonl(x)	jr_bswap_32 (x)
#		define jr_htons(x)	jr_bswap_16 (x)
#	endif

#else

#	ifndef ostype_winnt
#		include <netinet/in.h>
#	endif
#	define jr_ntohl(x)	ntohl (x)
#	define jr_ntohs(x)	ntohs (x)
#	define jr_htonl(x)	htonl (x)
#	define jr_htons(x)	htons (x)
#endif



/*
 * jr_NetChar should be 1 byte
 * jr_NetInt should be 4 bytes
 * jr_NetLong should be 8 bytes
 */


typedef char jr_NetChar;

#define jr_NetCharToHost(c)				((c))
#define jr_NetCharAsgnInt(c_ptr, v)		(*(c_ptr) = (v))


typedef unsigned char jr_NetUChar;

#define jr_NetUCharToHost(c)			((c))
#define jr_NetUCharAsgnUInt(c_ptr, v)	(*(c_ptr) = (v))

#define jr_NetUCharMaskedAsgn(c_ptr, mask, v)  \
		jr_NetUCharAsgnUInt ( \
			c_ptr, ((jr_NetUCharToHost (*(c_ptr)) & ~(mask))  |  ((v) & (mask))) \
		)

#define jr_NetUCharMaskedValue(c,  mask) \
		(jr_NetUCharToHost (c) & (mask))



typedef struct {
	jr_short net_short_value;
} jr_NetShort;

#define jr_NetShortAsgnShort(short_number, host_value) \
		((short_number)->net_short_value	= (short) jr_htons ((unsigned short) (host_value)))

#define jr_NetShortToHost(short_number)	\
		((short)	jr_ntohs((short_number).net_short_value))



typedef struct {
	unsigned jr_short	 net_u_short_value;
} jr_NetUShort;

#define jr_NetUShortAsgnUShort(short_number, host_value) \
		((short_number)->net_u_short_value	= jr_htons ((unsigned short) (host_value)))

#define jr_NetUShortToHost(short_number)	\
		((unsigned short)	jr_ntohs((short_number).net_u_short_value))

#define jr_NetUShortMaskedAsgn(l, mask, v)  \
		jr_NetUShortAsgnUShort ( \
			l, ((jr_NetUShortToHost (*(l)) & ~(mask))  |  ((v) & (mask))) \
		)

#define jr_NetUShortMaskedValue(l,  mask) \
		(jr_NetUShortToHost (l) & (mask))



typedef struct {
	jr_int net_int_value;
} jr_NetInt;

#define jr_NetIntAsgnInt(int_number, host_value) \
		((int_number)->net_int_value		= jr_htonl ((unsigned jr_int) (host_value)))

#define jr_NetIntToHost(int_number)		\
		((jr_int)		jr_ntohl((int_number).net_int_value))


typedef struct {
	unsigned jr_int net_u_int_value;
} jr_NetUInt;

#define jr_NetUIntAsgnUInt(int_number, host_value) \
		((int_number)->net_u_int_value		= jr_htonl ((unsigned jr_int) (host_value)))

#define jr_NetUIntToHost(int_number)		\
		((unsigned jr_int)	jr_ntohl((int_number).net_u_int_value))

#define jr_NetUIntMaskedValue(l,  mask) \
		(jr_NetUIntToHost (l) & (mask))

#define jr_NetUIntMaskedAsgn(l, mask, v)  \
		jr_NetUIntAsgnUInt ( \
			l, ((jr_NetUIntToHost (*(l)) & ~(mask))  |  ((v) & (mask))) \
		)



/******** jr_NetLong ********/

typedef struct {
	jr_int			net_msw;
	jr_int			net_lsw;
} jr_NetLong;


#define jr_NetLongSetMSW(longv, v)	((longv)->net_msw = jr_htonl ((jr_int) (v)))
#define jr_NetLongSetLSW(longv, v)	((longv)->net_lsw = jr_htonl ((jr_int) (v)))

#define jr_NetLongMSW(longv)			((jr_int) jr_ntohl ((longv).net_msw))
#define jr_NetLongLSW(longv)			((jr_int) jr_ntohl ((longv).net_lsw))


#define jr_NetLongAsgnLong(long_number, host_value)							\
		(																	\
			jr_NetLongSetMSW (long_number, jr_LongMSW (host_value)),		\
			jr_NetLongSetLSW (long_number, jr_LongLSW (host_value))			\
		)	

#define jr_NetLongAsgnInt(long_number, host_value)							\
		(																	\
			jr_NetLongSetMSW (long_number, 0),								\
			jr_NetLongSetLSW (long_number, host_value)						\
		)	

extern jr_int jr_NetLongPtrCmp	PROTO ((const jr_NetLong *netlong1,const jr_NetLong *netlong2));


/******** jr_NetULong ********/
typedef struct {
	unsigned jr_int	net_u_msw;
	unsigned jr_int	net_u_lsw;
} jr_NetULong;

#define jr_NetULongSetMSW(longv, v)			((longv)->net_u_msw = jr_htonl ((unsigned jr_int) (v)))
#define jr_NetULongSetLSW(longv, v)			((longv)->net_u_lsw = jr_htonl ((unsigned jr_int) (v)))

#define jr_NetULongMSW(longv)				((unsigned jr_int) jr_ntohl ((longv).net_u_msw))
#define jr_NetULongLSW(longv)				((unsigned jr_int) jr_ntohl ((longv).net_u_lsw))

#define jr_NetULongIsZero(longv)			\
		((longv).net_u_msw == 0  && (longv).net_u_lsw == 0)

#define jr_NetULongIsNonZero(longv)			(! jr_NetULongIsZero (longv))

#define jr_NetULongIsMaxValue(longv)				\
		((longv).net_u_msw == jr_UINT_MAX  && (longv).net_u_lsw == jr_UINT_MAX)

#define jr_NetULongAsgnMaxValue(long_number)		\
		(																	\
			jr_NetULongSetMSW (long_number, ULONG_MAX),						\
			jr_NetULongSetLSW (long_number, ULONG_MAX)						\
		)


#define jr_NetULongAsgnULong(long_number, host_value)						\
		(																	\
			jr_NetULongSetMSW (long_number, jr_ULongMSW (host_value)),		\
			jr_NetULongSetLSW (long_number, jr_ULongLSW (host_value))		\
		)

#define jr_NetULongAsgnUInt(long_number, host_value)						\
		(																	\
			jr_NetULongSetMSW (long_number, 0),								\
			jr_NetULongSetLSW (long_number, host_value)						\
		)	

#define jr_NetULongAsgn(long1, long2)										\
		(																	\
			(long1)->net_u_msw = (long2).net_u_msw,							\
			(long1)->net_u_lsw = (long2).net_u_lsw							\
		)


extern jr_int jr_NetIntPtrCmp	PROTO ((const jr_NetInt *netlong1,		const jr_NetInt *netlong2));
extern jr_int jr_NetUIntPtrCmp	PROTO ((const jr_NetUInt *netlong1,		const jr_NetUInt *netlong2));
extern jr_int jr_NetIntPtrCmpRev	PROTO ((const jr_NetInt *netlong1,	const jr_NetInt *netlong2));

extern jr_int jr_NetULongPtrCmp	PROTO ((const jr_NetULong *netlong1,const jr_NetULong *netlong2));



/******* Floating Point ********/

typedef struct {
	double		net_double_value;
} jr_NetDouble;

typedef struct {
	float		net_float_value;
} jr_NetFloat;


extern float		jr_NetFloatToHost		PROTO ((
						jr_NetFloat			net_float
					));

extern void			jr_NetFloatAsgnFloat	PROTO ((
						jr_NetFloat *		net_float,
						double				host_value
					));

extern double		jr_NetDoubleToHost		PROTO ((
						jr_NetDouble		net_double
					));

extern void			jr_NetDoubleAsgnDouble	PROTO ((
						jr_NetDouble *		net_double,
						double				host_value
					));


/*
 * Takes an array of NetUChar and extracts the given bit range
 */

extern void			jr_NetBitAsgn					PROTO ((
						jr_NetUChar *				bit_array,
						unsigned jr_int				bit_value,
						jr_int						bit_position,
						jr_int						num_bits
					));

extern unsigned jr_int jr_NetBitToHost				PROTO ((
						jr_NetUChar *				bit_array,
						jr_int						bit_position,
						jr_int						num_bits
					));

#endif
