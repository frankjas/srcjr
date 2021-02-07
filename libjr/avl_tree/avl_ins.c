#include "project.h"


void *jr_AVL_TreeSetNewElementWithSize(avl_tree, key_node, length)
	jr_AVL_Tree *	avl_tree;
	const void *	key_node ;
	jr_int  		length ;
{
	void *			new_node;

	new_node		= jr_AVL_TreeNewElementPtrWithSize (avl_tree, key_node, length);

	if (new_node) {
		memcpy (new_node, key_node, length);
	}

	return new_node;
}


void *jr_AVL_TreeNewElementPtrWithSize(avl_tree, key_node, length)
	jr_AVL_Tree *	avl_tree;
	const void *	key_node ;
	jr_int  		length ;
{
	void *			new_node;
	jr_int			status;

	new_node		= jr_AVL_TreeAllocateElementWithSize (avl_tree, length);

	status			= jr_AVL_TreeInsertElementUsingKey (avl_tree, new_node, key_node);

	if (status == -1) {
		jr_AVL_TreeFreeElement (avl_tree, new_node);
		return 0;
	}

	return new_node;
}


jr_int jr_AVL_TreeInsertElementUsingKey (avl_tree, new_node, key_node)
	jr_AVL_Tree *	avl_tree;
	const void *	new_node;
	const void *	key_node;
{
	void *			path[jr_AVL_TREE_MAX_HEIGHT];
	jr_int			path_index;

	void *			curr_node;
	jr_int			comp_val			= 0;		/* to shutup 'unused' warning */
	jr_int			is_left_child;


	curr_node			= avl_tree->root_node;

	for (path_index = 0;  curr_node;  path_index++) {
		path[path_index] = curr_node;

		comp_val = (*avl_tree->cmpfn) (key_node, curr_node, avl_tree->cmpfn_arg) ;

		if (comp_val == 0) return -1;

		if (comp_val  < 0) {
			curr_node = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);
		}
		else {
			curr_node = jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
		}
	}
	/*
	 * comp_val tells which side to put node on
	 *
	 * path [path_index - 1] contains the parent of new_node
	 */

	if (comp_val < 0) {
		is_left_child		= 1;
	}
	else {
		is_left_child		= 0;
	}

	jr_AVL_TreeAppendToPath  (avl_tree, path, path_index, is_left_child, new_node);

	return 0;
}


void jr_AVL_TreeAppendToPath  (avl_tree, path, path_length, is_left_child, new_node)
	jr_AVL_Tree *			avl_tree;
	void *					path[];
	jr_int					path_length;
	jr_int					is_left_child;
	const void *			new_node;
{
	if (path_length > 0) {
		if (is_left_child) {
			jr_AVL_TreeLeftChildPtr (avl_tree, path [path_length - 1]) = (void *) new_node;
		}
		else {
			jr_AVL_TreeRightChildPtr (avl_tree, path [path_length - 1]) = (void *) new_node;
		}
	}
	else {
		avl_tree->root_node = (void *) new_node;
	}

	jr_AVL_TreeLeftChildPtr		(avl_tree, new_node) = 0 ;
	jr_AVL_TreeRightChildPtr	(avl_tree, new_node) = 0 ;
	jr_AVL_TreeElementHeight	(avl_tree, new_node) = 0 ;


	if (jr_AVL_TreeMaintainsSize (avl_tree)) {
		jr_int			i;

		jr_AVL_TreeSetSubTreeSize		(avl_tree, new_node, 1);

		for (i=0;  i < path_length;  i++) {
			jr_AVL_TreeIncrementElementTreeSize (avl_tree, path[i]);
		}
	}

	jr_AVL_TreeBalancePath (avl_tree, path, path_length);
	/*
	 * Update the height of the nodes by backing up the path.
	 */
}


void jr_AVL_TreeBalancePath (avl_tree, path, path_length)
	jr_AVL_Tree *		avl_tree;
	void **				path;
	jr_int				path_length;
{
	void *				right_child;
	void *				left_child;
	jr_int				left_height, right_height;
	jr_int				max_child_height;
	jr_int				path_index, diff;


	for (path_index = path_length - 1; path_index >= 0; path_index--) {

		right_child		= jr_AVL_TreeRightChildPtr (avl_tree, path[path_index]);
		left_child		= jr_AVL_TreeLeftChildPtr (avl_tree, path[path_index]);

		/*
		 * get the heights of the children
		 */

			if (right_child) {
				right_height = jr_AVL_TreeElementHeight (avl_tree, right_child);
			}
			else {
				right_height = -1;
			}

			if (left_child)  {
				left_height  = jr_AVL_TreeElementHeight (avl_tree, left_child);
			}
			else {
				left_height  = -1;
			}


		if (right_height < left_height) {
			max_child_height	= left_height;
			diff				= left_height - right_height;
		}
		else {
			max_child_height	= right_height;
			diff				= right_height - left_height;
		}

		if (diff == 2) {
			if (path_index - 1 < 0) {
				/*
				 * the current node is the root 
				 * the 0 argument tells jr_AVL_TreeBalanceElement this
				 * is the case
				 */
				jr_AVL_TreeBalanceElement (avl_tree, path[path_index], 0);
			}
			else {
				jr_AVL_TreeBalanceElement (avl_tree, path[path_index], path[path_index - 1]);
			}

			/* we are guaranteed that the curr node is
			 * balanced and its height is no greater than it
			 * was prior to the insertion, so we don't
			 * have to back any further down the path
			 */
			break;
		}

		/*
		 * no balancing needed, but we may have to propogate
		 * some height information back to the parent
		 */

		if (jr_AVL_TreeElementHeight (avl_tree, path[path_index]) == max_child_height + 1) {
			/*
			 * We know that before the insertion this statement
			 * was true.  However after an insertion one
			 * of the children was increased by one.  In this
			 * case it was the "shorter" child so we didn't 
			 * change the height of this node.
			 */
			break;
		}
		/*
		 * Continue in the loop, i.e. look at the parent
		 * since we may end up unbalancing the tree farther back.
		 */
		jr_AVL_TreeElementHeight (avl_tree, path[path_index]) = max_child_height + 1;
	}
}


