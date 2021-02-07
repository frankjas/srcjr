#include "ezport.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>

#include "jr/long.h"
#include "jr/error.h"



#ifdef jr_IS_64_BIT

jr_int jr_ULongCmp (long1, long2)
	jr_ULong			long1;
	jr_ULong			long2;
{
	if (long1 > long2) {
		return (1);
	}
	if (long1 < long2) {
		return (-1);
	}
	return (0);
}

jr_int jr_ULongCmpUInt (long1, uint_value)
	jr_ULong			long1;
	unsigned jr_int		uint_value;
{
	if (long1 > uint_value) {
		return (1);
	}
	if (long1 < uint_value) {
		return (-1);
	}
	return (0);
}

jr_int jr_ULongPtrCmp (void_arg_1, void_arg_2)
	const void *			void_arg_1;
	const void *			void_arg_2;
{
	const jr_ULong *		long1			= void_arg_1;
	const jr_ULong *		long2			= void_arg_2;

	if (*long1 > *long2) {
		return 1;
	}
	if (*long1 < *long2) {
		return -1;
	}
	return 0;
}

jr_int jr_ULongPtrHash (void_arg_1)
	const void *			void_arg_1;
{
	const jr_ULong *		long_ptr			= void_arg_1;
	jr_int					hash_value			= 0;

	hash_value	+= (*long_ptr & ((uint64_t) 0xffffffff << 32)) >> 32;
	hash_value	+= *long_ptr & 0xffffffff;

	return hash_value;
}


#else

void jr_ULongAddUInt (result, long1, uint_value)
	jr_ULong *			result;
	jr_ULong			long1;
	unsigned jr_int		uint_value;
{
	/*
	 * In the following, if the lsw + u > jr_UINT_MAX
	 * then a carry should occur into the msw
	 *
	 * u + lsw  ==  u + lsw + jr_UINT_MAX - jr_UINT_MAX + 1 - 1
	 *
	 *          ==  jr_UINT_MAX + 1  +  u + lsw - jr_UINT_MAX - 1
	 *
	 *          ==  <carry into msw>  +  u - (jr_UINT_MAX - lsw) - 1
	 *
	 * Note the reorganisation of the order of evaluation to avoid
	 * any overflows
	 */

	if ((uint_value) > jr_UINT_MAX - jr_ULongLSW (long1)) {
		jr_ULongSetMSW (result, jr_ULongMSW (long1) + 1);
		jr_ULongSetLSW (result, uint_value - (jr_UINT_MAX - jr_ULongLSW (long1)) - 1);
	}
	else {
		jr_ULongSetMSW (result, jr_ULongMSW (long1));
		jr_ULongSetLSW (result, jr_ULongLSW (long1) + uint_value);
	}
}

void jr_ULongSubtractUInt (result, long1, uint_value)
	jr_ULong *			result;
	jr_ULong			long1;
	unsigned jr_int		uint_value;
{
	if ((uint_value) > jr_ULongLSW (long1)) {
		jr_ULongSetMSW (result, jr_ULongMSW (long1) - 1);
		jr_ULongSetLSW (result, jr_UINT_MAX - uint_value + 1 + jr_ULongLSW (long1));
	}
	else {
		jr_ULongSetMSW (result, jr_ULongMSW (long1));
		jr_ULongSetLSW (result, jr_ULongLSW (long1) - uint_value);
	}
}

