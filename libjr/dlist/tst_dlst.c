#include <stdio.h>

#include "jr/dlist.h"

extern jr_int jr_intpcmp ();

main ()
{
	jr_DList *list1, *list2;
	jr_int i, *ip;
	char buf[8], **cp;

	list1 = jr_DListCreate (2 * sizeof(jr_int));
	list2 = jr_DListCreate (2 * sizeof(jr_int));

	printf ("Enter add to tail test:\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListSetNewTail (list2, &i);
		cp = (char **) jr_DListTailPtr(list2);
		printf ("Tail: tail == %x, next addr == %x prev == %x\n", cp, 
			jr_DListNextElementPtr (list,cp), jr_DListPrevElementPtr (list,cp));
	}
	printf ("---- jr_DListSetNewTail\n");

	jr_DListForEachElementPtr (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x prev %x.\n",
			jr_DListHeadPtr(list2),
			jr_DListTailPtr(list2),
			ip, *ip,
			jr_DListNextElementPtr (list,ip),
			jr_DListPrevElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("---- jr_DListSetNewTail Reverse Printing\n");

	jr_DListForEachElementPtrRev (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x prev %x.\n",
			jr_DListHeadPtr(list2),
			jr_DListTailPtr(list2),
			ip, *ip,
			jr_DListNextElementPtr (list,ip),
			jr_DListPrevElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("Enter add to head test:\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListSetNewHead (list1, &i);
		cp = (char **) jr_DListHeadPtr(list1);
		printf ("Head: head == %x, next addr == %x prev == %x\n", cp, 
			jr_DListNextElementPtr (list,cp), jr_DListPrevElementPtr (list,cp));
	}

	printf ("---- jr_DListSetNewHead\n");
	jr_DListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x prev %x.\n",
			jr_DListHeadPtr(list1),
			jr_DListTailPtr(list1),
			ip, *ip,
			jr_DListNextElementPtr (list,ip),
			jr_DListPrevElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("---- jr_DListSetNewHead Reverse printing\n");
	jr_DListForEachElementPtrRev (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x prev %x.\n",
			jr_DListHeadPtr(list1),
			jr_DListTailPtr(list1),
			ip, *ip,
			jr_DListNextElementPtr (list,ip),
			jr_DListPrevElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("Enter insert test:\n");
	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListForEachElementPtr (list1, ip) {
			if (*ip ==i) break;
		}
		if (!ip) {
			printf ("%d not found.\n", i);
			continue;
		}
		jr_DListSetNewPrev (list1, ip, &i);
		jr_DListForEachElementPtr (list1, ip) {
			printf (
				"Head %x, Tail %x,  ip %x, *ip %d , next %x prev %x.\n",
				jr_DListHeadPtr(list1),
				jr_DListTailPtr(list1),
				ip, *ip,
				jr_DListNextElementPtr (list,ip),
				jr_DListPrevElementPtr (list,ip));
		}
		putchar ('\n');
	}

	printf ("Enter append test:\n");
	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListForEachElementPtr (list1, ip) {
			if (*ip ==i) break;
		}
		if (!ip) {
			printf ("%d not found.\n", i);
			continue;
		}
		jr_DListSetNewNext (list1, ip, &i);
		jr_DListForEachElementPtr (list1, ip) {
			printf (
				"Head %x, Tail %x,  ip %x, *ip %d , next %x prev %x.\n",
				jr_DListHeadPtr(list1),
				jr_DListTailPtr(list1),
				ip, *ip,
				jr_DListNextElementPtr (list,ip),
				jr_DListPrevElementPtr (list,ip));
		}
		putchar ('\n');
	}

	printf ("Enter delete test:\n");
	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListForEachElementPtr (list1, ip) {
			if (*ip ==i) break;
		}
		if (!ip) {
			printf ("%d not found.\n", i);
			continue;
		}
		jr_DListDeleteElement (list1, ip);
		jr_DListForEachElementPtr (list1, ip) {
			printf (
				"Head %x, Tail %x,  ip %x, *ip %d , next %x prev %x.\n",
				jr_DListHeadPtr(list1),
				jr_DListTailPtr(list1),
				ip, *ip,
				jr_DListNextElementPtr (list,ip),
				jr_DListPrevElementPtr (list,ip));
		}
		putchar ('\n');
	}

	jr_malloc_stats (stdout, "before empty list1");
	jr_DListDestroy(list1);

	jr_malloc_stats (stdout, "after empty");

	jr_DListDestroy(list2);
	jr_malloc_stats (stdout, "after empty");

	/* not implemented yet

	printf ("Enter add to sort test:\n");
	jr_DListEmpty (list1);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		AddToSortedDL (list1, &i, jr_intpcmp);
	}
	jr_DListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list1),
			jr_DListTailPtr(list1),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("Enter add to unique test:\n");
	jr_DListEmpty (list1);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListSetNewElementUniquely (list1, &i, jr_intpcmp);
	}
	jr_DListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list1),
			jr_DListTailPtr(list1),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("Enter to merge test dest:\n");
	jr_DListEmpty (list1);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		AddToSortedDL (list1, &i, jr_intpcmp);
	}

	printf ("Enter to merge test source:\n");
	jr_DListEmpty (list2);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		AddToSortedDL (list2, &i, jr_intpcmp);
	}

	MergeDL (list1, list2, jr_intpcmp);
	jr_DListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list1),
			jr_DListTailPtr(list1),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("Enter to merge unique test dest:\n");
	jr_DListEmpty (list1);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		AddToSortedDL (list1, &i, jr_intpcmp);
	}

	printf ("Enter to merge unique test source:\n");
	jr_DListEmpty (list2);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		AddToSortedDL (list2, &i, jr_intpcmp);
	}

	MergeUniqueDL (list1, list2, jr_intpcmp);

	jr_DListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list1),
			jr_DListTailPtr(list1),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	jr_DListEmpty (list1);
	jr_DListEmpty (list2);
	printf ("Enter comparison test (list1) :\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListSetNewTail (list1, &i);
	}

	printf ("Enter comparison test (list2) :\n");

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListSetNewTail (list2, &i);
	}

	printf ("Comparison value %d\n", jr_DListCmp (list1, list2, jr_intpcmp));

	printf ("Saving list: %x\n", list2);
	jr_DListForEachElementPtr (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list2),
			jr_DListTailPtr(list2),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	jr_DListDestroy (list1);
	list1 = Savejr_DList (list2);
	printf ("Saved list: %x\n", list1);
	jr_DListForEachElementPtr (list1, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list1),
			jr_DListTailPtr(list1),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("Enter sort list test :\n");

	jr_DListEmpty (list2);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListSetNewTail (list2, &i);
	}

	SortDL (list2, jr_intpcmp);
	jr_DListForEachElementPtr (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list2),
			jr_DListTailPtr(list2),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	printf ("Enter unique sort list test :\n");

	jr_DListEmpty (list2);

	while (fgets (buf, sizeof (buf), stdin) && (i = atoi (buf))) {
		jr_DListSetNewTail (list2, &i);
	}

	SortUniqueDL (list2, jr_intpcmp);
	jr_DListForEachElementPtr (list2, ip) {
		printf ("Head %x, Tail %x,  ip %x, *ip %d , next %x.\n",
			jr_DListHeadPtr(list2),
			jr_DListTailPtr(list2),
			ip, *ip,
			jr_DListNextElementPtr (list,ip));
	}
	putchar ('\n');

	jr_malloc_stats (stdout, "before empty list1");
	jr_DListDestroy(list1);
	jr_malloc_stats (stdout, "after empty");

	jr_malloc_stats (stdout, "before empty list2");
	jr_DListDestroy(list2);
	jr_malloc_stats (stdout, "after empty");

	********/

}
