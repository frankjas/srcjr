#include "ezport.h"

#include <stdio.h>

#include "jr/malloc.h"
#include "jr/circbuf.h"

void main ()
{
	jr_CB_CircularBuffer circbuf[1];
	jr_int i, element;

	jr_CB_CircularBufferInit (circbuf, 10, sizeof(jr_int));

	for (element=0; element < 15; element++) {
		jr_CB_SetNewTail (circbuf, &element, 0);
	}

	printf ("Size %d\n", jr_CB_CircularBufferSize (circbuf));
	jr_CB_ForEachElementIndex (circbuf, i) {
		Getjr_CB_ElementPtr (circbuf, i, &element);
		printf ("%d: %d\n", i, element);
	}

	for (i=0; i < 3; i++) {
		jr_CB_GetHead (circbuf, &element);
		printf ("head: %d\n", element);
		jr_CB_GetTail (circbuf, &element);
		printf ("tail: %d\n", element);
	}

	printf ("Size %d\n", jr_CB_CircularBufferSize (circbuf));
	jr_CB_ForEachElementIndex (circbuf, i) {
		Getjr_CB_ElementPtr (circbuf, i, &element);
		printf ("%d: %d\n", i, element);
	}

	for (element=0; element < 15; element++) {
		jr_CB_SetNewHead (circbuf, &element, 0);
	}

	printf ("Size %d\n", jr_CB_CircularBufferSize (circbuf));
	jr_CB_ForEachElementIndex (circbuf, i) {
		Getjr_CB_ElementPtr (circbuf, i, &element);
		printf ("%d: %d\n", i, element);
	}

	jr_CB_CircularBufferUndo (circbuf);
	jr_malloc_stats (stdout, "After jr_CB_CircularBufferUndo");
}
