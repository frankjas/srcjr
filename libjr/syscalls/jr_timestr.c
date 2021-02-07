#include "ezport.h"

#include <string.h>
#include <time.h>
#include <apr-1/apr_time.h>

#include "jr/time.h"
#include "jr/error.h"

extern void		jr_TimeInitFromTM (
					jr_TimeStruct *			time_ptr,
					struct tm *				tm_ptr);

extern void		jr_TimeToTM (
					jr_TimeStruct *			time_ptr,
					struct tm *				tm_ptr);

extern void		jr_TimeToAprTimeExp (
					jr_TimeStruct *				time_ptr,
					apr_time_exp_t *			tm_ptr);

extern void		jr_TimeInitFromAprTimeExp (
					jr_TimeStruct *				time_ptr,
					apr_time_exp_t *			tm_ptr);

void jr_TimeInit (
	jr_TimeStruct *				time_ptr,
	jr_seconds_t				time_seconds)
{
	jr_useconds_t				time_useconds	= jr_seconds_to_useconds( time_seconds);
	apr_time_t					apr_time		= jr_ULongToHost64( time_useconds);
	apr_time_exp_t				tm_value;

	apr_time_exp_gmt( &tm_value, apr_time);

	jr_TimeInitFromAprTimeExp (time_ptr, &tm_value);

	time_ptr->is_gmt			= 1;
}

#ifndef missing_strptime

jr_int jr_TimeInitFromString (
	jr_TimeStruct *				time_ptr,
	const char *				time_string,
	char *						error_buf)
{
	struct tm					tm_value;
	const char *				finish_ptr;

	memset( &tm_value, 0, sizeof(tm_value));

	/*
	** 4-9-2017: strptime() accepts but doesn't process timezone info
	*/
	finish_ptr	= strptime (time_string, "%a %b %d %T %Z %Y", &tm_value);

	if (finish_ptr != 0) {
		jr_esprintf( error_buf, "can't process timezone");
		return jr_MISUSE_ERROR;
	}

	finish_ptr	= strptime (time_string, "%a, %d %b %Y %T%z", &tm_value);

	if (finish_ptr != 0) {
		jr_esprintf( error_buf, "can't process timezone");
		return jr_MISUSE_ERROR;
	}

	finish_ptr	= strptime (time_string, "%a, %d %b %Y %T", &tm_value);
	
	if (finish_ptr == 0) {
		finish_ptr	= strptime (time_string, "%a %b %d %T %Y", &tm_value);
	}

	if (finish_ptr) {
		jr_TimeInitFromTM (time_ptr, &tm_value);

		if (strstr (time_string, "GMT")) {
			time_ptr->is_gmt		= 1;
		}
	}

	return 0;
}

#endif

char *jr_TimeToString (
	jr_TimeStruct *				time_ptr,
	char *						string,
	jr_int						max_length)
{
	apr_time_exp_t				tm_value;
	apr_size_t					str_len;

	jr_TimeToAprTimeExp (time_ptr, &tm_value);

	apr_strftime (string, &str_len, max_length, "%a, %d %b %Y %T", &tm_value);

	if (time_ptr->is_gmt  &&  (jr_int) strlen (string) + 4 + 1 < max_length) {
		strcat (string, " GMT");
	}
	return string;
}

