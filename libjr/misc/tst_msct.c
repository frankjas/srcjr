#include "ezport.h"

#include <stdio.h>

main ()
{
	jr_int i;
	char buf[512];

	strcpy (buf, "XXXXXXXXXX");
	printf ("%s\n", buf);

	jr_memscoot (buf, strlen(buf) + 1, 11, 2);

	printf ("%s\n", buf);
	strncpy (buf + 10, "OO", 2);
	printf ("%s\n", buf);

	/*
	fprintf (stderr, "Enter numbers for jr_next2power test:\n");
	while (gets (buf) && (i = atoi (buf))) {
		fprintf (stderr, "%d jr_next2power == %d\n",
			i, jr_next2power(i));
	}
	*/
}

