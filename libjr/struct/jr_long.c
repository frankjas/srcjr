#include "ezport.h"
#include "jr/misc.h"
#include "jr/long.h"

#ifdef jr_IS_64_BIT

jr_int jr_LongCmp (long1, long2)
	jr_Long			long1;
	jr_Long			long2;
{
	if (long1 > long2) {
		return (1);
	}
	if (long1 < long2) {
		return (-1);
	}
	return (0);
}

jr_int jr_LongCmpInt (long1, int_value)
	jr_Long			long1;
	jr_int			int_value;
{
	if (long1 > int_value) {
		return (1);
	}
	if (long1 < int_value) {
		return (-1);
	}
	return (0);
}

jr_int jr_LongPtrCmp (void_arg_1, void_arg_2)
	const void *			void_arg_1;
	const void *			void_arg_2;
{
	const jr_Long *		long1			= void_arg_1;
	const jr_Long *		long2			= void_arg_2;

	if (*long1 > *long2) {
		return 1;
	}
	if (*long1 < *long2) {
		return -1;
	}
	return 0;
}

jr_int jr_LongPtrHash (void_arg_1)
	const void *			void_arg_1;
{
	const jr_Long *		long_ptr			= void_arg_1;
	jr_int					hash_value			= 0;

	hash_value	+= (*long_ptr & ((uint64_t) 0xffffffff << 32)) >> 32;
	hash_value	+= *long_ptr & 0xffffffff;

	return hash_value;
}

void jr_LongSetMSW(
	jr_Long *		longp,
	jr_int			value)
{
	jr_int			abs_lsw			= *longp % jr_INT_MIN;

	if (abs_lsw < 0) {
		abs_lsw = -abs_lsw;
	}
	if (value < 0) {
		*longp		= ((jr_Long) value << 31) - abs_lsw;
	}
	else {
		*longp		= ((jr_Long) value << 31) + abs_lsw;
	}
}

void jr_LongSetLSW(
	jr_Long *		longp,
	jr_int			value)
{
	jr_int			abs_lsw			= *longp % jr_INT_MIN;

	if (abs_lsw < 0) {
		abs_lsw		= -abs_lsw;
	}
	if (*longp < 0) {
		*longp		= *longp + abs_lsw - value;
	}
	else {
		*longp		= *longp - abs_lsw + value;
	}
}

#else

jr_int jr_LongCmp (long1, long2)
	const jr_Long long1;
	const jr_Long long2;
{
	if (long1.msw < long2.msw) {
		return -1;
	}

	if (long1.msw > long2.msw) {
		return 1;
	}

	if (long1.lsw < long2.lsw) {
		return -1;
	}

	if (long1.lsw > long2.lsw) {
		return 1;
	}


	return 0;
}


jr_int jr_LongPtrCmp (void_arg_1, void_arg_2)
	const void *			void_arg_1;
	const void *			void_arg_2;
{
	const jr_Long *	long1 = void_arg_1;
	const jr_Long *	long2 = void_arg_2;

	if (long1->msw < long2->msw) {
		return -1;
	}

	if (long1->msw > long2->msw) {
		return 1;
	}

	if (long1->lsw < long2->lsw) {
		return -1;
	}

	if (long1->lsw > long2->lsw) {
		return 1;
	}

	return 0;
}


