#include "ezport.h"

#include <ctype.h>
#include "jr/io.h"


void jr_PrintArbitraryData (wfp, void_ptr, data_length, max_width)
	FILE *			wfp;
	const void *	void_ptr;
	jr_int			data_length;
	jr_int			max_width;
{
	const char *	data_ptr				= void_ptr;
	jr_int			truncated_data;
	jr_int			non_printing_data;
	jr_int			i;

	if (data_length > max_width) {
		data_length = max_width - 3;
		truncated_data = 1;
		/*
		 * -3 for the three dots ...
		 * which will always print
		 */
	}
	else {
		truncated_data = 0;
	}

	non_printing_data = 0;
	for (i = 0; i < data_length; i++, data_ptr++) {
		if (!non_printing_data && isprint (*(char *)data_ptr)) {
			putc (*(char *)data_ptr, wfp);
		}
		else {
			non_printing_data = 1;
			putc ('?', wfp);
		}
	} 
	if (truncated_data) {
		putc ('.', wfp);
		putc ('.', wfp);
		putc ('.', wfp);
	}
}
