#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/vector.h"

void jr_VectorDisplayLikeLs(wfp, vector_arg, num_items_in_vector, display_width)
	FILE *				wfp ;
	const char **		vector_arg ;
	jr_int				num_items_in_vector ;
	jr_int				display_width ;
{
	jr_int		max_length_entry	=	jr_VectorFindMaximum(
											vector_arg, num_items_in_vector,
											(jr_int (*) ()) strlen
										) ;
	jr_int		column_width		=	(max_length_entry + 2) ;
	jr_int		num_columns			=	display_width / column_width ;
	jr_int		num_rows ;
	
	num_rows = (num_items_in_vector / num_columns) ;
	if ((num_items_in_vector % num_columns) > 0) num_rows++ ;

	{
		jr_int	row_index ;
		jr_int	column_index ;
		jr_int	vector_index ;
		jr_int	padding_width ;
		jr_int	i ;

		row_index		= 0 ;
		column_index	= 0 ;
		for (i = 0 ; i < num_items_in_vector; i++) {
			vector_index = ((column_index * num_rows) + row_index) ;
			if (column_index >= (num_columns-1))	padding_width = 1 ;
			else									padding_width = column_width ;

			if (vector_index < num_items_in_vector) {
				fprintf(wfp, "%-*.*s",
					padding_width, column_width, vector_arg[vector_index]
				) ;
			}
			else {
				fprintf(wfp, "%-*.*s",
					padding_width, column_width, ""
				) ;
			}
			column_index++ ;

			if (column_index >= (num_columns-1)) {
				fprintf(wfp, "\n") ;
				column_index	= 0 ;
				row_index++ ;
			}
		}
		if (column_index != 0) {
			for ( ; column_index < num_columns; column_index++) {
				if (column_index >= (num_columns-1))	padding_width = 1 ;
				else									padding_width = column_width ;

				fprintf(wfp, "%-*.*s",
					column_width, column_width, ""
				) ;
			}
			fprintf(wfp, "\n") ;
		}
	}
}
