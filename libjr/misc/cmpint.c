#include "ezport.h"

#include "jr/misc.h"

jr_int jr_intcmp(n,m)
	jr_int n, m ;
{
	if (n < 0  &&  m > 0) {
		return -1;
	}
	if (n > 0  &&  m < 0) {
		return 1;
	}
	return((n)-(m)) ;
}

jr_int jr_intpcmp (void_arg_1, void_arg_2)
	const void *	void_arg_1;
	const void *	void_arg_2;
{
	const jr_int *	np			= void_arg_1;
	const jr_int *	mp			= void_arg_2;

	if (*np < 0  &&  *mp > 0) {
		return -1;
	}
	if (*np > 0  &&  *mp < 0) {
		return 1;
	}
	return((*np)-(*mp)) ;
}

jr_int jr_rintcmp(n,m)
	jr_int n, m ;
{
	if (n < 0  &&  m > 0) {
		return 1;
	}
	if (n > 0  &&  m < 0) {
		return -1;
	}
	return((m)-(n)) ;
}

jr_int jr_rintpcmp(np,mp)
	const jr_int *np ;
	const jr_int *mp ;
{
	if (*np < 0  &&  *mp > 0) {
		return 1;
	}
	if (*np > 0  &&  *mp < 0) {
		return -1;
	}
	return((*mp)-(*np)) ;
}

jr_int jr_posintpcmp(np,mp)
	const jr_int *np ;
	const jr_int *mp ;
{
	if ((! *np) && (! *mp)) return(0) ;
	if (! *np) return(1) ;
	if (! *mp) return(-1) ;
	return((*np)-(*mp)) ;
}

jr_int jr_posintcmp(n,m)
	jr_int n,m ;
{
	if ((! n) && (! m)) return(0) ;
	if (! n) return(1) ;
	if (! m) return(-1) ;
	return(n-m) ;
}
