#include <stdio.h>

#include "jr/llist.h"
#include "jr/malloc.h"

extern jr_int jr_intpcmp ();

void main ()
{
	jr_LList *list1, *list2;
	jr_int i, *ip;
	char buf[8], **cp;

	list1 = jr_LListCreate (2 * sizeof(jr_int));
	list2 = jr_LListCreate (2 * sizeof(jr_int));

	printf ("Enter add to head/tail test:\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewTail (list2, &i);
		jr_LListSetNewHead (list1, &i);

		cp = (char **) jr_LListTailPtr(list2);
		printf ("Tail: tail == %x, next addr == %x\n", (unsigned) cp, (unsigned) cp-1);

		cp = (char **) jr_LListHeadPtr(list1);
		printf ("Head: head == %x, next addr == %x\n", (unsigned) cp, (unsigned) cp-1);
	}

	printf ("---- jr_LListSetNewHead\n");
	jr_LListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list1),
			(unsigned) jr_LListTailPtr(list1),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	printf ("---- jr_LListSetNewTail\n");
	jr_LListForEachElementPtr (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list2),
			(unsigned) jr_LListTailPtr(list2),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	printf ("---- jr_LListDeleteHead\n");
	jr_LListDeleteHead (list2);
	jr_LListForEachElementPtr (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list2),
			(unsigned) jr_LListTailPtr(list2),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	printf ("Enter add to sort test:\n");
	jr_LListEmpty (list1);
	jr_LListEmpty (list2);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewElementInOrder (list1, &i, jr_intpcmp);
	}

	jr_LListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list1),
			(unsigned) jr_LListTailPtr(list1),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	printf ("Enter add to unique test:\n");
	jr_LListEmpty (list1);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewElementUniquely (list1, &i, jr_intpcmp);
	}
	jr_LListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list1),
			(unsigned) jr_LListTailPtr(list1),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	printf ("Enter to merge test dest:\n");
	jr_LListEmpty (list1);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewElementInOrder (list1, &i, jr_intpcmp);
	}

	printf ("Enter to merge test source:\n");
	jr_LListEmpty (list2);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewElementInOrder (list2, &i, jr_intpcmp);
	}

	jr_LListMerge (list1, list2, jr_intpcmp);
	jr_LListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list1),
			(unsigned) jr_LListTailPtr(list1),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	printf ("Enter to merge unique test dest:\n");
	jr_LListEmpty (list1);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewElementUniquely (list1, &i, jr_intpcmp);
	}

	printf ("Enter to merge unique test source:\n");
	jr_LListEmpty (list2);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewElementUniquely (list2, &i, jr_intpcmp);
	}

	jr_LListMergeUniquely (list1, list2, jr_intpcmp);

	jr_LListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list1),
			(unsigned) jr_LListTailPtr(list1),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	jr_LListEmpty (list1);
	jr_LListEmpty (list2);
	printf ("Enter comparison test (list1) :\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewTail (list1, &i);
	}

	printf ("Enter comparison test (list2) :\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewTail (list2, &i);
	}

	printf ("Comparison value %d\n", jr_LListCmp (list1, list2, jr_intpcmp));
	jr_LListEmpty (list1);
	jr_LListEmpty (list2);

	printf ("Saving list: %x\n", list2);
	printf ("Enter save list test  :\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_LListSetNewTail (list2, &i);
	}
	jr_LListForEachElementPtr (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list1),
			(unsigned) jr_LListTailPtr(list1),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');

	jr_LListDestroy (list1);
	list1 = jr_LListDup (list2);
	printf ("Saved list: %x\n", list1);
	jr_LListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			(unsigned) jr_LListHeadPtr(list1),
			(unsigned) jr_LListTailPtr(list1),
			(unsigned) ip, *ip,
			(unsigned) jr_NextLListElementPtr (ip));
	}
	putchar ('\n');


	jr_malloc_stats (stdout, "before empty list1");
	jr_LListDestroy(list1);
	jr_malloc_stats (stdout, "before empty list2");
	jr_LListDestroy(list2);
	jr_malloc_stats (stdout, "after empty");

}
