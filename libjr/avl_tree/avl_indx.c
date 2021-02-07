#include "project.h"


void *jr_AVL_TreeFindElementPtrUsingIndex (avl_tree, index_value)
	jr_AVL_Tree *	avl_tree;
	jr_int			index_value;
{
	void *			curr_node;
	jr_int			curr_index;

	void *			left_child;
	jr_int			left_tree_size;


	assert (jr_AVL_TreeMaintainsSize (avl_tree));

	curr_node	= avl_tree->root_node;
	curr_index	= 0;

	while (curr_node) {

		left_child = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);

		if (left_child) {
			left_tree_size	= jr_AVL_TreeSubTreeSize (avl_tree, left_child);
		}
		else {
			left_tree_size	= 0;
		}

		if (index_value == curr_index + left_tree_size) {
			return curr_node;
		}

		if (index_value  <  curr_index + left_tree_size) {
			curr_node	= left_child;
		}
		else {
			curr_node	= jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
			curr_index	+= left_tree_size + 1;
		}

	}
	/*
	 * Bigger than anything in the tree.  Could have checked the root node
	 * above for this.
	 */

	return 0;
}


void jr_AVL_TreeInsertElementUsingIndex (avl_tree, new_node, index_value)
	jr_AVL_Tree *	avl_tree;
	const void *	new_node;
	jr_int			index_value;
{
	void *			path[jr_AVL_TREE_MAX_HEIGHT];
	jr_int			path_index;

	void *			curr_node;
	jr_int			curr_index;
	void *			left_child;
	jr_int			left_tree_size;


	assert (jr_AVL_TreeMaintainsSize (avl_tree));
	assert (index_value <= jr_AVL_TreeSize (avl_tree));


	/*
	 * Same as find() above, but keeps track of the path.
	 */
	
	curr_node	= avl_tree->root_node;
	curr_index	= 0;
	path_index	= 0;

	while (curr_node) {

		path[path_index] = curr_node;
		path_index++;

		left_child = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);

		if (left_child) {
			left_tree_size	= jr_AVL_TreeSubTreeSize (avl_tree, left_child);
		}
		else {
			left_tree_size	= 0;
		}

		if (index_value == curr_index + left_tree_size) {
			break;
		}

		if (index_value  <  curr_index + left_tree_size) {
			curr_node	= left_child;
		}
		else {
			curr_node	= jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
			curr_index	+= left_tree_size + 1;
		}
	}


	if (curr_node) {
		curr_node = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);

		if (curr_node) {
			for (;  curr_node;  curr_node = jr_AVL_TreeRightChildPtr (avl_tree, curr_node)) {
				path [path_index] = curr_node;
				path_index ++;
			}
			jr_AVL_TreeAppendToPath  (avl_tree, path, path_index, 0, new_node);
		}
		else {
			jr_AVL_TreeAppendToPath  (avl_tree, path, path_index, 1, new_node);
		}
	}
	else {
		jr_AVL_TreeAppendToPath  (avl_tree, path, path_index, 0, new_node);
	}
}


void *jr_AVL_TreeExtractElementUsingIndex (avl_tree, index_value)
	jr_AVL_Tree *	avl_tree;
	jr_int			index_value;
{
	void *			path[jr_AVL_TREE_MAX_HEIGHT];
	jr_int			path_index;

	void *			curr_node;
	jr_int			curr_index;
	void *			left_child;
	jr_int			left_tree_size;


	assert (jr_AVL_TreeMaintainsSize (avl_tree));
	assert (index_value < jr_AVL_TreeSize (avl_tree));


	/*
	 * Same as find() above, but keeps track of the path.
	 */
	
	curr_node	= avl_tree->root_node;
	curr_index	= 0;
	path_index	= 0;

	while (curr_node) {

		path[path_index] = curr_node;
		path_index++;

		left_child = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);

		if (left_child) {
			left_tree_size	= jr_AVL_TreeSubTreeSize (avl_tree, left_child);
		}
		else {
			left_tree_size	= 0;
		}

		if (index_value == curr_index + left_tree_size) {
			break;
		}

		if (index_value  <  curr_index + left_tree_size) {
			curr_node	= left_child;
		}
		else {
			curr_node	= jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
			curr_index	+= left_tree_size + 1;
		}
	}

	curr_node = jr_AVL_TreeExtractPathEnd  (avl_tree, path, path_index);

	return curr_node;
}


