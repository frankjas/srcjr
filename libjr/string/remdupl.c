#include "ezport.h"

#include <stdio.h>
#include <ctype.h>

void	jr_RemoveContiguousDuplicateChar(s, c)
	char *	s ;
	jr_int	c ;
{
	char *t=s, *f=s ;

	for(; *f; ) {
		*t++ = *f++ ;
		if ((f[-1] == c) && (f[0] == c)) {
			f++ ;
		}
	}
	*t = '\0' ;
}

/*
void main()
{
	char 	result[128] ;
	char *	original = "//f//fline 100002hello 12345 there 6" ; 
	
	strcpy(result, original) ;

	jr_RemoveContiguousDuplicateChar(result, '/') ;

	fprintf(stdout, "jr_RemoveContiguousDuplicateChar('%s', '/') == %s\n",
		original,
		result
	) ;
	exit(0) ;
}
*/
