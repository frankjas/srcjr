#include "ezport.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#define JR_SCANF_USE_ARG_FOR_VALUE		-1
#define JR_SCANF_NO_WIDTH_SPECIFIED		-2
#define JR_SCANF_VARARG_PROTECT			1

#include "jr/string.h"

/*
 * In this module the scanf specifier  %n  will not be supported
 */
typedef struct {
	/* "%15.12s"  means absorb 15 input columns, restrict output string to 11 chars plus null */
	/* "%.12[A-Z] means absorb all the upper case letters and store up to 11 of them plus null */
	/* "%15.12[A-Z] means absorb 15 input columns storing up to 11 upper case letters plus null */
	/* "%15.12f"  means absorb 15 input columns, only first 12 used for float value interpretation */

	/* SET IN INITIALIZATION */
		jr_int	number_of_input_columns ;		/* the 15 in examples above */
		jr_int	output_array_bounds ;			/* the 12 in examples above */
		jr_int	was_leading_zero ;				/* %#010x   the 0 after the pound sign */
		jr_int	was_pound ;						/* the pound sign in example above */
		jr_int	was_left_justified ;			/* %-15.12s  minus sign indicates left justified */

		const char *	start_of_sequence ;		/* a pointer to the place right after the % */
		const char *	middle_of_sequence ;	/* points to the 'ld' in %010ld */
		const char *	end_of_sequence ;		/* a pointer to the  trailing  's' 'f' or 'x' */

	/* SET DURING EXECUTION */
		const char *	new_input_char_ptr ;	/* gives position after input has been interpreted */
		va_list			new_arg_list ;			/* after the args have been used/filled */

} jr_ScanfControlStruct ;

extern void jr_ColumnScanfInitControlStruct(
	jr_ScanfControlStruct *		control_sp,
	const char *				control_char_ptr
) ;

extern jr_int jr_ColumnScanfExecuteControlSequence(
	jr_ScanfControlStruct *	control,		/* use this struct to match and control assignment */
	const char *			input_char_ptr	/* use this string to match against and assign from */
) ;

extern jr_int jr_ScanfSetContainsCharacter(
	const char *	scanf_char_set,
	jr_int			char_value
) ;


jr_int jr_ColumnScanf(
	const char *	input_str,
	const char *	control_str, ...)
{
	va_list			arg_list;
	jr_ScanfControlStruct	control_sp[1] ;

	jr_int			number_of_percent_items_interpreted = 0 ;
	const char *	input_char_ptr ;
	const char *	control_char_ptr ;
	
	va_start (arg_list, control_str);

	input_char_ptr = input_str ;

	for (control_char_ptr = control_str ; *control_char_ptr ; control_char_ptr++) {
#		ifdef JR_SCANF_TRACE
			fprintf(stderr, "START with: %s\n", input_char_ptr) ;
#		endif

		switch (*control_char_ptr) {
			case '%' : {
				control_char_ptr++ ;	/* skip over percent that starts control sequence */

				if (*control_char_ptr != '%') {
					jr_int status ;

					jr_ColumnScanfInitControlStruct(control_sp, control_char_ptr) ;
					va_copy (control_sp->new_arg_list, arg_list);

					control_char_ptr		= control_sp->end_of_sequence ;

					status = jr_ColumnScanfExecuteControlSequence(
						control_sp,				/* use this struct to match and control assignment */
						input_char_ptr			/* use this string to match against and assign from */
					) ;
					if (status != 0) {
						goto return_point ;
					}

#		ifdef JR_SCANF_TRACE
					fprintf(stderr, "current stack ptr == %#010x\n", &arg_list) ;
					fprintf(stderr, "new stack ptr     == %#010x\n", &control_sp->new_arg_list) ;
#		endif
					va_copy (arg_list, control_sp->new_arg_list);


					input_char_ptr			= control_sp->new_input_char_ptr ;

					number_of_percent_items_interpreted++ ;

					break ;
				}
				else {
					/* then fall through because a second percent is like any other char */
				}
			}
			default  : {
				switch(*control_char_ptr) {
					case ' ' : {
						while (isspace(*input_char_ptr)) {

#							ifdef JR_SCANF_TRACE
								fprintf(stderr,"skipping blank on input\n") ;
#							endif

							input_char_ptr++ ;
						}
						break ;
					}
					default : {
						if (*control_char_ptr == *input_char_ptr) {

#							ifdef JR_SCANF_TRACE
								fprintf(stderr,"fixed input matched '%c'\n", *input_char_ptr) ;
#							endif

							input_char_ptr++ ;
						}
						else {
#							ifdef JR_SCANF_TRACE
								fprintf(stderr,"control string '%c' != '%c' so stopping\n",
									*control_char_ptr,
									*input_char_ptr
								) ;
#							endif

							/* input failed to match control string, we are done! */
							goto return_point ;
						}
						break ;
					}
				}

				break ;	/* out of switch on control_char */
			}
		}
	}	/* while there are more control characters */

	return_point:
		va_end (arg_list);
		return(number_of_percent_items_interpreted) ;

}

