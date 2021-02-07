#include "ezport.h"

#include "jr/io.h"
#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/alist.h"


void *	jr_FileNameReadIntoNewMemory(rfn, length_ptr)
	const char *		rfn ;
	unsigned jr_int *	length_ptr ;
{
	FILE *		rfp ;
	char *		result_buffer ;

	rfp = fopen(rfn, "r") ;

	if (rfp == NULL) return(0) ;

	result_buffer = jr_FilePtrReadIntoNewMemory(rfp, length_ptr) ;

	fclose(rfp) ;

	return(result_buffer) ;
}


void *	jr_FilePtrReadIntoNewMemory(rfp, length_ptr)
	FILE *				rfp ;
	unsigned jr_int *	length_ptr ;
{
	jr_int				num_bytes;
	void *				file_memory;
	jr_AList			char_list[1];
	jr_int				c;

	jr_AListInit (char_list, sizeof (char));

	while ((c = getc (rfp))  !=  EOF) {
		jr_AListNativeSetNewTail (char_list, c, char);
	}
	num_bytes		= jr_AListSize (char_list);		/* must go before adding 0, buffer-extract */

	jr_AListNativeSetNewTail (char_list, 0, char);
	file_memory		= jr_AListExtractBuffer (char_list);

	jr_AListUndo (char_list);

	*length_ptr = num_bytes;

	return (file_memory);
}
