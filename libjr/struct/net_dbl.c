#define _POSIX_SOURCE 1

#include "ezport.h"

#include "jr/nettype.h"
#include "jr/xdr.h"



static XDR		DoubleEncodeXDR[1];
static XDR		DoubleDecodeXDR[1];
static jr_int	DidInit			= 0;

static double	DoubleEncodeBuf[16];	
static double	DoubleDecodeBuf[16];
/*
 * Only need 1 double, verified in RPC specifications, 8 bytes per double.
 * make it 16 to be sure.
 */

#define DoubleXDR_InitIfNecessary()		\
		( DidInit ? 0 : (DoubleXDR_Init (), DidInit = 1))

static void DoubleXDR_Init ()
{
	xdrmem_create (DoubleEncodeXDR, (caddr_t) DoubleEncodeBuf, sizeof (DoubleEncodeBuf), XDR_ENCODE);
	xdrmem_create (DoubleDecodeXDR, (caddr_t) DoubleDecodeBuf, sizeof (DoubleDecodeBuf), XDR_DECODE);
	/*
	 * no need to Undo, since xdr_destroy, for mem xdr's is a null op
	 */
	DidInit = 1;
}

double jr_NetDoubleToHost (net_double)
	jr_NetDouble			net_double;
{
	double					tmp_double		= net_double.net_double_value;
	
	DoubleXDR_InitIfNecessary ();

	DoubleDecodeBuf[0]		= net_double.net_double_value;

	xdr_setpos (DoubleDecodeXDR, 0);
	xdr_double (DoubleDecodeXDR, &tmp_double);

	return tmp_double;
}

void jr_NetDoubleAsgnDouble (net_double_ptr, host_value)
	jr_NetDouble *		net_double_ptr;
	double				host_value;
{
	DoubleXDR_InitIfNecessary ();

	xdr_setpos (DoubleEncodeXDR, 0);
	xdr_double (DoubleEncodeXDR, &host_value);

	net_double_ptr->net_double_value		= DoubleEncodeBuf[0];
}

