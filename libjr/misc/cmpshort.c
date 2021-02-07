#include "ezport.h"

#include "jr/misc.h"

jr_int shortcmp(n,m)
	short n, m ;
{
	return((n)-(m)) ;
}

jr_int shortpcmp(np,mp)
	short *np, *mp ;
{
	return((*np)-(*mp)) ;
}

jr_int rshortcmp(n,m)
	short n, m ;
{
	return((m)-(n)) ;
}

jr_int rshortpcmp(np,mp)
	short *np, *mp ;
{
	return((*mp)-(*np)) ;
}

jr_int jr_ushort_add_max (
	unsigned jr_short *			ushort_ref,
	jr_int						addend)
{
	if (addend >= 0) {
		if (addend >= jr_USHRT_MAX) {
			addend	= jr_USHRT_MAX;
		}
		if (*ushort_ref + addend <= jr_USHRT_MAX) {
			*ushort_ref	= (unsigned jr_short) (*ushort_ref + addend);
		}
		else {
			*ushort_ref	= jr_USHRT_MAX;
		}
	}
	else {
		if (*ushort_ref + addend < 0) {
			*ushort_ref	= 0;
		}
		else {
			*ushort_ref	= (unsigned jr_short) (*ushort_ref + addend);
		}
	}
	return *ushort_ref;
}
