#ifndef _jr_xdr_h___
#define _jr_xdr_h___

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

/*
 * xdr.h, External Data Representation Serialization Routines.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

/*
** 1/25/07 Copied from rpc/types.h and rpc/xdr.h
** modified to use JR types for portability
** 	bool_t		-> jr_int
**	u_int		-> unsigned jr_int
*/

#ifndef __THROW
	/*
	** 1/25/07: Missing under Microsoft, a define under GNU
	*/
#	define __THROW
#endif


/*
 * Xdr operations.  XDR_ENCODE causes the type to be encoded into the
 * stream.  XDR_DECODE causes the type to be extracted from the stream.
 * XDR_FREE can be used to release the space allocated by an XDR_DECODE
 * request.
 */
enum xdr_op {
  XDR_ENCODE = 0,
  XDR_DECODE = 1,
  XDR_FREE = 2
};

/*
 * The XDR handle.
 * Contains operation which is being applied to the stream,
 * an operations vector for the particular implementation (e.g. see xdr_mem.c),
 * and two private fields for the use of the particular implementation.
 */
typedef struct XDR XDR;

struct XDR {
	enum xdr_op x_op;		/* operation; fast additional param */
	struct xdr_ops {
		jr_int (*x_getlong)				(XDR *__xdrs, long *__lp);
										/* get a long from underlying stream */
		jr_int (*x_putlong)				(XDR *__xdrs, const long *__lp);
										/* put a long to " */
		jr_int (*x_getbytes)			(XDR *__xdrs, caddr_t __addr, unsigned int __len);
										/* get some bytes from " */
		jr_int (*x_putbytes)			(XDR *__xdrs, const char *__addr, unsigned jr_int __len);
										/* put some bytes to " */
		unsigned jr_int (*x_getpostn)	(const XDR *__xdrs);
										/* returns bytes off from beginning */
		jr_int (*x_setpostn)			(XDR *__xdrs, unsigned jr_int __pos);
										/* lets you reposition the stream */
		jr_int *(*x_inline)				(XDR *__xdrs, unsigned jr_int __len);
										/* buf quick ptr to buffered data */
		void (*x_destroy)				(XDR *__xdrs);
										/* free privates of this xdr_stream */
		jr_int (*x_getint32)			(XDR *__xdrs, jr_int *__ip);
										/* get a int from underlying stream */
		jr_int (*x_putint32)			(XDR *__xdrs, const jr_int *__ip);
										/* put a int to " */
	}
	 *x_ops;
	caddr_t				x_public;		/* users' data */
	caddr_t				x_private;		/* pointer to private data */
	caddr_t				x_base;		/* private used for position info */
	unsigned jr_int		x_handy;		/* extra private word */
};

/* XDR using memory buffers */

extern void				xdrmem_create (
							XDR *				__xdrs,
							const caddr_t		__addr,
							unsigned jr_int		__size,
							enum xdr_op			__xop
						) __THROW;

extern jr_int			xdr_float (
							XDR *				__xdrs,
							float *				__fp
						) __THROW;

extern jr_int			xdr_double (
							XDR *				__xdrs,
							double *			__dp
						) __THROW;

#define					xdr_setpos(xdrs, pos)				\
						(*(xdrs)->x_ops->x_setpostn)(xdrs, pos)

#define					XDR_GETLONG(xdrs, longp)			\
						(*(xdrs)->x_ops->x_getlong)(xdrs, longp)

#define					XDR_PUTLONG(xdrs, longp)			\
						(*(xdrs)->x_ops->x_putlong)(xdrs, longp)
#endif
