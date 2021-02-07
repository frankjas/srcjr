#include "ezport.h"

#include <string.h>
#include "jr/string.h"
#include "jr/trace.h"
#include "jr/malloc.h"

jr_int jr_CountSubByteMatches(source_buf_arg, source_len, pattern_arg, patlen)
	const void *	source_buf_arg ;
	jr_int			source_len ;
	const void *	pattern_arg ;
	jr_int			patlen ;
{
	char *	source_buf = (char *) source_buf_arg ;
	char *	pattern = (char *) pattern_arg ;
	jr_int number_of_matches ;
	jr_int i ;

	if ((source_len == 0) && (patlen == 0)) return(1) ;
	if (source_len == 0) return(0) ;
	if (patlen     == 0) return(source_len) ;

	number_of_matches = 0 ;
	for (i = 0 ; i <= (source_len - patlen); i++) {
		if (memcmp(source_buf+i, pattern, patlen) == 0) {
			number_of_matches++ ;
			i += patlen ;
		}
	}

	return(number_of_matches) ;
}

void *	jr_ReplaceSubByteMatches(source_buf_arg, source_len, old_pattern_arg, old_pattern_len, new_pattern_arg, new_pattern_len)
	const void *	source_buf_arg ;
	jr_int			source_len ;
	const void *	old_pattern_arg ;
	jr_int			old_pattern_len ;
	const void *	new_pattern_arg ;
	jr_int			new_pattern_len ;
{
	char *		source_buf = (char *) source_buf_arg ;
	char *		old_pattern = (char *) old_pattern_arg ;
	char *		new_pattern = (char *) new_pattern_arg ;
	char *		target_buf ;
	jr_int		target_len ;
	jr_int		additional_bytes_needed ;
	jr_int		number_of_occurences ;
	jr_int		source_index ;
	jr_int		target_index ;

	number_of_occurences	= jr_CountSubByteMatches(source_buf,source_len,old_pattern,old_pattern_len) ;
	additional_bytes_needed	= number_of_occurences * (new_pattern_len - old_pattern_len) ;
  	target_len				= source_len + additional_bytes_needed ;

	target_buf				= (char *) jr_malloc((target_len + 1) * sizeof(char)) ;
	if (target_buf == 0) {
		return(0) ;
	}

	target_buf[target_len] = 0 ;

	target_index = 0 ; source_index = 0 ;
	while (source_index <= (source_len - old_pattern_len)) {

		if (memcmp(source_buf+source_index, old_pattern, old_pattern_len) == 0) {
			/*
			 * memmove take care of overlapping copies
			 */
			memmove(target_buf+target_index, new_pattern, new_pattern_len) ;
			target_index += new_pattern_len ;

			if (old_pattern_len == 0) {
				target_buf[target_index] = source_buf[source_index] ;
				target_index++ ;
				source_index++ ;
				if (source_index == source_len) {
					break ;
				}
			}
			else {
				source_index += old_pattern_len ;
			}
		}
		else {
			target_buf[target_index] = source_buf[source_index] ;
			target_index++ ;
			source_index++ ;
		}
	}

	while (source_index < source_len) {
		target_buf[target_index] = source_buf[source_index] ;
		target_index++ ;
		source_index++ ;
	}

	return(target_buf) ;
}


char *	jr_ReplaceSubStringGlobally(source_buf_arg, old_pattern_arg, new_pattern_arg)
	const char *	source_buf_arg ;
	const char *	old_pattern_arg ;
	const char *	new_pattern_arg ;
{
	return
		jr_ReplaceSubByteMatches(
			source_buf_arg,
			(jr_int) strlen(source_buf_arg),
			old_pattern_arg,
			(jr_int) strlen(old_pattern_arg),
			new_pattern_arg,
			(jr_int) strlen(new_pattern_arg)
		) ;
}

/*
main()
{
	char s1[50] ;
	char s2[50] ;
	char s3[50] ;
	char *new ;

	jr_set_trace(jr_malloc_stats) ;
	jr_set_trace(jr_malloc_calls) ;

	while (1) {
		printf("Enter the long string in which replacements will be made: ") ;
		if (! gets(s1)) exit(0) ;
		printf("Enter the pattern: ") ;
		if (! gets(s2)) exit(0) ;
		printf("Enter the new: ") ;
		if (! gets(s3)) exit(0) ;
		printf("position of '%s' in '%s' is %d\n",s2,s1, strstr(s1,s2) - s1) ;
		if (new = jr_ReplaceSubByteMatches(s1,strlen(s1),s2,strlen(s2),s3,strlen(s3))) {
			printf("New string '%s'\n",new) ;
		}
		else {
			perror("jr_ReplaceSubStringGlobally failed ") ;
		}
		jr_free(new) ;
	}

	jr_malloc_stats(stdout, "After loop") ;
}
*/

