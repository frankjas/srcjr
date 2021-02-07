#include "ezport.h"

#include "jr/misc.h"

void jr_memswitch (vp1, vp2, nbytes)
	void *		vp1;
	void *		vp2;
	jr_int		nbytes;
{
	char *		p1		= vp1;
	char *		p2		= vp2;
	jr_int		i;
	jr_int		c;

	for (i=0; i < nbytes;  i++, p1++, p2++) {
		c	= *p1;
		*p1	= *p2;
		*p2 = c;
	}
}

jr_int jr_memisset (vp1, value, nbytes)
	void *		vp1;
	jr_int		value;
	jr_int		nbytes;
{
	char *		p1		= vp1;
	jr_int		i;

	for (i=0; i < nbytes;  i++, p1++) {
		if (*p1 != value) {
			return 0;
		}
	}

	return 1;
}
