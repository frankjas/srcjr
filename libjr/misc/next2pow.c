#include "ezport.h"

unsigned jr_int jr_next2power (number)
	unsigned jr_int number;
{
	unsigned jr_int two_to_n = 1;

	for (; two_to_n < number; two_to_n <<= 1);
	return (two_to_n);
}

jr_int jr_is2power (number)
	unsigned jr_int number;
{
	unsigned jr_int two_to_n = 1;

	for (; two_to_n < number; two_to_n <<= 1);

	if (two_to_n == number)		return (1);
	else						return (0);
}

unsigned jr_int jr_get2power (power)
	jr_int power;
{
	unsigned jr_int two_to_n;
	jr_int i;

	two_to_n = 1;

	for (i=0; i < power; i++) {
		two_to_n <<= 1;
	}

	return (two_to_n);
}

unsigned jr_int jr_log_of_2_power (number)
	jr_int		number;
{
	jr_int		log_value;

	log_value	= 0;

	for (;  number > 1; number >>= 1) {
		log_value ++;
	}
	return		log_value;
}