void jr_ColumnScanfInitControlStruct(control_sp, control_char_ptr)
	jr_ScanfControlStruct *		control_sp ;
	const char *				control_char_ptr ;
{
	memset((void *)control_sp, 0, sizeof(*control_sp)) ;

	control_sp->output_array_bounds 	= JR_SCANF_NO_WIDTH_SPECIFIED ;
	control_sp->number_of_input_columns	= JR_SCANF_NO_WIDTH_SPECIFIED ;

	control_sp->start_of_sequence = control_char_ptr ;

	for ( ; *control_char_ptr ; control_char_ptr++) {
		if (*control_char_ptr == '#') {
			control_sp->was_pound++ ;
			continue ;
		}
		else if (*control_char_ptr == '-') {
			control_sp->was_left_justified++ ;
			continue ;
		}
		else if (*control_char_ptr == '0') {
			control_sp->was_leading_zero++ ;
			continue ;
		}
		else if (isdigit(*control_char_ptr)) {
			if (control_sp->number_of_input_columns < 0) control_sp->number_of_input_columns = 0 ;
			control_sp->number_of_input_columns =
				(control_sp->number_of_input_columns * 10) + (*control_char_ptr - '0') ;
		}
		else if (*control_char_ptr == '*') {
			control_sp->number_of_input_columns = JR_SCANF_USE_ARG_FOR_VALUE ;
			/* break ; commented out FJ 4/14/94 */
			/* so that end_of_sequence won't be 0 if we see %*s */
		}
		else if (*control_char_ptr == '.') {
			break ;
		}
		else {
			control_sp->end_of_sequence = control_char_ptr ;
			goto return_point ;
		}
	}
	
	if (*control_char_ptr == 0) {
		control_sp->end_of_sequence = control_char_ptr ;
		goto return_point ;
	}

	if (*control_char_ptr == '.') {
		control_char_ptr++ ;	/* skip over dot, look for array bounds spec */

		for ( ; *control_char_ptr ; control_char_ptr++) {
			if (*control_char_ptr == '#') {
				continue ;
			}
			else if (isdigit(*control_char_ptr)) {
				if (control_sp->output_array_bounds < 0) control_sp->output_array_bounds = 0 ;
				control_sp->output_array_bounds =
					(control_sp->output_array_bounds * 10) + (*control_char_ptr - '0') ;
			}
			else if (*control_char_ptr == '*') {
				control_sp->output_array_bounds = JR_SCANF_USE_ARG_FOR_VALUE ;
				/* break ;		 commented out 2/27/94 */
			}
			else {
				control_sp->end_of_sequence = control_char_ptr ;
				goto return_point ;
			}
		}
	}
	
	return_point:
		control_sp->middle_of_sequence = control_sp->end_of_sequence ;

		if		(
					(
						(*control_sp->end_of_sequence == 'l') ||
						(*control_sp->end_of_sequence == 'h')
					) &&
						(strchr("doxefg", control_sp->end_of_sequence[1]) != 0)
				) {
			control_sp->end_of_sequence++ ;
		}
		else if (*control_sp->end_of_sequence == '[') {
			while (*control_sp->end_of_sequence) {
				if (*control_sp->end_of_sequence == ']') break ;
				control_sp->end_of_sequence++ ;
			}
		}
		return ;
}

