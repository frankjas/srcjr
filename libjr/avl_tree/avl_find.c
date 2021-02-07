#include "project.h"

/*| begin jr_AVL_TreeFindElementPtr_decl |*/
void *jr_AVL_TreeFindElementPtrUsingKey (avl_tree, obj)
	jr_AVL_Tree *	avl_tree;
	const void *	obj ;
{
	jr_int			comp_val;
	const void *	curr_node		= avl_tree->root_node;


	while (curr_node != 0) {

		comp_val = (*avl_tree->cmpfn) (obj, curr_node, avl_tree->cmpfn_arg) ;

		if (comp_val == 0) {
			return (void *) curr_node;
		}

		if (comp_val  < 0) {
			curr_node = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);
		}
		else {
			curr_node = jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
		}
	}
	return 0;
}
/*| end jr_AVL_TreeFindElementPtr_decl |*/

void *jr_AVL_TreeSmallestElementPtr (avl_tree)
	jr_AVL_Tree *	avl_tree;
{
	const void *	curr_node		= avl_tree->root_node;

	while (curr_node  &&  jr_AVL_TreeLeftChildPtr (avl_tree, curr_node)) {
		curr_node = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);
	}
	return (void *) curr_node;
}

void *jr_AVL_TreeGreatestElementPtr (avl_tree)
	jr_AVL_Tree *	avl_tree;
{
	const void *	curr_node		= avl_tree->root_node;

	while (curr_node  &&  jr_AVL_TreeRightChildPtr (avl_tree, curr_node)) {
		curr_node = jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
	}
	return (void *) curr_node;
}
