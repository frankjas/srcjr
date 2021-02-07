#include "ezport.h"

#include <stdint.h>

#include "jr/misc.h"

void jr_memscoot (buffer_arg, buffer_size, hole_offset, hole_size)
	void *buffer_arg;
	jr_int buffer_size;
	jr_int hole_offset;
	jr_int hole_size;
{
	char *	buffer = buffer_arg ;
	char *	scoot_end;
	jr_int	scoot_size;
	jr_int *	int_scoot_end;
	jr_int	int_scoot_size;
	jr_int	int_hole_size;
	jr_int	i ;

	scoot_end  = buffer + buffer_size;
	scoot_size = buffer_size - hole_offset;

	if (hole_size % sizeof (jr_int)) {
		/*
		 * its an odd # of bytes to open, do it character by character for simplicity
		 */
		for (i = 0;  i < scoot_size;  i++, scoot_end--) {
			scoot_end [hole_size - 1]  =  scoot_end [-1];
		}

		return;
	}

	/*
	 * hole size is word multiple
	 * scoot until scoot_end is on a word boundary
	 */
	for (i = 0;  i < scoot_size  &&  (intptr_t) scoot_end % sizeof (jr_int) != 0;  i++, scoot_end--) {

		if ((intptr_t) scoot_end % sizeof (jr_int) == 0) {
			/*
			 * the end of the section to be copied is on a word boundary
			 */
			break;
		}
		scoot_end [hole_size - 1]  =  scoot_end [-1];
	}

	/*
	 * have scoot_size - i left to scoot
	 */
	int_scoot_end	= (jr_int *) scoot_end;
	int_scoot_size	= (scoot_size - i) / sizeof (jr_int);
	int_hole_size	= hole_size / sizeof (jr_int);
	scoot_size		= (scoot_size - i) % sizeof (jr_int);

	for (i = 0;  i < int_scoot_size;  i++, int_scoot_end--) {
		int_scoot_end [int_hole_size - 1]  =  int_scoot_end [-1];
	}

	scoot_end		= (char *) int_scoot_end;
	for (i = 0;  i < scoot_size;  i++, scoot_end--) {
		scoot_end [hole_size - 1]  =  scoot_end [-1];
	}
}