void jr_LongAddInt (result, long1, int_value)
	jr_Long *			result;
	jr_Long				long1;
	jr_int				int_value;
{
	if (int_value >= 0) {
		/* addition */
		if (jr_LongIsPositive (long1)) {
			if (int_value > jr_INT_MAX - jr_LongLSW (long1)) {
				jr_LongSetMSW (result, jr_LongMSW (long1) + 1);
				jr_LongSetLSW (result, int_value - (jr_INT_MAX - jr_LongLSW (long1)) - 1);
			}
			else {
				jr_LongSetMSW (result, jr_LongMSW (long1));
				jr_LongSetLSW (result, jr_LongLSW (long1) + int_value);
			}
		}
		else if (jr_LongLSW (long1) < 0) {
			/* MSW == 0 */
			jr_LongSetMSW (result, 0);
			jr_LongSetLSW (result, jr_LongLSW (long1) + int_value);
		}
		else {
			/* MSW < 0, LSW > 0 */
			if ((int_value) > jr_LongLSW (long1)) {
				jr_LongSetMSW (result, jr_LongMSW (long1) + 1);

				jr_LongSetLSW (result, jr_INT_MAX - int_value + jr_LongLSW (long1) + 1);
				if (jr_LongMSW (*result) == 0) {
					jr_LongSetLSW (result, -jr_LongLSW (*result));
				}
			}
			else {
				jr_LongSetMSW (result, jr_LongMSW (long1));
				jr_LongSetLSW (result, jr_LongLSW (long1) - int_value);
			}
		}
	}
	else {
		/* subtraction */
		int_value = -int_value;

		if (jr_LongLSW (long1) >= 0) {

			if (jr_LongMSW (long1) == 0) {
				jr_LongSetMSW (result, jr_LongMSW (long1));
				jr_LongSetLSW (result, jr_LongLSW (long1) - int_value);
			}
			else if (jr_LongMSW (long1) < 0) {
				if (jr_INT_MAX - jr_LongLSW (long1) > int_value) {
					jr_LongSetMSW (result, jr_LongMSW (long1));
					jr_LongSetLSW (result, jr_LongLSW (long1) + int_value);
				}
				else {
					jr_LongSetMSW (result, jr_LongMSW (long1) - 1);
					jr_LongSetLSW (result, jr_INT_MAX - int_value - jr_LongLSW (long1) + 1);
				}
			}
			else {
				if (jr_LongLSW (long1) >= int_value) {
					jr_LongSetMSW (result, jr_LongMSW (long1));
					jr_LongSetLSW (result, jr_LongLSW (long1) - int_value);
				}
				else {
					jr_LongSetMSW (result, jr_LongMSW (long1) - 1);
					jr_LongSetLSW (result, jr_INT_MAX - int_value + jr_LongLSW (long1) + 1);
				}
			}
		}
		else {
			/* MSW == 0, LSW < 0 */
			if (jr_INT_MAX + jr_LongLSW (long1) > int_value) {
				jr_LongSetMSW (result, 0);
				jr_LongSetLSW (result, jr_LongLSW (long1) - int_value);
			}
			else {
				jr_LongSetMSW (result, -1);
				jr_LongSetLSW (result, int_value - (jr_INT_MAX + jr_LongLSW (long1)) - 1);
			}
		}
	}
}


void jr_LongAdd (result, long1, long2)
	jr_Long *			result;
	jr_Long				long1;
	jr_Long				long2;
{

	if (jr_LongIsPositive (long2)) {
		jr_LongAddInt (result, long1, jr_LongLSW (long2));

		if (jr_LongIsNegative( *result)) {
			if (jr_LongLSW (*result) < 0 && jr_LongMSW (long2) > 0) {
				/* carry jr_INT_MAX to LSW */
				jr_LongSetMSW (result, jr_LongMSW (long2) - 1);
				jr_LongSetLSW (result, jr_INT_MAX + jr_LongLSW (*result) + 1);
			}
			else {
				/* result msw < 0, lsw >= 0 */
				if (jr_LongMSW(long2) > 0  &&  jr_LongMSW(*result) + jr_LongMSW(long2) == 0) {
					jr_LongSetLSW( result, -jr_LongLSW(*result));
				}
				jr_LongSetMSW (result, jr_LongMSW (*result) + jr_LongMSW (long2));
			}
		}
		else {
			jr_LongSetMSW (result, jr_LongMSW (*result) + jr_LongMSW (long2));
		}
	}
	else {
		if (jr_LongMSW (long2) == 0) {
			/* long2: MSW == 0, LSW < 0 */
			jr_LongAddInt (result, long1, jr_LongLSW (long2));
		}
		else {
			/* long2: MSW < 0, LSW >= 0 */
			jr_LongAddInt (result, long1, -jr_LongLSW (long2));
			
			if (jr_LongLSW (*result) < 0) {
				jr_LongSetMSW (result, jr_LongMSW (long2) - 1);
				jr_LongSetLSW (result, jr_INT_MAX + jr_LongLSW (*result) + 1);
			}
			else {
				if (jr_LongMSW (*result) < 0) {
					jr_LongSetMSW (result, jr_LongMSW (*result) + jr_LongMSW (long2));
				}
				else {
					/* result: MSW >= 0 LSW >= 0 */
					/* long2:  MSW < 0           */
					jr_int	msw_diff;

					msw_diff = jr_LongMSW (*result) + jr_LongMSW (long2);

					if (msw_diff < 0  &&  jr_LongLSW (*result) > 0) {
						jr_LongSetMSW (result, msw_diff + 1);
						jr_LongSetLSW (result, - (jr_INT_MAX - jr_LongLSW (*result) + 1));
					}
					else {
						jr_LongSetMSW (result, jr_LongMSW (*result) + jr_LongMSW (long2));
					}
				}
			}
		}
	}
}


