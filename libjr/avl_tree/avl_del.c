#include "project.h"


jr_int jr_AVL_TreeDeleteElement (avl_tree, key_node)
	jr_AVL_Tree *	avl_tree;
	void *			key_node;
{
	void *			deleted_node;

	deleted_node	= jr_AVL_TreeExtractElementUsingKey (avl_tree, key_node);

	if (deleted_node == 0) {
		return -1;
	}

	if (jr_AVL_TreeAllocatesElements (avl_tree)) {
		jr_AVL_TreeFreeElement (avl_tree, deleted_node);
	}

	return 0;
}


void *jr_AVL_TreeExtractElementUsingKey (avl_tree, key_node)
	jr_AVL_Tree *	avl_tree;
	const void *	key_node;
{
	void *			path[jr_AVL_TREE_MAX_HEIGHT];
	jr_int			path_length;

	void *			curr_node;
	jr_int			comp_val;


	curr_node		= avl_tree->root_node;
	path_length		= 0;

	while (curr_node) {

		path[path_length] = curr_node;
		path_length++;

		comp_val = (*avl_tree->cmpfn)(key_node, curr_node, avl_tree->cmpfn_arg) ;

		if (comp_val == 0) {
			break;
		}
		else if (comp_val  < 0)	{
			curr_node	= jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);
		}
		else {
			curr_node	= jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
		}
	}

	if (curr_node == 0) {
		return 0;
	}

	curr_node	= jr_AVL_TreeExtractPathEnd (avl_tree, path, path_length);

	return curr_node;
}


