/*	$NetBSD: xdr_float.c,v 1.9 1995/06/05 11:48:26 pk Exp $	*/

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */


#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)xdr_float.c 1.12 87/08/11 Copyr 1984 Sun Micro";*/
/*static char *sccsid = "from: @(#)xdr_float.c	2.1 88/07/29 4.0 RPCSRC";*/
static char *rcsid = "$NetBSD: xdr_float.c,v 1.9 1995/06/05 11:48:26 pk Exp $";
#endif

/*
 * xdr_float.c, Generic XDR routines impelmentation.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * These are the "floating point" xdr routines used to (de)serialize
 * most common data items.  See xdr.h for more info on the interface to
 * xdr.
 */

/*
 * NB: Not portable.
 * This routine works on machines with IEEE754 FP and Vaxen.
 */

/*
** 1/25/07 Copied from rpc/types.h and rpc/xdr.h
** Modified to use JR types for portability
**
** Only supports IEEE754, eliminated VAX support for clarity
*/


#include "ezport.h"

#ifdef missing_xdr_float

#include <stdio.h>

#include "jr/xdr.h"
#include "jr/misc.h"
#include "jr/nettype.h"

#ifndef BYTE_ORDER
#	error "No BYTE_ORDER defined (big, little endian)"
#endif

#define int32_t		jr_int
#define bool_t		jr_int
#define TRUE		((jr_int) 1)
#define FALSE		((jr_int) 0)


bool_t
xdr_float(xdrs, fp)
	register XDR *xdrs;
	register float *fp;
{
	bool_t rv;
	long tmpl;
	switch (xdrs->x_op) {

	case XDR_ENCODE:
		tmpl = *(int32_t *)fp;
		return (XDR_PUTLONG(xdrs, &tmpl));

	case XDR_DECODE:
		rv = XDR_GETLONG(xdrs, &tmpl);
		*(int32_t *)fp = tmpl;
		return (rv);

	case XDR_FREE:
		return (TRUE);
	}
	return (FALSE);
}



bool_t
xdr_double(xdrs, dp)
	register XDR *xdrs;
	double *dp;
{
	register int32_t *i32p;
	bool_t rv;
	long tmpl;

	switch (xdrs->x_op) {

	case XDR_ENCODE:
		i32p = (int32_t *)dp;
#if BYTE_ORDER == BIG_ENDIAN
		tmpl = *i32p++;
		rv = XDR_PUTLONG(xdrs, &tmpl);
		if (!rv)
			return (rv);
		tmpl = *i32p;
		rv = XDR_PUTLONG(xdrs, &tmpl);
#else
		tmpl = *(i32p+1);
		rv = XDR_PUTLONG(xdrs, &tmpl);
		if (!rv)
			return (rv);
		tmpl = *i32p;
		rv = XDR_PUTLONG(xdrs, &tmpl);
#endif
		return (rv);

	case XDR_DECODE:
		i32p = (int32_t *)dp;
#if BYTE_ORDER == BIG_ENDIAN
		rv = XDR_GETLONG(xdrs, &tmpl);
		*i32p++ = tmpl;
		if (!rv)
			return (rv);
		rv = XDR_GETLONG(xdrs, &tmpl);
		*i32p = tmpl;
#else
		rv = XDR_GETLONG(xdrs, &tmpl);
		*(i32p+1) = tmpl;
		if (!rv)
			return (rv);
		rv = XDR_GETLONG(xdrs, &tmpl);
		*i32p = tmpl;
#endif
		return (rv);

	case XDR_FREE:
		return (TRUE);
	}
	return (FALSE);
}

#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
 
#endif
