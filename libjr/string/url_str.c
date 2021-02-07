#define _POSIX_SOURCE 1

#include "ezport.h"

#include <ctype.h>

#include "jr/string.h"
#include "jr/alist.h"



char *jr_StringEncodeForURL (input_string)
	const char *				input_string;
{
	jr_AList					result_chars[1];
	char						small_buf[16];
	char *						encoded_string;
	jr_int						curr_char;
	jr_int						j;


	jr_AListInit (result_chars, sizeof (char));

	for (; *input_string;  input_string++) {
		curr_char	= *input_string;

		switch (curr_char) {
			/*
			 * Characters from O'Reilly's  'HTML: The definitive guide'
			 * 3rd edition, page 196
			 */

			case '$'				:
			case '-'				:
			case '_'				:
			case '.'				:
			case '+'				:
			case '!'				:
			case '*'				:
			case '\''				:
			case '('				:
			case ')'				:
			case ','				: {
				jr_AListNativeSetNewTail (result_chars, curr_char, char);
				break;
			}
			default					: {
				if (isalnum (curr_char)) {
					jr_AListNativeSetNewTail (result_chars, curr_char, char);
				}
				else {
					jr_AListNativeSetNewTail (result_chars, '%', char);
					sprintf (small_buf, "%02X", curr_char);

					for (j=0; small_buf[j]; j++) {
						jr_AListNativeSetNewTail (result_chars, small_buf[j], char);
					}
				}
				break;
			}
		}
	}

	jr_AListNativeSetNewTail (result_chars, 0, char);

	encoded_string	= jr_AListExtractBuffer (result_chars);

	jr_AListUndo (result_chars);

	return encoded_string;
}


char *jr_StringEncodeForQuotes (input_string, quotes_string)
	const char *				input_string;
	const char *				quotes_string;
{
	jr_AList					result_chars[1];
	char						small_buf[16];
	char *						encoded_string;
	jr_int						curr_char;
	jr_int						encode_char;
	jr_int						j;


	jr_AListInit (result_chars, sizeof (char));

	for (; *input_string;  input_string++) {
		curr_char	= *input_string;

		if (	curr_char == '%'
			||	curr_char  <  32  ||  curr_char >= 127) {

			encode_char	= 1;
		}
		else if (strchr (quotes_string, curr_char)) {
			encode_char = 1;
		}
		else {
			encode_char = 0;
		}

		if (encode_char) {
			jr_AListNativeSetNewTail (result_chars, '%', char);
			sprintf (small_buf, "%02X", curr_char);

			for (j=0; small_buf[j]; j++) {
				jr_AListNativeSetNewTail (result_chars, small_buf[j], char);
			}
		}
		else {
			jr_AListNativeSetNewTail (result_chars, curr_char, char);
		}
	}

	jr_AListNativeSetNewTail (result_chars, 0, char);

	encoded_string	= jr_AListExtractBuffer (result_chars);

	jr_AListUndo (result_chars);

	return encoded_string;
}


char *jr_StringDecodeFromURLWithFixReturn (input_string, fix_return_newline)
	const char *				input_string;
	jr_int						fix_return_newline;
{
	jr_AList					result_chars[1];
	char *						encoded_string;
	jr_int						curr_char;

	jr_int						getting_hex_value			= 0;
	jr_int						hex_value					= 0;
	jr_int						num_hex_digits				= 0;
	jr_int						digit_value;

	jr_int						saw_return					= 0;


	jr_AListInit (result_chars, sizeof (char));


	for (; *input_string;  input_string++) {

		curr_char	= *input_string;

		if (getting_hex_value) {
			if (isxdigit (curr_char)  &&  num_hex_digits < 2) {
				if (isdigit (curr_char)) {
					digit_value = curr_char - '0';
				}
				else if (isupper (curr_char)) {
					digit_value = 10 + curr_char - 'A';
				}
				else {
					digit_value = 10 + curr_char - 'a';
				}
				hex_value	= 16 * hex_value + digit_value;
				num_hex_digits ++;
				continue;
			}

			if (hex_value == '\r'  &&  fix_return_newline) {
				/*
				 * On Mac (and probably DOS), input into a TEXTAREA
				 * gets an ASCII 13, 10, sequence.  We need to remove
				 * the ASCII 13, i.e. the '\r' to make it correct
				 * as a C string.
				 */
				saw_return		= 1;
			}
			else {
				if (saw_return  &&  hex_value != '\n') {
					jr_AListNativeSetNewTail (result_chars, '\r', char);
				}

				jr_AListNativeSetNewTail (result_chars, hex_value, char);
				saw_return		= 0;
			}
			getting_hex_value	= 0;
		}

		switch (curr_char) {
			case '%'		: {
				getting_hex_value	= 1;
				hex_value			= 0;
				num_hex_digits		= 0;
				break;
			}

			default					: {
				if (saw_return) {
					/*
					 * Put the return in if it wasn't followed by a '\n'
					 */
					jr_AListNativeSetNewTail (result_chars, '\r', char);
				}

				jr_AListNativeSetNewTail (result_chars, curr_char, char);
				saw_return		= 0;
				break;
			}
		}
	}

	if (saw_return) {
		/*
		 * Put the return in if it wasn't followed by a '\n'
		 */
		jr_AListNativeSetNewTail (result_chars, '\r', char);
	}
	if (getting_hex_value) {
		jr_AListNativeSetNewTail (result_chars, hex_value, char);
	}
	jr_AListNativeSetNewTail (result_chars, 0, char);

	encoded_string	= jr_AListExtractBuffer (result_chars);

	jr_AListUndo (result_chars);

	return encoded_string;
}