void jr_AVL_TreeAppendLastIndexedElement (avl_tree, new_node)
	jr_AVL_Tree *			avl_tree;
	const void *			new_node;
{
	void *					path[jr_AVL_TREE_MAX_HEIGHT];
	jr_int					path_index;

	void *					curr_node;


	curr_node	= avl_tree->root_node;

	for (path_index=0; curr_node; path_index++) {

		path[path_index]	= curr_node;
		curr_node			= jr_AVL_TreeRightChildPtr (avl_tree, curr_node);
	}

	jr_AVL_TreeAppendToPath  (avl_tree, path, path_index, 0, new_node);
}


jr_int jr_AVL_TreeIndexedCmp (void_ptr1, void_ptr2)
	const void *			void_ptr1;
	const void *			void_ptr2;
{
	const jr_int *			int_ptr1		= void_ptr1;
	const jr_int *			int_ptr2		= void_ptr2;

	return *int_ptr1 - *int_ptr2;
}



void jr_AVL_TreeSetMaintainsSize (avl_tree, value)
	jr_AVL_Tree *	avl_tree;
	jr_int			value;
{
	if (	avl_tree->allocates_elements
		&&  avl_tree->maintains_size != (unsigned) (value != 0)) {

		fprintf (stderr, "\n\njr_AVL_TreeSetMaintainsSize(): called on non-empty tree\n");
		jr_coredump ();
	}
	avl_tree->maintains_size	= value != 0;
}

void *jr_AVL_TreeSetNewIndexedElementWithSize (avl_tree, index, new_element, length)
	jr_AVL_Tree *			avl_tree;
	jr_int					index;
	const void *			new_element;
	jr_int  				length ;
{
	void *					new_node;

	new_node		= jr_AVL_TreeNewIndexedElementPtrWithSize (avl_tree, index, length);

	if (new_node) {
		memcpy (new_node, new_element, length);
	}

	return new_node;
}


void *jr_AVL_TreeNewIndexedElementPtrWithSize (avl_tree, index, length)
	jr_AVL_Tree *			avl_tree;
	jr_int					index;
	jr_int  				length ;
{
	void *					new_node;

	new_node		= jr_AVL_TreeAllocateElementWithSize (avl_tree, length);

	jr_AVL_TreeInsertElementUsingIndex (avl_tree, new_node, index);

	return new_node;
}


void *jr_AVL_TreeSetNewLastIndexedElementWithSize (avl_tree, new_element, length)
	jr_AVL_Tree *			avl_tree;
	const void *			new_element;
	jr_int  				length ;
{
	void *					new_node;

	new_node		= jr_AVL_TreeNewLastIndexedElementPtrWithSize (avl_tree, length);

	if (new_node) {
		memcpy (new_node, new_element, length);
	}

	return new_node;
}


void *jr_AVL_TreeNewLastIndexedElementPtrWithSize (avl_tree, length)
	jr_AVL_Tree *			avl_tree;
	jr_int  				length ;
{
	void *					new_node;

	new_node		= jr_AVL_TreeAllocateElementWithSize (avl_tree, length);

	jr_AVL_TreeAppendLastIndexedElement (avl_tree, new_node);

	return new_node;
}


jr_int jr_AVL_TreeDeleteIndexedElement (avl_tree, index)
	jr_AVL_Tree *			avl_tree;
	jr_int					index;
{
	void *					deleted_node;

	deleted_node	= jr_AVL_TreeExtractElementUsingIndex (avl_tree, index);

	if (deleted_node == 0) {
		return -1;
	}

	if (jr_AVL_TreeAllocatesElements (avl_tree)) {
		jr_AVL_TreeFreeElement (avl_tree, deleted_node);
	}

	return 0;
}


jr_int jr_AVL_TreeCalculateElementIndex (avl_tree, existing_node)
	jr_AVL_Tree *	avl_tree;
	const void *	existing_node;
{
	const void *	curr_node;
	jr_int			curr_index;


	jr_AVL_TreeForEachElementIndex (avl_tree, curr_index) {
		curr_node	= jr_AVL_TreeFindIndexedElementPtr (avl_tree, curr_index);

		if (existing_node  ==  curr_node) {
			return curr_index;
		}
	}
	return -1;
}