void *jr_AVL_TreeExtractPathEnd (avl_tree, path, path_length)
	jr_AVL_Tree *			avl_tree;
	void *					path[];
	jr_int					path_length;
{
	void *					deleted_node;
	void *					deleted_parent_node;
	void *					replacement_node;
	void *					replacement_parent_node;
	void *					right_child;
	void *					left_child;

	jr_int					deleted_node_index;
	jr_int					go_left;
	jr_int					go_right;


	if (path_length == 0) {
		return 0;
	}

	deleted_node				= path [path_length - 1];
	deleted_node_index			= path_length - 1;

	if (path_length > 1) {
		deleted_parent_node		= path [path_length - 2];
	}
	else {
		deleted_parent_node		= 0;
	}


	/*
	 * find a replacement node, if the deleted node has children
	 * it must be the greatest node in the left sub-tree or
	 * the smallest node in the right sub-tree.  Take from the
	 * highest sub-tree if the deleted node has both children.
	 */
	left_child		= jr_AVL_TreeLeftChildPtr (avl_tree, deleted_node);
	right_child		= jr_AVL_TreeRightChildPtr (avl_tree, deleted_node);

	go_left			= 0;
	go_right		= 0;

	if (left_child  &&  right_child) {
		/*
		 * choose replacement from highest sub-tree
		 */
		if (	jr_AVL_TreeElementHeight (avl_tree, left_child)
			>	jr_AVL_TreeElementHeight (avl_tree, right_child)) {
			go_left		= 1;
		}
		else {
			go_right	= 1;
		}
	}
	else if (left_child) {
		go_left			= 1;
	}
	else if (right_child) {
		go_right		= 1;
	}
		
	replacement_node			= 0;
	replacement_parent_node		= 0;

	if (go_left) {

		for (	
			replacement_node = left_child;
			replacement_node;
			replacement_node = jr_AVL_TreeRightChildPtr (avl_tree, replacement_node),
				path_length++
			) {

			path [path_length] = replacement_node;
		}
		replacement_node = path [path_length - 1];
	}
	else if (go_right) {

		for (	
			replacement_node = right_child;
			replacement_node;
			replacement_node = jr_AVL_TreeLeftChildPtr (avl_tree, replacement_node),
				path_length ++
			) {

			path [path_length] = replacement_node;
		}
		replacement_node = path [path_length - 1];
	}

	if (replacement_node) {
		/*
		 * The deleted_node has descendents, replace it with the replacement node.
		 * path containes the deleted node and replacement, so path_length - 2 is >= 0
		 * path [path_length - 2] contains the parent of the replacement node.
		 */
		replacement_parent_node	= path [path_length - 2];

		/*
		 * delete the replacement node by changing its parent's child pointer
		 */
		if (replacement_parent_node == deleted_node) {
			if (go_left) {
				/*
				 * just went left, replacement has no right child
				 * take this sub-tree and add it to the right child of the replacement's parent
				 */
				jr_AVL_TreeLeftChildPtr (avl_tree, replacement_parent_node)
				= jr_AVL_TreeLeftChildPtr (avl_tree, replacement_node);
			}
			else if (go_right) {
				/*
				 * must mean we went right, because we went until the left child was 0
				 * take this sub-tree and add it to the left child of the replacement's parent
				 */
				jr_AVL_TreeRightChildPtr (avl_tree, replacement_parent_node)
				= jr_AVL_TreeRightChildPtr (avl_tree, replacement_node);
			}
		}
		else if (go_left) {
			/*
			 * must we went left then right as far as possible ==> replacement is right child
			 * take this sub-tree and add it to the right child of the replacement's parent
			 */
			jr_AVL_TreeRightChildPtr (avl_tree, replacement_parent_node)
			= jr_AVL_TreeLeftChildPtr (avl_tree, replacement_node);
		}
		else if (go_right) {
			/*
			 * must we went right then left as far as possible ==> replacement is left child
			 * must mean we went right, because we went until the left child was 0
			 * take this sub-tree and add it to the left child of the replacement's parent
			 */
			jr_AVL_TreeLeftChildPtr (avl_tree, replacement_parent_node)
			= jr_AVL_TreeRightChildPtr (avl_tree, replacement_node);
		}

		/*
		 * now the replacement node is ready to be switched with the deleted node
		 */
		jr_AVL_TreeLeftChildPtr (avl_tree, replacement_node)	
			= jr_AVL_TreeLeftChildPtr (avl_tree, deleted_node);

		jr_AVL_TreeRightChildPtr (avl_tree, replacement_node)	
			= jr_AVL_TreeRightChildPtr (avl_tree, deleted_node);

		jr_AVL_TreeElementHeight (avl_tree, replacement_node)	
			= jr_AVL_TreeElementHeight (avl_tree, deleted_node);

		if (jr_AVL_TreeMaintainsSize (avl_tree)) {
			jr_AVL_TreeSetSubTreeSize (
				avl_tree, replacement_node, jr_AVL_TreeSubTreeSize (avl_tree, deleted_node)
			);
		}
	}

	if (deleted_parent_node) {
		/*
		 * has parent, replacement node is either 0, or the correct replacement
		 */
		if (jr_AVL_TreeLeftChildPtr (avl_tree, deleted_parent_node)  ==  deleted_node) {
			jr_AVL_TreeLeftChildPtr (avl_tree, deleted_parent_node) = (void *) replacement_node;
		}
		if (jr_AVL_TreeRightChildPtr (avl_tree, deleted_parent_node)  ==  deleted_node) {
			jr_AVL_TreeRightChildPtr (avl_tree, deleted_parent_node) = (void *) replacement_node;
		}
	}
	else {
		avl_tree->root_node = replacement_node;
	}

	path [deleted_node_index] = replacement_node;


	/*
	 * if there was a replacement, the path goes to the replacement node.
	 * with no replacement, it goes to the deleted_node
	 *
	 * If there was a replacement, we want to start balancing
	 * with the parent of the replacement node (path_length - 1 is the path length).
	 * If the deleted node was the parent of the replacement
	 * we want to start balancing with the deleted node (deleted_node_index + 1
	 * is the path length, but in this case that equals path_length - 1).
	 *
	 * If there was no replacement node, then we want to start balancing
	 * with the parent of the deleted node.
	 */

	if (jr_AVL_TreeMaintainsSize (avl_tree)) {
		jr_int			i;

		for (i=0;  i < path_length - 1;  i++) {
			jr_AVL_TreeDecrementElementTreeSize (avl_tree, path[i]);
		}
	}

	jr_AVL_TreeBalancePath (avl_tree, path, path_length - 1);

	jr_AVL_TreeMakeNullElement (avl_tree, deleted_node);
	/*
	 * so prefix transformations on this element don't affect anything
	 */

	return (void *) deleted_node;
}


void jr_AVL_TreeMakeNullElement (avl_tree, el_ptr)
	jr_AVL_Tree *		avl_tree;
	void *				el_ptr;
{
	jr_AVL_TreeLeftChildPtr (avl_tree, el_ptr)		= 0;
	jr_AVL_TreeRightChildPtr (avl_tree, el_ptr)	= 0;
	jr_AVL_TreeElementHeight (avl_tree, el_ptr)	= -1;
}

