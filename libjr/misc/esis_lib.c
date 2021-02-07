#include "esis_prj.h"

char *jr_ESIS_ReadWordWithIsLast (rfp, char_list, is_last_ptr)
	FILE *							rfp;
	jr_AList *						char_list;
	jr_int *						is_last_ptr;
{
	jr_int							c;

	jr_AListEmpty (char_list);

	while ((c = jr_ESIS_getc (rfp))  !=  EOF) {
		if (c == '\n'  ||  c == ' ') {
			break;
		}
		if (c == jr_ESIS_EMBEDDED_NEWLINE) {
			c = '\n';
		}
		jr_AListNativeSetNewTail (char_list, c, char);
	}

	jr_AListNativeSetNewTail (char_list, 0, char);


	if (is_last_ptr) {
		if (c == '\n') {
			*is_last_ptr = 1;
		}
		else {
			*is_last_ptr = 0;
		}
	}
		
	return jr_AListHeadPtr (char_list);
}

char *jr_ESIS_SaveWordWithIsLast (rfp, char_list, is_last_ptr)
	FILE *							rfp;
	jr_AList *						char_list;
	jr_int *						is_last_ptr;
{
	char *							new_word;

	new_word = jr_strdup (jr_ESIS_ReadWordWithIsLast (rfp, char_list, is_last_ptr));

	return new_word;
}

char *jr_ESIS_ReadData (rfp, char_list)
	FILE *							rfp;
	jr_AList *						char_list;
{
	jr_int							c;

	jr_AListEmpty (char_list);

	while ((c = jr_ESIS_getc (rfp))  !=  EOF) {
		if (c == '\n') {
			break;
		}
		if (c == jr_ESIS_EMBEDDED_NEWLINE) {
			c = '\n';
		}
		jr_AListNativeSetNewTail (char_list, c, char);
	}
	jr_AListNativeSetNewTail (char_list, 0, char);

	return jr_AListHeadPtr (char_list);
}

char *jr_ESIS_SaveData (rfp, char_list)
	FILE *							rfp;
	jr_AList *						char_list;
{
	char *							new_word;

	new_word = jr_strdup (jr_ESIS_ReadData (rfp, char_list));

	return new_word;
}


jr_int jr_ESIS_getc (rfp)
	FILE *				rfp;
{
	jr_int				c;


	c = getc (rfp);

	if (c != '\\') {
		return c;
	}

	c = getc (rfp);

	switch (c) {
		case EOF		: {
			return '\\';
		}

		case '\\'		: {
			return '\\';
		}

		case '|'		: {
			return jr_ESIS_SDATA_BRACKET_CHAR;
		}

		case 'n'		: {
			return jr_ESIS_EMBEDDED_NEWLINE;
		}

		case '#'		:
		case '%'		: {
			/*
			 * Number in decimal
			 */
			jr_int		n = 0;

			while ((c = getc (rfp)) != EOF) {
				if (c >= '0'  &&  c <= '7') {
					n = 10*n + c - '0';
				}
				else {
					ungetc (c, rfp);
					break;
				}
			}
			return n;
		}

		default			: {
			if (isdigit (c)) {
				/*
				 * number in octal
				 */
				jr_int		n = c - '0';

				while ((c = getc (rfp)) != EOF) {
					if (c >= '0'  &&  c <= '7') {
						n = 8*n + c - '0';
					}
					else {
						ungetc (c, rfp);
						break;
					}
				}
				return n;
			}
			ungetc (c, rfp);
			return '\\';
		}
	}
	return EOF;
}



jr_int jr_ESIS_putc (c, wfp)
	jr_int				c;
	FILE *				wfp;
{

	switch (c) {
		case '\\'		: {
			putc ('\\', wfp);
			putc ('\\', wfp);
			return c;
		}

		case '\n'		: {
			putc ('\\', wfp);
			putc ('n', wfp);
			return c;
		}
	}

	if (isprint (c)  &&  (!isspace (c)  ||  c == ' ')) {
		putc (c, wfp);
		return c;
	}

	fprintf (wfp, "\\%o", c);
	return c;
}