void jr_ULongShiftInt (result, long1, shift_value)
	jr_ULong *			result;
	jr_ULong			long1;
	jr_int				shift_value;
{
	jr_int				i;
	jr_int				mask;
	jr_int				right_shift;
	jr_int				left_shift;

	unsigned jr_int		msw;			/* need to be unsigned */
	unsigned jr_int		msw_bits;		/* otherwise shift does sign extension */
	unsigned jr_int		lsw;
	unsigned jr_int		lsw_bits;


	right_shift	= 0;
	left_shift	= 0;

	if (shift_value > 0) {
		right_shift	= 1;
	}
	if (shift_value < 0) {
		shift_value = -shift_value;
		left_shift	= 1;
	}

	if (shift_value >= 64) {
		jr_ULongAsgnUInt (result, 0);
		return;
	}

	if (shift_value >= 32) {
		if (right_shift) {
			jr_ULongSetLSW (result, jr_ULongMSW (long1)  >>  (shift_value - 32));
			jr_ULongSetMSW (result, 0);
		}
		else {
			jr_ULongSetMSW (result, jr_ULongLSW (long1)  <<  (shift_value - 32));
			jr_ULongSetLSW (result, 0);
		}
		return;
	}


	for (mask=1, i=0; i < shift_value; i++) {
		mask *= 2;
	}
	mask --;

	if (right_shift) {
		msw_bits	= jr_ULongMSW (long1) & mask;
		lsw			= jr_ULongLSW (long1) >> shift_value;
		lsw			|= msw_bits << (sizeof (long) * CHAR_BIT - shift_value);


		jr_ULongSetMSW (result, jr_ULongMSW (long1) >> shift_value);
		jr_ULongSetLSW (result, lsw);
	}

	if (left_shift) {
		mask		<<= sizeof (long) * CHAR_BIT - shift_value;
		lsw_bits	= jr_ULongLSW (long1) & mask;

		msw			= jr_ULongMSW (long1) << shift_value;
		msw			|= lsw_bits >> (sizeof (long) * CHAR_BIT - shift_value);


		jr_ULongSetMSW (result, msw);
		jr_ULongSetLSW (result, jr_ULongLSW (long1) << shift_value);
	}
}

jr_int jr_ULongModUInt (long1, mod_value)
	jr_ULong				long1;
	unsigned jr_int			mod_value;
{
	/*
	 * a long value ==  2^32*msw + lsw
	 *              ==  (2^32 - 1 + 1)*msw + lsw
	 *              ==  (2^32 - 1)*msw + msw + lsw
	 * so,
	 *
	 *  jr_long % m ==  ((2^32 - 1)*msw + msw + lsw) % m
	 *              ==  [((2^32 - 1)%m) * (msw%m) + msw%m + lsw%m ] % m
	 *              ==  [   rem1     *     rem2  +  rem2  +  rem3 ] % m
	 */
	unsigned jr_int			rem1;
	unsigned jr_int			rem2;
	unsigned jr_int			rem3;
	jr_ULong				tmp_ulong;


	rem1 = jr_UINT_MAX % mod_value;
	rem2 = jr_ULongMSW (long1) % mod_value;
	rem3 = jr_ULongLSW (long1) % mod_value;

	jr_ULongAsgnUInt (&tmp_ulong, rem1 * rem2);
	jr_ULongAddUInt (&tmp_ulong, tmp_ulong, rem2);
	jr_ULongAddUInt (&tmp_ulong, tmp_ulong, rem3);

	assert (jr_ULongIsUInt (tmp_ulong));

	return jr_ULongToHost (tmp_ulong) % mod_value;
}


