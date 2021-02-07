#include "ezport.h"

#include "jr/nettype.h"

jr_int jr_NetIntPtrCmp (netlong1, netlong2)
	const jr_NetInt *netlong1;
	const jr_NetInt *netlong2;
{
	return (jr_NetIntToHost (*netlong1) - jr_NetIntToHost (*netlong2));
}

jr_int jr_NetUIntPtrCmp (netlong1, netlong2)
	const jr_NetUInt *netlong1;
	const jr_NetUInt *netlong2;
{
	return (jr_NetUIntToHost (*netlong1) - jr_NetUIntToHost (*netlong2));
}

jr_int jr_NetIntPtrCmpRev (netlong1, netlong2)
	const jr_NetInt *netlong1;
	const jr_NetInt *netlong2;
{
	return (jr_NetIntToHost (*netlong2) - jr_NetIntToHost (*netlong1));
}


jr_int jr_NetULongPtrCmp (netlong1, netlong2)
	const jr_NetULong *netlong1;
	const jr_NetULong *netlong2;
{
	jr_ULong		long1;
	jr_ULong		long2;

	jr_ULongAsgnNetULong (&long1, *netlong1);
	jr_ULongAsgnNetULong (&long2, *netlong2);

	return (jr_ULongCmp (long1, long2));
}

jr_int jr_NetLongPtrCmpRev (netlong1, netlong2)
	const jr_NetLong *netlong1;
	const jr_NetLong *netlong2;
{
	jr_Long		long1 [1];
	jr_Long		long2 [1];

	jr_LongAsgnNetLong (long1, *netlong1);
	jr_LongAsgnNetLong (long2, *netlong2);

	return (jr_LongPtrCmp (long2, long1));
}

jr_int jr_NetLongPtrCmp (netlong1, netlong2)
	const jr_NetLong *netlong1;
	const jr_NetLong *netlong2;
{
	jr_Long		long1 [1];
	jr_Long		long2 [1];

	jr_LongAsgnNetLong (long1, *netlong1);
	jr_LongAsgnNetLong (long2, *netlong2);

	return (jr_LongPtrCmp (long1, long2));
}


#	define CreateMask(num_bits, shift_amount)	\
	(~ ((~(0xffffffff << (num_bits))) << (shift_amount)))


void jr_NetBitAsgn (bit_array, bit_value, bit_position, num_bits)
	jr_NetUChar *			bit_array;
	unsigned jr_int			bit_value;
	jr_int					bit_position;
	jr_int					num_bits;
{
	jr_int					array_index		= bit_position / 8;
	jr_int					shift_amount;
	jr_int					mask;


	if (num_bits  >= 16) {
		unsigned jr_int *	value_ptr;

		value_ptr		= (void *) ((char *) bit_array + array_index);
		shift_amount	= bit_position % 32;

		mask			= CreateMask (num_bits, shift_amount);
		*value_ptr		&= mask;
		*value_ptr		|= bit_value << shift_amount;
	}
	else if (num_bits  >=  8) {
		unsigned jr_short *	value_ptr;

		value_ptr		= (void *) ((char *) bit_array + array_index);
		shift_amount	= bit_position % 16;

		mask			= CreateMask (num_bits, shift_amount);
		*value_ptr		&= mask;
		*value_ptr		|= bit_value << shift_amount;
	}
	else {
		unsigned char *		value_ptr;

		value_ptr		= (void *) ((char *) bit_array + array_index);
		shift_amount	= bit_position % 8;

		mask			= CreateMask (num_bits, shift_amount);
		*value_ptr		&= mask;
		*value_ptr		|= bit_value << shift_amount;
	}
}

#undef CreateMask

#	define CreateMask(num_bits, shift_amount)	\
	((~(0xffffffff << (num_bits))) << (shift_amount))



unsigned jr_int jr_NetBitToHost (bit_array, bit_position, num_bits)
	jr_NetUChar *			bit_array;
	jr_int					bit_position;
	jr_int					num_bits;
{
	jr_int					array_index		= bit_position / 8;
	jr_int					shift_amount;
	jr_int					value;
	jr_int					mask;

	if (num_bits  >= 16) {
		unsigned jr_int *	value_ptr;

		value_ptr		= (void *) ((char *) bit_array + array_index);
		shift_amount	= bit_position % 32;

		mask			= CreateMask (num_bits, shift_amount);
		value			= (*value_ptr & mask) >> shift_amount;
	}
	else if (num_bits  >=  8) {
		unsigned jr_short *	value_ptr;

		value_ptr		= (void *) ((char *) bit_array + array_index);
		shift_amount	= bit_position % 16;

		mask			= CreateMask (num_bits, shift_amount);
		value			= (*value_ptr & mask) >> shift_amount;
	}
	else {
		unsigned char *		value_ptr;

		value_ptr		= (void *) ((char *) bit_array + array_index);
		shift_amount	= bit_position % 8;

		mask			= CreateMask (num_bits, shift_amount);
		value			= (*value_ptr & mask) >> shift_amount;
	}
	return value;
}


