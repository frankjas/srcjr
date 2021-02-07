#include "ezport.h"

#include <stdio.h>

#include "jr/vector.h"
#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/alist.h"

char **	jr_VectorReadDeclaration(rfp, variable_name, error_buf)
	FILE *	rfp ;
	char *	variable_name ;
	char *	error_buf ;
{
	char		value_buf[4096] ;
	char		linebuf[4096] ;
	jr_AList 	list[1] ;
	char *		new_string ;
	char *		trailing_quote ;
	char *		start_ptr ;
	jr_int		n ;

	jr_AListInit(list, sizeof(char *)) ;

	/* fprintf(wfp, "char *	%s[] = {\n", variable_name) ; */
	fgets(linebuf, sizeof(linebuf), rfp) ;

	n = sscanf(linebuf, "char * %[^[ \t\n][] = {", variable_name) ;
	if (n != 1) {
		jr_esprintf (error_buf, "first line of vector declaration malformed") ;
		goto err_return ;
	}

	while (fgets(linebuf, sizeof(linebuf), rfp) != NULL) {
		start_ptr = jr_FirstNonWhiteSpacePtr(linebuf) ;
		if (*start_ptr == '}') break ;

		if (strncmp(start_ptr, "(char", 5) == 0) {
			continue ;
		}

		n = sscanf(start_ptr, "\"%[^\n]", value_buf) ;
		if (n != 1) {
			jr_esprintf (error_buf, "malformed vector declaration: '%.32s'",
				linebuf
			) ;
			goto err_return ;
		}
		trailing_quote = strrchr(value_buf, '\"') ;
		if (trailing_quote == 0) {
			jr_esprintf (error_buf, "missing end quote: '%.32s'", linebuf) ;
			goto err_return ;
		}
		*trailing_quote = 0 ;

		jr_EscapedCharsToControlChars(value_buf) ;

		new_string = jr_strdup(value_buf) ;

		jr_AListNativeSetNewTail(list, new_string, char *) ;
	}

	if (0) {
		err_return:

		jr_AListUndo(list) ;
		return(0) ;
	}

	jr_AListNativeSetNewTail(list, 0, char *) ;

	return((char **) jr_AListHeadPtr(list)) ;
}
