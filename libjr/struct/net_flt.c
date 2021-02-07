#define _POSIX_SOURCE 1

#include "ezport.h"

#include "jr/nettype.h"
#include "jr/xdr.h"



static XDR		FloatEncodeXDR[1];
static XDR		FloatDecodeXDR[1];
static jr_int	DidInit			= 0;

static float	FloatEncodeBuf[16];	
static float	FloatDecodeBuf[16];
/*
 * Only need 1 float, verified by examining source for xdr_float
 * make it 16 to be sure.
 */

#define FloatXDR_InitIfNecessary()		\
		( DidInit ? 0 : (FloatXDR_Init (), DidInit = 1))

static void FloatXDR_Init ()
{
	xdrmem_create (FloatEncodeXDR, (caddr_t) FloatEncodeBuf, sizeof (FloatEncodeBuf), XDR_ENCODE);
	xdrmem_create (FloatDecodeXDR, (caddr_t) FloatDecodeBuf, sizeof (FloatDecodeBuf), XDR_DECODE);
	/*
	 * no need to Undo, since xdr_destroy, for mem xdr's is a null op
	 */
	DidInit = 1;
}

float jr_NetFloatToHost (net_float)
	jr_NetFloat			net_float;
{
	float				tmp_float		= net_float.net_float_value;
	
	FloatXDR_InitIfNecessary ();

	FloatDecodeBuf[0]		= net_float.net_float_value;

	xdr_setpos (FloatDecodeXDR, 0);
	xdr_float (FloatDecodeXDR, &tmp_float);

	return tmp_float;
}

void jr_NetFloatAsgnFloat (net_float_ptr, host_value_dbl)
	jr_NetFloat *		net_float_ptr;
	double				host_value_dbl;
{
	float				host_value_flt		= host_value_dbl;

	FloatXDR_InitIfNecessary ();

	xdr_setpos (FloatEncodeXDR, 0);
	xdr_float (FloatEncodeXDR, &host_value_flt);

	net_float_ptr->net_float_value		= FloatEncodeBuf[0];
}