void jr_ULongDivideUInt (result_ptr, long1, div_value)
	jr_ULong *				result_ptr;
	jr_ULong				long1;
	unsigned jr_int			div_value;
{
	/*
	 * 	jr_long		==	2^32*msw + lsw
	 *
	 * Let:
	 *		q1		==	msw/d
	 *		r1		==	msw%d
	 *		q2		==	lsw/d
	 *		r2		==	lsw%d
	 *
	 * where / stands for integer division
	 *
	 *		msw		==	q1*d + r1
	 *		lsw		==	q2*d + r2
	 *
	 *
	 * 	jr_long		==	2^32*msw + lsw
	 *				==	2^32*(q1*d + r1) + q2*d + r2
	 *				==	2^32*q1*d  +  2^32*r1  +  q2*d  +  r2
	 * 
	 * Note that	(d * k)			/ d		==	k
	 * and that		(d * k + m)		/ d		==  k + m/d
	 *
	 * where / is integer division.
	 *
	 * So jr_long after division by d equals:
	 *
	 *	A)			==	2^32*q1  +  q2  +  (2^32*r1 + r2)/d
	 *
	 * Note:
	 *
	 * 2^32*r1			==	(2^32 - 1)*r1 + r1
	 *
	 * Note further:
	 *
	 * (2^32 - 1)*r1	==	[d*((2^32 - 1)/d)    + ((2^32 - 1)%d)]  *  r1
	 *					==	 d*((2^32 - 1)/d)*r1 + ((2^32 - 1)%d)*r1
	 *					==	 d*q3*r1 + r3*r1
	 *
	 * Where	q3		==	(2^32 - 1)/d
	 * 			r3		==	(2^32 - 1)%d		(which is < d)
	 *
	 * So,
	 *
	 * 2^32*r1			==	(2^32 - 1)*r1    + r1
	 *					==	d*q3*r1 + r3*r1  + r1
	 *
	 * Substituting in into A:
	 *
	 *	B)				==	2^32*q1 + q2 + (d*q3*r1 + r3*r1 + r1 + r2) / d
	 *					==	2^32*q1 + q2 + q3*r1 + (r3*r1 + r1  + r2) / d
	 *	Alternatively,	==	2^32*q1 + q2 + q3*r1 + (r1*(r3 + 1) + r2) / d
	 *
	 * Note that:
	 *			q3*r1	<  (2^32/d)*d  == 2^32
	 * Since
	 *			r1		<	d
	 *			q3		<=	2^32/d
	 *
	 * But also note that  r1*(r3 + 1) + r2  may be greater than  2^32.
	 * Or in other words,  r1*(r3 + 1)       may be greater than  2^32 - r2
	 * Or                     (r3 + 1)       may be greater than (2^32 - r2) / r1
	 */

	unsigned jr_int		q1 = jr_ULongMSW (long1) / div_value;
	unsigned jr_int		r1 = jr_ULongMSW (long1) % div_value;
	unsigned jr_int		q2 = jr_ULongLSW (long1) / div_value;
	unsigned jr_int		r2 = jr_ULongLSW (long1) % div_value;
	unsigned jr_int		q3 = jr_UINT_MAX / div_value;
	unsigned jr_int		r3 = jr_UINT_MAX % div_value;


	jr_ULongSetMSW	(result_ptr, q1);
	jr_ULongSetLSW	(result_ptr, q2);

	jr_ULongAddUInt (result_ptr, *result_ptr, q3 * r1);

	if (r1  &&  r3  &&  (r3 + 1)  >  (jr_UINT_MAX - r2) / r1) {
		jr_ULong		tmp1;

		jr_ULongAsgnUIntMult	(&tmp1, r1, r3 + 1);
		jr_ULongAddUInt			(&tmp1, tmp1, r2);
		jr_ULongDivideUInt		(&tmp1, tmp1, div_value);
		/*
		 * tmp1 < long1, so with each recursive call, msw will decrease,
		 * eventually to 0, which will make r1 0
		 */

		jr_ULongAdd (result_ptr, *result_ptr, tmp1);
	}
	else {
		jr_ULongAddUInt (result_ptr, *result_ptr, (r1*(r3 + 1) + r2) / div_value);
	}
}
	
	
void jr_ULongMultUInt (result_ptr, long_val, mult_value)
	jr_ULong *			result_ptr;
	jr_ULong			long_val;
	unsigned jr_int		mult_value;
{
	/*
	 *	jr_long		==  (2^32*msw + lsw)
	 *  jr_long * x ==  (2^32*msw + lsw) * x
	 *
	 *				==  2^32*msw*x + lsw*x
	 *
	 *	both x*msw  and x*lsw may be greater than 2^32 
	 *
	 */

	jr_ULong	tmp1;


	jr_ULongAsgnUIntMult	(result_ptr,	jr_ULongMSW (long_val), mult_value);
	jr_ULongShiftInt		(result_ptr,	*result_ptr, -32);
	jr_ULongAsgnUIntMult	(&tmp1,			jr_ULongLSW (long_val), mult_value);
	jr_ULongAdd				(result_ptr,	*result_ptr, tmp1);
}


