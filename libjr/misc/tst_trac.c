#include "ezport.h"

#include <stdio.h>

#include "jr/trace.h"

void main()
{
	if (jr_do_trace(jr_malloc_calls)) {
		printf("should not print\n") ;
	}

	jr_set_trace(jr_matrix) ;

	if (jr_do_trace(jr_matrix)) {
		printf("matrix is on\n") ;
	}
}
