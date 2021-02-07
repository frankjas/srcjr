#include "project.h"

jr_int jr_AVL_TreeFillSearchPath (avl_tree, path, obj)
	jr_AVL_Tree *	avl_tree;
	void **			path;
	const void *	obj ;
{
	void *			curr_node;
	jr_int			comp_val;
	jr_int			path_index;

	/*
	 * standard binary tree search.  Much like
	 * a binary search of an array
	 */
	curr_node = avl_tree->root_node;

	for (path_index = 0; curr_node; path_index ++) {

		path [path_index]	= curr_node;

		comp_val			= (*avl_tree->cmpfn)(obj, curr_node, avl_tree->cmpfn_arg) ;

		if (comp_val == 0) {
			return (path_index + 1) ;
		}

		if (comp_val  < 0)	curr_node = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);
		else				curr_node = jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
	}
	return (path_index);
}


void *jr_AVL_TreeGetPrevPtrFromPath (avl_tree, path, path_length)
	jr_AVL_Tree *	avl_tree;
	void **			path;
	jr_int			path_length;
{
	void *			curr_node;
	jr_int			path_index;

	path_index = path_length - 1;


	if (jr_AVL_TreeLeftChildPtr (avl_tree, path [path_index]) == 0) {
		/*
		 * no left child, go up to parent
		 * the bottom of the path could be left or right child.
		 * If its the left child, then keep backing out until
		 * you get an ancestor with a left child.
		 * If its the right child, get the rightmost descendant
		 * of the parent's left child, or the parent if there is no left child
		 */
		while (path_index > 0  &&
				jr_AVL_TreeLeftChildPtr (avl_tree, path [path_index - 1])  ==  path [path_index]) {
			/*
			 * while the current level is the left child of the parent
			 */
			path_index --;
		}
		if (path_index == 0) {
			/*
			 * this node is the smallest node in the whole tree
			 */
			return (0);
		}
		/*
		 * path index is a node that has a parent and is a right child
		 */
		return (path [path_index - 1]);
	}

	curr_node = jr_AVL_TreeLeftChildPtr (avl_tree, path [path_length - 1]);

	while (jr_AVL_TreeRightChildPtr (avl_tree, curr_node)) {
		curr_node = jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
	}
	return (curr_node);
}

void *jr_AVL_TreePrevElementPtr (avl_tree, obj)
	jr_AVL_Tree *	avl_tree;
	const void *	obj ;
{
	void *	path [jr_AVL_TREE_MAX_HEIGHT];
	jr_int	path_length;
	void *	prev_node;

	path_length	= jr_AVL_TreeFillSearchPath (avl_tree, path, obj);

	if (path_length == 0) return (0);

	if ((*avl_tree->cmpfn) (obj, path [path_length - 1], avl_tree->cmpfn_arg) > 0) {
		/*
		 * the object is greater than the bottom of the path,
		 * so the bottom is the previous
		 */
		prev_node	= path [path_length - 1];
	}
	else {
		prev_node	= jr_AVL_TreeGetPrevPtrFromPath (avl_tree, path, path_length);
	}

	return (prev_node);
}

void *jr_AVL_TreeGetNextPtrFromPath (avl_tree, path, path_length)
	jr_AVL_Tree *	avl_tree;
	void **			path;
	jr_int			path_length;
{
	void *			curr_node;
	jr_int			path_index;

	path_index = path_length - 1;


	if (jr_AVL_TreeRightChildPtr (avl_tree, path [path_index]) == 0) {
		/*
		 * no right child, go up to parent
		 * the bottom of the path could be left or right child.
		 * If its the right child, then keep backing out until
		 * you get an ancestor with a right child.
		 * If its the left child, get the leftmost descendant
		 * of the parent's right child, or the parent if there is no right child
		 */
		while (path_index > 0  &&
				jr_AVL_TreeRightChildPtr (avl_tree, path [path_index - 1])  ==  path [path_index]) {
			/*
			 * while the current level is the right child of the parent
			 */
			path_index --;
		}
		if (path_index == 0) {
			/*
			 * this node is the greatest node in the whole tree
			 */
			return (0);
		}
		/*
		 * path index is a node that has a parent and is a left child
		 */
		return (path [path_index - 1]);
	}

	curr_node = jr_AVL_TreeRightChildPtr (avl_tree, path [path_length - 1]);

	while (jr_AVL_TreeLeftChildPtr (avl_tree, curr_node)) {
		curr_node = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);
	}
	return (curr_node);
}

void *jr_AVL_TreeNextElementPtr (avl_tree, obj)
	jr_AVL_Tree *	avl_tree;
	const void *	obj ;
{
	void *	path [jr_AVL_TREE_MAX_HEIGHT];
	jr_int	path_length;
	void *	next_node;

	path_length	= jr_AVL_TreeFillSearchPath (avl_tree, path, obj);

	if (path_length == 0) return (0);

	if ((*avl_tree->cmpfn) (obj, path [path_length - 1], avl_tree->cmpfn_arg) < 0) {
		/*
		 * the object is less than the bottom of the path,
		 * so the bottom is the next
		 */
		next_node	= path [path_length - 1];
	}
	else {
		next_node	= jr_AVL_TreeGetNextPtrFromPath (avl_tree, path, path_length);
	}

	return (next_node);
}