jr_int jr_DaysPerMonth [] = { 0, 31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

#ifdef missing_apr

jr_seconds_t jr_TimeToSecondsUTC (
	jr_TimeStruct *				time_ptr)
{
	jr_seconds_t				seconds;

	jr_int						leapyr ;
	jr_int						year ;

	leapyr		= ((time_ptr->year %4) == 0) ? 1 : 0;

	year		= time_ptr->year - 68 ;
	seconds		= (((year - 2)*365) + ((year - 1)/4)) ;
	/*
	** seconds now equals the number of days since 01/01/1970 
	** to the beginning of the year specified
	*/

	seconds		+= jr_DaysPerMonth[time_ptr->month] ;

	if (leapyr && (time_ptr->month > 1)) {
		(seconds)++ ;
	}
	/*
	** seconds now equals the number of days since 01/01/70
	** to the beginning of the year and month specified
	*/

	seconds		+= (time_ptr->month_day - 1) ;
	/*
	** seconds now equals the number of days since 01/01/70
	** to the year month and date specified
	*/

	seconds = (seconds * 24L) + time_ptr->hour - ((time_ptr->is_dst)?1:0) ;
	seconds = (seconds * 60L) + time_ptr->minute ;
    seconds = (seconds * 60L) + time_ptr->second ;

	if (! time_ptr->is_gmt) {
		struct timezone				tzs ;
		struct timeval				tvs ;

		gettimeofday(&tvs,&tzs) ;
		
		seconds += tzs.tz_minuteswest * 60;
	}

	return seconds;
}

#else

jr_seconds_t jr_TimeToSecondsUTC (
	jr_TimeStruct *				time_ptr)
{
	apr_time_exp_t				tm_value;
	apr_time_t					apr_time;

	jr_TimeToAprTimeExp (time_ptr, &tm_value);
	/*
	** 4-9-2017: will fail on 12/31/1969 16:00 since jr_Time doesn't
	** propogate the gmtoffset
	*/

	apr_time_exp_gmt_get( &apr_time, &tm_value);

	return apr_time/1000000;
}

#endif

void jr_TimeToTM (
	jr_TimeStruct *				time_ptr,
	struct tm *					tm_ptr)
{
	tm_ptr->tm_sec				= time_ptr->second;
	tm_ptr->tm_min				= time_ptr->minute;
	tm_ptr->tm_hour				= time_ptr->hour;
	tm_ptr->tm_mday				= time_ptr->month_day;
	tm_ptr->tm_mon				= time_ptr->month;
	tm_ptr->tm_year				= time_ptr->year;
	tm_ptr->tm_wday				= time_ptr->week_day;
	tm_ptr->tm_yday				= time_ptr->year_day;
	tm_ptr->tm_isdst			= time_ptr->is_dst != 0;
}


void jr_TimeInitFromTM (
	jr_TimeStruct *				time_ptr,
	struct tm *					tm_ptr)
{
	memset (time_ptr, 0, sizeof (*time_ptr));

	time_ptr->second			= tm_ptr->tm_sec;
	time_ptr->minute			= tm_ptr->tm_min;
	time_ptr->hour				= tm_ptr->tm_hour;
	time_ptr->month_day			= tm_ptr->tm_mday;
	time_ptr->month				= tm_ptr->tm_mon;
	time_ptr->year				= tm_ptr->tm_year;
	time_ptr->week_day			= tm_ptr->tm_wday;
	time_ptr->year_day			= tm_ptr->tm_yday;
	time_ptr->is_dst			= tm_ptr->tm_isdst != 0;
}


void jr_TimeToAprTimeExp (
	jr_TimeStruct *				time_ptr,
	apr_time_exp_t *			tm_ptr)
{
	memset( tm_ptr, 0, sizeof(* tm_ptr));

	tm_ptr->tm_usec				= 0;
	tm_ptr->tm_sec				= time_ptr->second;
	tm_ptr->tm_min				= time_ptr->minute;
	tm_ptr->tm_hour				= time_ptr->hour;
	tm_ptr->tm_mday				= time_ptr->month_day;
	tm_ptr->tm_mon				= time_ptr->month;
	tm_ptr->tm_year				= time_ptr->year;
	tm_ptr->tm_wday				= time_ptr->week_day;
	tm_ptr->tm_yday				= time_ptr->year_day;
	tm_ptr->tm_isdst			= time_ptr->is_dst != 0;
	tm_ptr->tm_gmtoff			= time_ptr->gmt_off;
}


void jr_TimeInitFromAprTimeExp (
	jr_TimeStruct *				time_ptr,
	apr_time_exp_t *			tm_ptr)
{
	memset (time_ptr, 0, sizeof (*time_ptr));

	time_ptr->second			= tm_ptr->tm_sec;
	time_ptr->minute			= tm_ptr->tm_min;
	time_ptr->hour				= tm_ptr->tm_hour;
	time_ptr->month_day			= tm_ptr->tm_mday;
	time_ptr->month				= tm_ptr->tm_mon;
	time_ptr->year				= tm_ptr->tm_year;
	time_ptr->week_day			= tm_ptr->tm_wday;
	time_ptr->year_day			= tm_ptr->tm_yday;
	time_ptr->is_dst			= tm_ptr->tm_isdst;
	time_ptr->gmt_off			= tm_ptr->tm_gmtoff;
}


