#include "ezport.h"

#include "jr/nettype.h"

unsigned jr_int jr_bswap_32 (unsigned jr_int value)
{
	/*
	** 1/27/07: Little endian:	0123
	**			Big endian:		3210
	*/

	value	= (0xffff0000 & value) >> 16 | (0x0000ffff & value) << 16;
	value	= (0xff00ff00 & value) >> 8  | (0x00ff00ff & value) << 8;
	
	return value;
}

unsigned jr_int jr_bswap_16 (unsigned jr_short value)
{
	/*
	** 1/27/07: Little endian:	01
	**			Big endian:		10
	*/
	value	= (0xff00 & value) >> 8 | (0x00ff & value) << 8;
	
	
	return value;
}

#define jr_CLASS_A_MASK				0xff000000
#define jr_CLASS_B_MASK				0xffff0000
#define jr_CLASS_C_MASK				0xffffff00

unsigned jr_int jr_inet_host_number( unsigned jr_int ip_number)
{
	ip_number	= jr_ntohl( ip_number);

	if ((ip_number & 0x80000000) == 0) {
		return ip_number & ~jr_CLASS_A_MASK;
	}
	if ((ip_number & 0xc0000000) == 0x80000000) {
		return ip_number & ~jr_CLASS_B_MASK;
	}
	return ip_number & ~jr_CLASS_C_MASK;
}

unsigned jr_int jr_inet_network_number( unsigned jr_int ip_number)
{
	ip_number	= jr_ntohl( ip_number);

	if ((ip_number & 0x80000000) == 0) {
		return (ip_number & jr_CLASS_A_MASK) >> 24;
	}
	if ((ip_number & 0xc0000000) == 0x80000000) {
		return (ip_number & jr_CLASS_B_MASK) >> 16;
	}
	return (ip_number & jr_CLASS_C_MASK) >> 8;
}
