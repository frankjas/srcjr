#include "ezport.h"

#include "time.h"
#include "jr/misc.h"

main ()
{
	struct tm from_local_time_sp[1] ;
	struct tm from_jr_tstotmsp_sp[1] ;
	char ctime_str[128] ;
	char tstoi_str[128] ;
	char jr_tmsptoi_str[128] ;
	char jr_timestring_str[128] ;
	time_t from_time ;
	time_t from_local_time ;
	time_t from_jr_tmsptoi ;
	time_t from_tstoi ;

	time(&from_time) ;

	bcopy(localtime(&from_time), from_local_time_sp, sizeof(from_local_time_sp)) ;
	/*
	from_local_time = timelocal(from_local_time_sp) ;
	*/
	jr_tmsptoi(from_local_time_sp, &from_jr_tmsptoi) ;
	strcpy(jr_tmsptoi_str, asctime(from_local_time_sp)) ;

	strcpy(ctime_str, ctime(&from_time)) ;
	strcpy(jr_timestring_str, jr_timestring()) ;
	tstoi(ctime_str, &from_tstoi) ;
	strcpy(tstoi_str, ctime(&from_tstoi)) ;
	bcopy(jr_tstotmsp(ctime_str), from_jr_tstotmsp_sp, sizeof(from_jr_tstotmsp_sp)) ;

	jr_RemoveTrailingWhiteSpace(jr_timestring_str) ;
	jr_RemoveTrailingWhiteSpace(ctime_str) ;
	jr_RemoveTrailingWhiteSpace(tstoi_str) ;

	fprintf(stdout, "time()                     == %010ld\n", from_time) ;
	/*
	fprintf(stdout, "timelocal(localtime())     == %010ld\n", from_local_time) ;
	*/
	fprintf(stdout, "jr_tmsptoi(localtime())       == %010ld\n", from_jr_tmsptoi) ;
	fprintf(stdout, "ctime(%010ld)          ==            %s\n", from_time, ctime_str) ;
	fprintf(stdout, "jr_timestring()               ==            %s\n", jr_timestring_str) ;
	fprintf(stdout, "tstoi(%17.17s)   == %010ld %s\n", ctime_str, from_tstoi, tstoi_str) ;


}
