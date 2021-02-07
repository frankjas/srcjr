#define AVL_TRAV_SRC

#include "project.h"

void jr_AVL_TreeTraverseInOrder(avl_tree, printfn, data_arg)
	jr_AVL_Tree *			avl_tree;
	void					(*printfn)() ;
	void *					data_arg;
{
	jr_AVL_TreeTraverseElementInOrder (avl_tree, jr_AVL_TreeRootPtr (avl_tree), printfn, data_arg);
}

void jr_AVL_TreeTraverseElementInOrder(avl_tree, node, printfn, data_arg)
	jr_AVL_Tree *			avl_tree;
	const void *			node ;
	void					(*printfn)() ;
	void *					data_arg;
{
	
	if (node == 0) return ;

	if (jr_AVL_TreeLeftChildPtr (avl_tree, node)) {
		jr_AVL_TreeTraverseElementInOrder (
			avl_tree, jr_AVL_TreeLeftChildPtr (avl_tree, node), printfn, data_arg
		);
	}

	(*printfn)(node, data_arg) ;

	if (jr_AVL_TreeRightChildPtr (avl_tree, node)) {
		jr_AVL_TreeTraverseElementInOrder (
			avl_tree, jr_AVL_TreeRightChildPtr (avl_tree, node), printfn, data_arg
		);
	}
}

