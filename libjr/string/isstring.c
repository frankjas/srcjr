#include "ezport.h"

#include <ctype.h>
#include "jr/string.h"

jr_int jr_IsString (data_ptr, data_length)
	char *			data_ptr;
	jr_int			data_length;
{
	jr_int i;

	for (i=0; i < data_length - 1; i++) {
		if (data_ptr [i] == 0) break;

		if (!isprint (data_ptr [i])  &&  !isspace (data_ptr [i])) {
			return (0);
		}
	}
	if (data_ptr [i] != 0) return (0);

	return (1);
}