void jr_LongSubtract (result, long1, long2)
	jr_Long *			result;
	jr_Long				long1;
	jr_Long				long2;
{
	jr_Long				tmp_long;

	if (jr_LongMSW (long2) == 0) {
		jr_LongSetMSW (&tmp_long, 0);
		jr_LongSetLSW (&tmp_long, -jr_LongLSW (long2));
	}
	else {
		jr_LongSetMSW (&tmp_long, -jr_LongMSW (long2));
		jr_LongSetLSW (&tmp_long, jr_LongLSW (long2));
	}

	jr_LongAdd (result, long1, tmp_long);
}

jr_int jr_LongModInt (long1, mod_value)
	jr_Long				long1;
	jr_int				mod_value;
{
	jr_ULong			ulong1;
	jr_int				result;


	if (jr_LongIsPositive (long1)) {
		jr_ULongAsgnLong (&ulong1, long1);
	}
	else {
		jr_Long			tmp_long;

		jr_LongAsgnAbsValue (&tmp_long, long1);
		jr_ULongAsgnLong (&ulong1, tmp_long);
	}

	if (mod_value < 0) {
		result			= jr_ULongModUInt (ulong1, -mod_value);
	}
	else {
		result			= jr_ULongModUInt (ulong1, -mod_value);
	}

	if (jr_LongIsPositive (long1)) {
		return result;
	}
	return result;
}


void jr_LongAsgnAbsValue (result_ptr, long1)
	jr_Long *			result_ptr;
	jr_Long				long1;
{
	if (jr_LongIsNegative (long1)) {
		if (jr_LongIsInt (long1)) {
			jr_LongSetMSW (result_ptr, 0);
			jr_LongSetLSW (result_ptr, - jr_LongLSW (long1));
		}
		else {
			jr_LongSetMSW (result_ptr, - jr_LongMSW (long1));
			jr_LongSetLSW (result_ptr,   jr_LongLSW (long1));
		}
	}
	else {
		jr_LongAsgn (result_ptr, long1);
	}
#define jr_LongAsgnAbsValue(long1, long2)									\
		(jr_LongIsNegative(long2)											\
			? ((long1)->msw = -(long2).msw,  (long1)->lsw = -(long2).lsw)	\
			: ((long1)->msw =  (long2).msw,  (long1)->lsw =  (long2).lsw)	\
		)

}

void jr_LongDivideInt (result_ptr, long1, div_value)
	jr_Long *			result_ptr;
	jr_Long				long1;
	jr_int				div_value;
{
	jr_coredump ();
}
	
	
void jr_LongMultInt (result_ptr, long_val, mult_value)
	jr_Long *			result_ptr;
	jr_Long				long_val;
	jr_int				mult_value;
{
	jr_coredump ();
}


void jr_LongAsgnIntMult (result_ptr, u, v)
	jr_Long *			result_ptr;
	jr_int				u;
	jr_int				v;
{
}

#endif


char *jr_LongToBase32String (long_value, number_string, use_lower_case)
	jr_Long					long_value;
	char *					number_string;
	jr_int					use_lower_case;
{
	jr_ULong				ulong_value;
	jr_int					is_negative;

	
	if (jr_LongIsNegative (long_value)) {
		number_string[0]	= '-';
		is_negative			= 1;

		jr_LongAsgnAbsValue (&long_value, long_value);
	}
	else {
		is_negative			= 0;
	}

	jr_ULongAsgnLong (&ulong_value, long_value);

	return jr_ULongToBase32String (ulong_value, number_string + is_negative, use_lower_case);
}

/*
jr_int jr_LongInitFromBase32String (long_ptr, number_string, error_buf)
	jr_Long *				long_ptr;
	char *					number_string;
	char *					error_buf;
{
	jr_ULong				ulong_value;
	jr_int					is_negative;

	number_string	= jr_FirstNonWhiteSpacePtr (number_string);

	if (number_string[0]  ==  '-') {
		is_negative			= 1;
	}
	else {
		is_negative			= 0;
	}
	
	status	= jr_ULongInitFromBase32String (ulong_value, number_string, error_buf)

	if (status != 0) {
		return status;
	}

	jr_LongAsgnULong (long_ptr, ulong_value);

	if (is_negative) {
		jr_LongMult
	}

	return 0;
}

*/
