#define _POSIX_SOURCE 1

#include "ezport.h"

#include <apr-1/apr_time.h>

#include "jr/time.h"

jr_seconds_t jr_time_seconds ()
{
	apr_time_t	curr_time;

	curr_time	= apr_time_now ();

	return apr_time_sec (curr_time);
}

jr_int jr_time_gmt_offset()
{
	apr_time_t		curr_time;
	apr_time_exp_t	curr_time_exp;
	jr_int			status;

	curr_time	= apr_time_now ();

	status	= apr_time_exp_lt( &curr_time_exp, curr_time);

	if (status == APR_SUCCESS) {
		return curr_time_exp.tm_gmtoff;
	}
	return 0;
}

jr_useconds_t jr_time_useconds ()
{
	apr_time_t		curr_time;
	jr_ULong		tmp_ulong;

	curr_time	= apr_time_now ();

	jr_ULongAsgn64( &tmp_ulong, curr_time );
	return tmp_ulong;
}

char *jr_ctime (
	jr_seconds_t				curr_time,
	char *						buf)
{
	apr_time_t					tmp_time;

	if (curr_time < 0) {
		curr_time	= jr_time_seconds();
	}
	tmp_time	= apr_time_from_sec (curr_time);

	apr_ctime (buf, tmp_time);

	return buf;
}

unsigned jr_int jr_useconds_to_seconds (
	jr_useconds_t				useconds)
{
	jr_ULong					tmp_ulong;

	jr_ULongDivideUInt( &tmp_ulong, useconds, 1000000 );

	if (jr_ULongIsUInt( tmp_ulong )) {
		return jr_ULongLSW( tmp_ulong );
	}
	return jr_UINT_MAX;
}

jr_useconds_t jr_seconds_to_useconds(
	jr_seconds_t				seconds)
{
	jr_useconds_t				useconds;

	
	jr_ULongAsgnUInt (&useconds, seconds);
	jr_ULongMultUInt (&useconds, useconds, 1000000);

	return useconds;
}


void jr_useconds_init (
	jr_useconds_t *				interval_ptr,
	unsigned jr_int				seconds,
	unsigned jr_int				useconds)
{
	jr_ULongAsgnUInt (interval_ptr, seconds);
	jr_ULongMultUInt (interval_ptr, *interval_ptr, 1000000);
	jr_ULongAddUInt (interval_ptr, *interval_ptr, useconds);
}

void jr_useconds_subtract(
	jr_useconds_t *				dest_ref,
	jr_useconds_t				v1,
	jr_useconds_t				v2)
{
	if (jr_ULongCmp( v1, v2) < 0) {
		jr_useconds_init( dest_ref, 0, 0);
	}
	else {
		jr_ULongSubtract( dest_ref, v1, v2);
	}
}
