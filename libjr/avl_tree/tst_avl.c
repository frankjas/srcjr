#include <stdio.h>
#include "../project.h"

void print_integer(node, wfp)
	jr_int *	node ;
	FILE *	wfp;
{
	fprintf (wfp, "%4d", *node);
}

jr_int jr_intpcmp(np,mp)
	jr_int *np, *mp ;
{
	return((*np)-(*mp)) ;
}

jr_int main()
{
	jr_AVL_Tree avl_tree[1];
	char lbuf[512] ;
	jr_int *node ;
	jr_int key ;

	jr_AVL_TreeInit (avl_tree, sizeof (jr_int), jr_intpcmp);

	fprintf(stderr, "Number to insert? ") ;
	while ( gets(lbuf) ) {

		key = atoi(lbuf) ;

		node = jr_AVL_TreeFindElementPtr(avl_tree, &key);

		if (node) {
			fprintf (stdout, "\t%d already in tree.\n", *node) ;
		}
		else {
			fprintf(stdout, "\t%d is inserted.\n", key) ;

			jr_AVL_TreeSetNewElement (avl_tree, &key);
		}


		jr_AVL_TreePrint (avl_tree, stdout, print_integer, 4);
		/*
		 * 4 is the maximum width of each printed item
		 */

		fprintf(stderr, "Number to insert? ") ;
	}


	jr_AVL_TreeUndo (avl_tree);

	exit (0);
}
