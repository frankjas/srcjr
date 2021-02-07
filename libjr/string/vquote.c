#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/vector.h"
#include "jr/string.h"

jr_int jr_VectorizeWithQuotes (str, delimiters, white_space, quotes, vector, vector_size)
	const char *	str ;
	const char *	delimiters ;
	const char *	white_space ;
	const char *	quotes ;
	char **			vector ;
	jr_int			vector_size ;
{
	const char *	begin ;
	char *			arg ;
	jr_int			length ;
	jr_int			argc				= 0 ;
	jr_int			in_quote			= 0;
	jr_int			last_char			= 0;
	jr_int			quote_stack[64];
	jr_int			quote_stack_depth	= sizeof( quote_stack) / sizeof( jr_int);

	/*
	 * skip leading white space
	 */
	for (; *str && strchr(white_space, *str); str++) ;

	if (*str == 0) {
		vector[0] = 0;
		return(0) ;
	}

	begin	= str ;
	length	= 0 ;

	for ( ;; str++) {
		
		if ((*str == 0) || (strchr(delimiters, *str)  && !in_quote)) {
			/*
			 * hit the end of a word
			 */
			
			if (argc == (vector_size - 1)) {
				/*
				 * If we have hit the last valid index (where a null should go)
				 * then stick the remaining un-vectorized string into this slot
				 * and return.
				 */
				vector[argc++] = (char *) begin ;
				return(argc) ;
			}
			else {
				/*
				 * Since we remembered the beginning of the string
				 * and have counted the length up to this position
				 * we are ready to save the bytes from the beginning
				 * of the last argument to the current position.
				 *
				 * Remove trailing white space
				 */
				const char *	end;
				const char *	quote_ptr;

				for (end = begin + length;  end > begin; end--) {
					if (strchr (white_space, end[-1]) == 0) {
						break;
					}
				}
				length = end - begin;

				if ((quote_ptr = strchr( quotes, begin[0])) != 0
					&& begin[length - 1] == *quote_ptr) {
					/*
					** 1-17-2012: the first char is a quote and there's
					** a matching quote in the last char.  Strip both and
					** but leave leading/trailing white space. Why? Bad design
					** and now backward compatibility.
					*/
					begin++;
					length -= 2;
				}

				arg = (char *) jr_malloc(length+1) ;
				strncpy(arg, begin, length) ;
				arg[length] = 0 ;

				vector[argc++] = arg ;
			}

			if (*str == 0) break ;

			/*
			 * skip leading white space on next word
			 * (*str is a delimiter)
			 */

			for (str++; *str && strchr(white_space, *str) ; str++) ;

			if (*str == 0) break ;

 			begin	= str ;
			length	= 0 ;

			if (*str && strchr(delimiters, *str)) {
				str-- ;
				continue ;
			}
		}

		if (*str  &&  strchr (quotes, *str) && last_char != '\\') {
			jr_int		stack_index		= in_quote;

			if (stack_index >= quote_stack_depth) {
				/*
				** 1-17-2012: we can only store quote_stack_depth levels of quote nesting.
				** If we excede that, then we don't properly match open/close pairs, e.g.
				** a begin single quote may be closed by a double quote.
				*/
				stack_index	= -1;
			}

			if (in_quote  &&  (stack_index == -1  ||  quote_stack[stack_index - 1] == *str)) {
				in_quote --;
			}
			else {
				quote_stack[stack_index]	= *str;
				in_quote ++;
			}
		}
		/*
		 * \\" does not escape the "
		 */
		if (last_char == '\\') {
			last_char = 0;
		}
		else {
			last_char = *str;
		}

		/*
		 * If the char is not a delimiter than it is
		 * part of an argument, so increment the length
		 * of that argument.
		 */
		length++ ;
	}
	vector[argc] = 0 ;
	return(argc) ;
}

const char ** jr_VectorCreateWithQuotes (str, delimiters, white_space, quotes)
	const char *	str ;
	const char *	delimiters ;
	const char *	white_space ;
	const char *	quotes ;
{
	char **			new_vector;		
	jr_int			max_elem			= 8;
	jr_int			num_elem;

	new_vector					= jr_malloc (max_elem * sizeof (char *));
	new_vector[max_elem - 1]	= (char *) str;
	num_elem					= 0;


	while (1) {
		
		num_elem		+= jr_VectorizeWithQuotes (
							str, delimiters, white_space, quotes,
							new_vector + num_elem, max_elem - num_elem
						);

		if (num_elem  ==  max_elem) {
			str			= new_vector[max_elem - 1];
			num_elem	= max_elem - 1;

			max_elem	*= 2;
			new_vector	= jr_realloc (new_vector, max_elem * sizeof (char *));
		}
		else {
			break;
		}
	}

	return (const char **) new_vector;
}
