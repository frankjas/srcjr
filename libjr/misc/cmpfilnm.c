#include "ezport.h"

#include <string.h>

#include "jr/string.h"

jr_int jr_trailing_filename_pcmp(sa1,sa2)
	char **		sa1 ;
	char **		sa2 ;
{
	char *	trailing1 = strrchr(*sa1, '/') ;
	char *	trailing2 = strrchr(*sa2, '/') ;

	if (trailing1 == 0) trailing1 = *sa1 ;
	else				trailing1++ ;

	if (trailing2 == 0) trailing2 = *sa2 ;
	else				trailing2++ ;

	return(strcmp(trailing1,trailing2)) ;
}