jr_int jr_ColumnScanfExecuteControlSequence(control_sp, input_char_ptr)
	jr_ScanfControlStruct *	control_sp ;		/* use this struct to match and control assignment */
	const char *			input_char_ptr ;	/* use this string to match against and assign from */
{
	char 	my_control_sequence[512] ;
	char *	my_input_string ;
	jr_int	status = 0 ;
	jr_int	i ;

	/* extract the %f or %ld from %#15.12d */ {
		strcpy(my_control_sequence, "%") ;
		strncat(
			my_control_sequence, control_sp->middle_of_sequence,
			control_sp->end_of_sequence - control_sp->middle_of_sequence + 1
		) ;
		/*
		 * 4/14/93 FJ: added strncat to only copy the current format specifier
		 * this copies all the following format specifiers, some of
		 * which will be 'ColumnScanf' style, which will screw up the sscanf
		 * below, which gets called if the thing is a %d, %f, etc.
		 * Note: my_control_sequence is only used with the sscanf, put this code below?
		 */
	}

	/* fill in run time assigned width values as in %*.*s */ {
		if (control_sp->number_of_input_columns == JR_SCANF_USE_ARG_FOR_VALUE) {
			jr_int	width_spec = va_arg(control_sp->new_arg_list, jr_int) ;
			control_sp->number_of_input_columns = width_spec ;
		}
		if (control_sp->output_array_bounds == JR_SCANF_USE_ARG_FOR_VALUE) {
			jr_int	width_spec = va_arg(control_sp->new_arg_list, jr_int) ;
			control_sp->output_array_bounds = width_spec ;
		}
	}

	/*
	 * it is guaranteed that our copy of the input characters won't need to be longer
	 * than the original
	 */
	my_input_string = (char *) alloca(strlen(input_char_ptr) + 1) ;

	/* make our own null ended copy of the input string and increment the input_char_ptr */
	{
		/*
		 * There are two ways the number of input columns for each argument
		 * can be specified: by number as in %15s  OR by 'scanf set' %.15[A-Z]
		 */
		jr_int saw_end_of_matching_chars = 0 ;
		jr_int	i ;

		for (i = 0 ; input_char_ptr[i] ; i++) {
			if (input_char_ptr[i] == 0) break ;

			if ((*control_sp->middle_of_sequence == '[') &&
				(! jr_ScanfSetContainsCharacter(control_sp->middle_of_sequence, input_char_ptr[i]))) {
				saw_end_of_matching_chars++ ;

				if (control_sp->number_of_input_columns < 0) {
					/*
					 * no specific number of columns was specified
					 * and the input characters no longer match our set so break
					 */
					 break ;
				}
			}
			if ((control_sp->number_of_input_columns >= 0) && (i >= control_sp->number_of_input_columns)) {
				/* a specific number of input columns was specified and we are there so break */
				break ;
			}

			/*
			 * added FJ 4/14/94  (modified by will to NOT break if there were %[] chars)
			 */
			if ((*control_sp->middle_of_sequence != '[') &&
				(control_sp->number_of_input_columns == JR_SCANF_NO_WIDTH_SPECIFIED) &&
				isspace(input_char_ptr[i]) ) {
				/*
				 * no input column length was specified, and we saw a blank, so the string is done
				 */
				break;
			}

			if (saw_end_of_matching_chars) {
				my_input_string[i] = 0 ;
			}
			else {
				my_input_string[i] = input_char_ptr[i] ;
			}
		}
		my_input_string[i] = 0 ;
		control_sp->new_input_char_ptr = input_char_ptr + i ;
	}	/* end of block for our own null ended copy of input chars and incrementing input_char_ptr */

	/*
	 * At this point we have a copy of the input in  my_input_string
	 * which is at most number_of_input_columns long and contains
	 * only those characters which were in the specified set (if there was a set).
	 */

	if ((*control_sp->end_of_sequence == 's') || (*control_sp->middle_of_sequence == '[')) {
		char *	output_buffer = va_arg(control_sp->new_arg_list, char *) ;
		char *	input_ptr ;

		/*
		 * Now we will copy just the portion of  my_input_string
		 * which will fit into the output_buffer
		 */

		if (*my_input_string == 0   &&
			(*control_sp->middle_of_sequence != '['  &&  control_sp->number_of_input_columns != 0)) {
			/*
			 * added 4/28/95, if there is not enough input, we
			 * should return a bad status, except if a set was requested or
			 * 0 columns were requested (%[] implies 0 or more characters matching).
			 */
			status = -1;
		}
		else if (control_sp->output_array_bounds >= 0) {
			input_ptr = my_input_string ;
			for (i=0; i < (control_sp->output_array_bounds - 1); i++) {

				output_buffer[i] = *input_ptr ;
				if (*input_ptr) input_ptr++ ;
			}
			output_buffer[i] = 0 ;
		}
		else {
			strcpy(output_buffer, my_input_string) ;
		}
	}
	else {
		/*
		 * Now we will the real scanf handle all the numeric conversions
		 */
		jr_int	item_was_scanned ;

		my_input_string[control_sp->output_array_bounds] = 0 ;
		item_was_scanned = sscanf(
			my_input_string,
			my_control_sequence,
			va_arg(control_sp->new_arg_list, void *)
		) ;
		if (item_was_scanned != 1) {
			/*
			 * FJ 4/14/94: sscanf may return -1 (EOF) if input error
			 * occurs before any conversion (i.e. a null string)
			 */
			status = -1 ;
		}
	}

	return(status) ;
}

jr_int jr_ScanfSetContainsCharacter(scanf_char_set_arg, char_value)
	const char *	scanf_char_set_arg ;
	jr_int			char_value ;
{
	const char *	scanf_char_set				= scanf_char_set_arg ;
	jr_int			reverse_sense_of_comparison = 0 ;
	jr_int			matched						= 0;
	jr_int			left_side_of_dash			= 077777;
	jr_int			scanf_set_character ;
	jr_int			return_value ;


	if (*scanf_char_set == '[') {
		scanf_char_set++ ;	/* skip over opening '[' */

		if (*scanf_char_set == '^') {
			scanf_char_set++ ;	/* skip over and remember NOT character '^' */
			reverse_sense_of_comparison++ ;
		}
	}

	while (scanf_set_character = *scanf_char_set++) {
		if (scanf_set_character == ']') {
			break ;
		}
		if (scanf_set_character == '-') {
			if ((left_side_of_dash <= char_value) && (char_value <= *scanf_char_set++)) {
				matched++;
			}
		}
		else {
			if (char_value == (left_side_of_dash = scanf_set_character)) {
				matched++;
			}
		}
	}

	return_value = (reverse_sense_of_comparison) ? (! matched) : matched ;

#	ifdef JR_SCANF_TRACE
		fprintf(stderr,"%s: %c is %s part of set\n",
			scanf_char_set_arg,
			char_value,
			return_value ? "" : "NOT"
		) ;
#	endif

	return(return_value) ;
}
