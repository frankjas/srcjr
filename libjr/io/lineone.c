#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/io.h"

jr_int jr_FirstLineOfFileName(rfn, result_buf, result_buf_length)
	const char *	rfn  ;
	char *			result_buf ;
	jr_int			result_buf_length ;
{
	FILE *	rfp ;
	char *	tp ;
	
	rfp = fopen(rfn, "r") ;
	if (rfp == NULL) {
		return(-1) ;
	}

	if (result_buf_length > 0) {
		*result_buf = 0 ;
	}
	fgets(result_buf, result_buf_length, rfp) ;
	fclose(rfp) ;

	if (tp = (char *) strchr(result_buf,'\n')) {
		*tp = 0 ;
	}

	return(0) ;
}