void jr_ULongAsgnUIntMult (result_ptr, u, v)
	jr_ULong *					result_ptr;
	unsigned jr_int				u;
	unsigned jr_int				v;
{
	/*
	 *	The following definitions are used below, where / is integer division:
	 *
	 *			q1	== u / 64k
	 *			q2	== v / 64k
	 *			r1	== u % 64k
	 *			r2	== v % 64k
	 *
	 *	Each of q1, q2, r1, and r2 are < 64k because:
	 *
	 *	2^32		>	u
	 *	2^32/64k	>	u/64k
	 *	64k			>	u/64k
	 *
	 *			u	==	64k * q1  +  r1
	 *			v	==	64k * q2  +  r2
	 *
	 *	u * v		==	(64k*q1 + r1)  *  (64k*q2 + r2)
	 *				==	64k*q1*64k*q2  +  64k*q1*r2  +  r1*64k*q2  + r1*r2
	 *				==	64k*64k*q1*q2  +  64k*q1*r2  +  64k*r1*q2  + r1*r2
	 *				==	  2^32 * val1  +  64k* val2  +  64k* val3  +  val4
	 *
	 *	Since q1 and q2 are < 64k, then val1 < 2^32,
	 *	as are all the other 'val' terms.
	 *
	 */				

	unsigned jr_int				q1			= u / (64 * 1024);
	unsigned jr_int				r1			= u % (64 * 1024);
	unsigned jr_int				q2			= v / (64 * 1024);
	unsigned jr_int				r2			= v % (64 * 1024);

	unsigned jr_int				val1		= q1 * q2;
	unsigned jr_int				val2		= q1 * r2;
	unsigned jr_int				val3		= r1 * q2;
	unsigned jr_int				val4		= r2 * r1;

	jr_ULong					tmp1;


	jr_ULongSetMSW		(result_ptr, val1);
	jr_ULongSetLSW		(result_ptr, 0);

	jr_ULongAsgnUInt	(&tmp1, val2);
	jr_ULongShiftInt	(&tmp1, tmp1, -16);

	jr_ULongAdd			(result_ptr, *result_ptr, tmp1);

	jr_ULongAsgnUInt	(&tmp1, val3);
	jr_ULongShiftInt	(&tmp1, tmp1, -16);

	jr_ULongAdd			(result_ptr, *result_ptr, tmp1);
	jr_ULongAddUInt		(result_ptr, *result_ptr, val4);
}

void jr_ULongDivide (result_ptr, long1, long2)
	jr_ULong *				result_ptr;
	jr_ULong				long1;
	jr_ULong				long2;
{
	/*
	 *	long1		==	2^32*msw1 + lsw1
	 *	long2		==	2^32*msw2 + lsw2
	 *	long1/long2	==	(2^32*msw1 + lsw1) / (2^32*msw2 + lsw2)
	 *				==	2^32*msw1 / (2^32*msw2 + lsw2)  +  lsw1 / (2^32*msw2 + lsw2)
	 *				==
	 */
}


double jr_ULongToFloat (ulong_value)
	jr_ULong				ulong_value;
{
	double					float_value;
	jr_int					i;

	float_value		= jr_ULongMSW (ulong_value);

	if (float_value != 0) {
		for (i = 0;  i < (jr_int) sizeof (long) * CHAR_BIT;  i++) {
			float_value	*= 2;
		}
	}

	float_value += jr_ULongLSW (ulong_value);

	return float_value;
}

void jr_ULongAsgnFloat (ulong_value_ptr, float_value)
	jr_ULong *				ulong_value_ptr;
	double					float_value;
{
	double					high_order;

	high_order				= (double) float_value / jr_INT_MAX;

	jr_ULongSetMSW (ulong_value_ptr, high_order);
	jr_ULongSetLSW (ulong_value_ptr, float_value - (high_order * jr_INT_MAX));
}



jr_int jr_ULongCmp (long1, long2)
	jr_ULong			long1;
	jr_ULong			long2;
{
	if (long1.u_msw < long2.u_msw) {
		return (-1);
	}
	if (long1.u_msw > long2.u_msw) {
		return (1);
	}

	if (long1.u_lsw < long2.u_lsw) {
		return (-1);
	}
	if (long1.u_lsw > long2.u_lsw) {
		return (1);
	}
	return (0);
}

