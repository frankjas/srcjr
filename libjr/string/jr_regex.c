#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

#include "jr/string.h"
#include "jr/alist.h"
#include "jr/malloc.h"
#include "jr/error.h"

jr_int jr_RegExMatch (
	const char *				source_string,
	const char *				pattern_string,
	jr_int						flags,
	char *						error_buf)
{
	regex_t						regex_info[1];
	jr_int						undo_regex_info			= 0;
	jr_int						reg_flags				= 0;
	jr_int						num_sub_pats			= 1;
	regmatch_t					sub_pat_array[1];


	jr_int						status;


	if (flags & jr_REGEX_POSIX_EXTENDED_FLAG) {
		reg_flags	|= REG_EXTENDED;
	}
	if (flags & jr_REGEX_IGNORE_CASE_FLAG) {
		reg_flags	|= REG_ICASE;
	}

	status	= regcomp (regex_info, pattern_string, reg_flags);

	if (status != 0) {
		jr_esprintf (error_buf, "bad pattern: %s", jr_regex_strerror (status));
		status = jr_MISUSE_ERROR;
		goto return_status;
	}
	undo_regex_info	= 1;

	/*
	** 2-8-2011: regexec() find matches starting anywhere
	*/

	status	= regexec (regex_info, source_string, num_sub_pats, sub_pat_array, 0);

	if (status != 0) {
		if (status == REG_NOMATCH) {
			status = 0;
		}
		else {
			status = jr_INTERNAL_ERROR;
		}
		jr_esprintf (error_buf, "%s", jr_regex_strerror (status));
		goto return_status;
	}
	else if (sub_pat_array[0].rm_so != 0) {
		/*
		** 2-8-2011: need to match the beginning of the string.
		*/
		status = 0;
	}
	else {
		status = 1;
	}

	return_status : {
		if (undo_regex_info) {
			regfree (regex_info);
		}
	}

	return status;
}

const char *jr_RegExSub (
	const char *				source_string,
	const char *				pattern_string,
	const char *				replace_string,
	jr_int						flags,
	char *						error_buf)
{
	const char *				result_string			= 0;
	regex_t						regex_info[1];
	jr_int						undo_regex_info			= 0;
	jr_int						reg_flags				= 0;

	jr_int						num_sub_pats;
	regmatch_t *				sub_pat_array			= 0;

	jr_int						q;
	jr_int						k;
	jr_int						pat_index;

	jr_int						status;
	jr_AList					result_list[1];


	jr_AListInit (result_list, sizeof (char));

	num_sub_pats	= 1 + jr_CountAllOfChar (pattern_string, '\\');
	sub_pat_array	= jr_malloc (num_sub_pats * sizeof (regmatch_t));

	/*
	 * 6/29/2005: num_sub_pats will have twice the number of sub-patterns as \(xx\) pairs
	 */

	if (flags & jr_REGEX_POSIX_EXTENDED_FLAG) {
		reg_flags	|= REG_EXTENDED;
	}
	if (flags & jr_REGEX_IGNORE_CASE_FLAG) {
		reg_flags	|= REG_ICASE;
	}

	status	= regcomp (regex_info, pattern_string, reg_flags);

	if (status != 0) {
		jr_esprintf (error_buf, "bad pattern: %s", jr_regex_strerror (status));
		goto return_result;
	}
	undo_regex_info	= 1;


	while (1) {

		status	= regexec (regex_info, source_string, num_sub_pats, sub_pat_array, 0);

		if (status != 0) {

			for (q = 0; source_string[q]; q++) {
				jr_AListNativeSetNewTail (result_list, source_string[q], char);
			}
			break;
		}

#		if 0
		for (q=0; q < num_sub_pats; q++) {
			if (sub_pat_array[q].rm_so >= 0) {
				printf ("%d: [%d, %d]\n", q, sub_pat_array[q].rm_so, sub_pat_array[q].rm_eo);
			}
		}
#		endif

		/*
		 * 6/29/2005: Sub-pat 0 is the whole match, 1 is the first \(xx\), 2 is the second, etc.
		 */

		for (q=0; q < sub_pat_array[0].rm_so; q++) {
			jr_AListNativeSetNewTail (result_list, source_string[q], char);
		}


		for (q=0; replace_string[q]; q++) {
			
			if (replace_string[q] == '\\') {
				if (isdigit (replace_string[q+1])) {
					pat_index	= atoi (&replace_string[q+1]); 
					for (q++; replace_string[q]; q++) {
						if (!isdigit (replace_string[q])) {
							break;
						}
					}
					q--;

					if (pat_index < num_sub_pats  &&  sub_pat_array[pat_index].rm_so >= 0) {
						for (	k = sub_pat_array[pat_index].rm_so;
								k < sub_pat_array[pat_index].rm_eo;
								k++) {

							jr_AListNativeSetNewTail (result_list, source_string[k], char);
						}
					}
				}
			}
			else {
				jr_AListNativeSetNewTail (result_list, replace_string[q], char);
			}
		}

		source_string	+= sub_pat_array[0].rm_eo;
	}
	

	jr_AListNativeSetNewTail (result_list, 0, char);


	result_string	= jr_AListExtractBuffer (result_list);

	return_result : {
		jr_AListUndo (result_list);
		if (undo_regex_info) {
			regfree (regex_info);
		}
		if (sub_pat_array) {
			jr_free (sub_pat_array);
		}
	}

	return result_string;
}

const char *jr_regex_strerror (
	jr_int					reg_errno)
{
	static char				return_buf[64];
	switch (reg_errno) {
		case REG_NOMATCH		: return "didn't find a match";			/* for regexec() */

		/* POSIX regcomp return error codes.  (In the order listed in the standard.)  */

		case REG_BADPAT			: return "invalid pattern";
		case REG_ECOLLATE		: return "not implemented";
		case REG_ECTYPE			: return "invalid character class name";
		case REG_EESCAPE		: return "trailing backslash";
		case REG_ESUBREG		: return "invalid back reference";
		case REG_EBRACK			: return "unmatched left bracket";
		case REG_EPAREN			: return "parenthesis imbalance";
		case REG_EBRACE			: return "unmatched \\{";
		case REG_BADBR			: return "invalid contents of \\{\\}";
		case REG_ERANGE			: return "invalid range end";
		case REG_ESPACE			: return "ran out of memory";
		case REG_BADRPT			: return "io preceding re for repetition op";

		/* Error codes we've added */

		case REG_EEND			: return "Premature end";
		case REG_ESIZE			: return "Compiled pattern bigger than 2^16 bytes";
		case REG_ERPAREN		: return "Unmatched ) or \\); not returned from regcomp";
	}

	snprintf (return_buf, sizeof (return_buf), "unknown error code %d", reg_errno);

	return return_buf;
}