void jr_AVL_TreeBalanceElement (avl_tree, curr_node, parent_node)
	jr_AVL_Tree *		avl_tree;
	void *				curr_node;
	void *				parent_node;
{
	void *				right_child;
	void *				left_child;
	jr_int				right_height;
	jr_int				left_height;


	right_child	= jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
	left_child	= jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);

	/*
	 * initialize height information
	 */

		if (right_child) {
			right_height		= jr_AVL_TreeElementHeight (avl_tree, right_child);
		}
		else {
			right_height		= -1;
		}

		if (left_child) {
			left_height			= jr_AVL_TreeElementHeight (avl_tree, left_child);
		}
		else {
			left_height			= -1;
		}

	if (right_height > left_height) {
		void *			right_grand_child;
		void *			left_grand_child;
		jr_int			right_grand_height;
		jr_int			left_grand_height;


		right_grand_child	= jr_AVL_TreeRightChildPtr (avl_tree, right_child);
		left_grand_child	= jr_AVL_TreeLeftChildPtr (avl_tree, right_child);

			if (right_grand_child) {
				right_grand_height 		= jr_AVL_TreeElementHeight (avl_tree, right_grand_child);
			}
			else {
				right_grand_height		= -1;
			}

			if (left_grand_child) {
				left_grand_height		= jr_AVL_TreeElementHeight (avl_tree, left_grand_child);
			}
			else {
				left_grand_height		= -1;
			}

		if (right_grand_height > left_grand_height) {
			/*
			 * Single rotation:
			 * right_child becomes curr_node with
			 * curr_node and right_grand as left, right children.
			 * curr_node has left_child and left_grand as left, right children.
			 */

			 /*
			  * make right_child the new curr_node
			  */
			if (parent_node) {
				if (jr_AVL_TreeRightChildPtr (avl_tree, parent_node) == curr_node) {
					jr_AVL_TreeRightChildPtr (avl_tree, parent_node) = right_child;
				}
				else {
					jr_AVL_TreeLeftChildPtr (avl_tree, parent_node) = right_child;
				}
			}
			else {
				avl_tree->root_node = right_child;
			}
			jr_AVL_TreeLeftChildPtr (avl_tree, right_child) = curr_node;
			jr_AVL_TreeRightChildPtr (avl_tree, curr_node) = left_grand_child;

			/*
			 * adjust the heights
			 */
			jr_AVL_TreeElementHeight (avl_tree, curr_node)		= left_height + 1;
			jr_AVL_TreeElementHeight (avl_tree, right_child)	= right_grand_height + 1;

			if (jr_AVL_TreeMaintainsSize (avl_tree)) {
				/* order of update matters */
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, curr_node);
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, right_child);
			}
		}
		else {
			/*
			 * left_grand_height > right_grand_height 
			 * Double rotation (see diagram).
			 * left_grand_child becomes curr_node.
			 * The order of assignments below is important.
			 */
			if (parent_node) {
				if (jr_AVL_TreeRightChildPtr (avl_tree, parent_node) == curr_node) {
					jr_AVL_TreeRightChildPtr (avl_tree, parent_node) = left_grand_child;
				}
				else {
					jr_AVL_TreeLeftChildPtr (avl_tree, parent_node) = left_grand_child;
				}
			}
			else {
				avl_tree->root_node = left_grand_child;
			}

			jr_AVL_TreeRightChildPtr (avl_tree, curr_node)		
				= jr_AVL_TreeLeftChildPtr (avl_tree, left_grand_child);

			jr_AVL_TreeElementHeight (avl_tree, curr_node)
				= left_height + 1;
			
			jr_AVL_TreeLeftChildPtr (avl_tree, right_child)
				= jr_AVL_TreeRightChildPtr (avl_tree, left_grand_child);

			jr_AVL_TreeElementHeight (avl_tree, right_child)
				= right_grand_height + 1;

			jr_AVL_TreeLeftChildPtr (avl_tree, left_grand_child)
				= curr_node;

			jr_AVL_TreeRightChildPtr (avl_tree, left_grand_child)
				= right_child;

			jr_AVL_TreeElementHeight (avl_tree, left_grand_child)
				= jr_AVL_TreeElementHeight (avl_tree, curr_node) + 1;

			if (jr_AVL_TreeMaintainsSize (avl_tree)) {
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, curr_node);
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, right_child);
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, left_grand_child);
			}
		}
	}
	else {
		/* left_height > right_height */
		void *			right_grand_child;
		void *			left_grand_child;
		jr_int			right_grand_height;
		jr_int			left_grand_height;

		left_grand_child	= jr_AVL_TreeLeftChildPtr (avl_tree, left_child);
		right_grand_child	= jr_AVL_TreeRightChildPtr (avl_tree, left_child);

			if (right_grand_child) {
				right_grand_height = jr_AVL_TreeElementHeight (avl_tree, right_grand_child);
			}
			else {
				right_grand_height = -1;
			}

			if (left_grand_child) {
				left_grand_height = jr_AVL_TreeElementHeight (avl_tree, left_grand_child);
			}
			else {
				left_grand_height = -1;
			}

		if (left_grand_height > right_grand_height) {
			/*
			 * Single rotation.
			 * left_child becomes the curr_node with
			 * left_grand_child, currnode as left and right children.
			 * curr_node has right_grand_child and right_child as left and right children.
			 */

			 /*
			  * make left_child the new curr_node
			  */
			if (parent_node) {
				if (jr_AVL_TreeRightChildPtr (avl_tree, parent_node) == curr_node) {
					jr_AVL_TreeRightChildPtr (avl_tree, parent_node) = left_child;
				}
				else {
					jr_AVL_TreeLeftChildPtr (avl_tree, parent_node) = left_child;
				}
			}
			else {
				avl_tree->root_node = left_child;
			}
			jr_AVL_TreeRightChildPtr (avl_tree, left_child) = curr_node;
			jr_AVL_TreeLeftChildPtr (avl_tree, curr_node) = right_grand_child;

			jr_AVL_TreeElementHeight (avl_tree, curr_node) = right_height + 1;
			jr_AVL_TreeElementHeight (avl_tree, left_child) = left_grand_height+1;


			if (jr_AVL_TreeMaintainsSize (avl_tree)) {
				/* order of update matters */
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, curr_node);
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, left_child);
			}
		}
		else {
			/*
			 * right_grand_height > left_grand_height
			 * Double rotation (see diagram).
			 * right_grand_child becomes curr_node.
			 * The order of assignments below is important.
			 */

			if (parent_node) {
				if (jr_AVL_TreeRightChildPtr (avl_tree, parent_node) == curr_node) {
					jr_AVL_TreeRightChildPtr (avl_tree, parent_node)	= right_grand_child;
				}
				else {
					jr_AVL_TreeLeftChildPtr (avl_tree, parent_node)	= right_grand_child;
				}
			}
			else {
				avl_tree->root_node = right_grand_child;
			}

			jr_AVL_TreeLeftChildPtr (avl_tree, curr_node)
				= jr_AVL_TreeRightChildPtr (avl_tree, right_grand_child);

			jr_AVL_TreeElementHeight (avl_tree, curr_node)
				= right_height + 1;
			
			jr_AVL_TreeRightChildPtr (avl_tree, left_child)
				= jr_AVL_TreeLeftChildPtr (avl_tree, right_grand_child);

			jr_AVL_TreeElementHeight (avl_tree, left_child)
				= left_grand_height + 1;

			jr_AVL_TreeRightChildPtr (avl_tree, right_grand_child)
				= curr_node;

			jr_AVL_TreeLeftChildPtr (avl_tree, right_grand_child)
				= left_child;

			jr_AVL_TreeElementHeight (avl_tree, right_grand_child)
				= jr_AVL_TreeElementHeight (avl_tree, curr_node) + 1;

			if (jr_AVL_TreeMaintainsSize (avl_tree)) {
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, curr_node);
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, left_child);
				jr_AVL_TreeElementUpdateTreeSize (avl_tree, right_grand_child);
			}
		}
	}
}

void jr_AVL_TreeElementUpdateTreeSize (avl_tree, node_ptr)
	jr_AVL_Tree *	avl_tree;
	void *			node_ptr;
{
	void *			left_child			= jr_AVL_TreeLeftChildPtr (avl_tree, node_ptr);
	void *			right_child			= jr_AVL_TreeRightChildPtr (avl_tree, node_ptr);
	jr_int			tree_size			= 1;

	if (left_child) {
		tree_size	+= jr_AVL_TreeSubTreeSize (avl_tree, left_child);
	}
	if (right_child) {
		tree_size	+= jr_AVL_TreeSubTreeSize (avl_tree, right_child);
	}

	jr_AVL_TreeSetSubTreeSize (avl_tree, node_ptr, tree_size);
}


