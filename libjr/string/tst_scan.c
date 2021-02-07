#include "ezport.h"

#include <stdio.h>

#include "jr/trace.h"
#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/string.h"

void main()
{
	char	sid_string_buf[128] ;
	char	last_buf[128] ;
	char	first_buf[128] ;
	char	empty_buf[16] ;
	jr_int	num_items ;
	float	float_value ;
	double	double_value ;
	jr_int	int_value ;

	strcpy(sid_string_buf, "garbage") ;
	strcpy(last_buf, "garbage") ;
	strcpy(first_buf, "garbage") ;
	strcpy(empty_buf, "garbage") ;

	float_value = -1 ;
	double_value = -1 ;
	int_value = -1 ;

	num_items = jr_ColumnScanf(
		"123456789RUSSELL,    willard 1234.28.8771",			/* sample input */
		"%[0-9]%6.15[A-Z]%.0[^,], %8.6s %6f %4lf %2.1d",		/* control string */
		sid_string_buf,							/* should be 123456789 */
		last_buf,								/* should be RUSSEL */
		empty_buf,								/* place holder for absorbed chars (after comma) */
		first_buf,								/* should be willa  (6 on output includes null) */
		&float_value,							/* should be 1234.2 */
		&double_value,							/* should be 8.87 */
		&int_value								/* should be 7 */
	) ;

	fprintf(stdout, "ITEMS: 7		   %d\n", num_items) ;
	fprintf(stdout, "SID:   123456789 '%s'\n", sid_string_buf) ;
	fprintf(stdout, "LAST:  RUSSEL    '%s'\n", last_buf) ;
	fprintf(stdout, "EMPTY:           '%s'\n", empty_buf) ;
	fprintf(stdout, "FIRST: willa     '%s'\n", first_buf) ;
	fprintf(stdout, "FLOAT:  1234.20  '%6.2f'\n", float_value) ;
	fprintf(stdout, "DOUBLE: 8.870    '%4.3lf'\n", double_value) ;
	fprintf(stdout, "INT:    7        '%d'\n", int_value) ;

	num_items = jr_ColumnScanf(
		"123456789RUSSELL,    skipped willard 1234.28.8771",	/* sample input */
		"%.*[^,], %*.*[^ ]",										/* control string */
		4,
		sid_string_buf,									/* should be 123 */
		8,												/* 8 columns of input */
		5,												/* 5 chars available in buf */
		last_buf										/* should be skip */


	) ;

	fprintf(stdout, "ITEMS: 2		   %d\n", num_items) ;
	fprintf(stdout, "SID:   123       '%s'\n", sid_string_buf) ;
	fprintf(stdout, "last:  skip      '%s'\n", last_buf) ;

	exit(0) ;
}