jr_int jr_ULongCmpUInt (long1, uint_value)
	jr_ULong			long1;
	unsigned jr_int		uint_value;
{
	if (!jr_ULongIsUInt (long1)) {
		return (1);
	}

	if (long1.u_lsw > uint_value) {
		return (1);
	}
	if (long1.u_lsw < uint_value) {
		return (-1);
	}
	return (0);
}

jr_int jr_ULongPtrCmp (void_arg_1, void_arg_2)
	const void *			void_arg_1;
	const void *			void_arg_2;
{
	const jr_ULong *		long1			= void_arg_1;
	const jr_ULong *		long2			= void_arg_2;

	if (long1->u_msw < long2->u_msw) {
		return (-1);
	}
	if (long1->u_msw > long2->u_msw) {
		return (1);
	}

	if (long1->u_lsw < long2->u_lsw) {
		return (-1);
	}
	if (long1->u_lsw > long2->u_lsw) {
		return (1);
	}
	return (0);
}


jr_int jr_ULongPtrHash (void_arg_1)
	const void *			void_arg_1;
{
	const jr_ULong *		long_ptr			= void_arg_1;
	jr_int					hash_value			= 0;

	hash_value	+= jr_ULongMSW (*long_ptr);
	hash_value	+= jr_ULongLSW (*long_ptr);

	return hash_value;
}


#endif


char *jr_ULongToBase32String (ulong_value, number_string, use_lower_case)
	jr_ULong				ulong_value;
	char *					number_string;
	jr_int					use_lower_case;
{
	char					tmp_buf [32];
	jr_int					i						= 0;
	jr_int					remainder;
	jr_int					num_digits;

	
	do {
		remainder	= jr_ULongLSW (ulong_value)  &  0x1f;

		if (remainder < 10) {
			tmp_buf[i++]	= '0' + remainder;
		}
		else if (use_lower_case) {
			tmp_buf[i++]	= 'a' + remainder - 10;
		}
		else {
			tmp_buf[i++]	= 'A' + remainder - 10;
		}

		jr_ULongShiftInt (&ulong_value, ulong_value, 5);
	}
	while (jr_ULongIsNonZero (ulong_value));

	num_digits	= i;

	for (i=0;  i < num_digits;  i++) {
		number_string[i]	= tmp_buf [num_digits - 1 - i];
	}
	number_string [i] = 0;

	return number_string;
}


jr_int jr_ULongInitFromBase32String (ulong_ptr, number_string, error_buf)
	jr_ULong *				ulong_ptr;
	const char *			number_string;
	char *					error_buf;
{
	jr_int					i;
	jr_int					c;
	jr_int					n;
	jr_ULong				curr_ulong;
	jr_ULong				tmp_ulong;
	jr_ULong				max_base;
	

	jr_ULongAsgnMaxValue (&max_base);
	jr_ULongShiftInt (&max_base, max_base, 5);

	jr_ULongAsgnUInt (&curr_ulong, 0);

	for (i=0;  number_string[i];  i++) {
		c = number_string [i];

		if (isdigit (c)) {
			n = c - '0';
		}
		else if (isupper (c)) {
			n = 10 + c - 'A';
		}
		else if (islower (c)) {
			n = 10 + c - 'a';
		}
		else {
			jr_esprintf (error_buf, "bad id digit '%c'", c);
			return -1;
		}

		if (jr_ULongCmp (curr_ulong, max_base)  >  0) {
			jr_esprintf (error_buf, "id overflow");
			return -1;
		}

		jr_ULongShiftInt	(&curr_ulong, curr_ulong, -5);

		jr_ULongAsgnMaxValue (&tmp_ulong);
		jr_ULongSubtractUInt (&tmp_ulong, tmp_ulong, n);

		if (jr_ULongCmp (curr_ulong, tmp_ulong)  >  0) {
			jr_esprintf (error_buf, "id overflow");
			return -1;
		}

		jr_ULongAddUInt		(&curr_ulong, curr_ulong, n);
	}

	jr_ULongAsgn (ulong_ptr, curr_ulong);

	return 0;
}

