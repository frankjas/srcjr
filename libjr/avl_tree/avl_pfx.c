#include "project.h"

void jr_AVL_TreeSetContainers (
	jr_AVL_Tree *			avl_tree, ...)
{
	va_list					arg_list;

	va_start (arg_list, avl_tree);

	jr_AVL_TreeSetContainersFromVA_List (avl_tree, arg_list);

	va_end (arg_list);
}


void jr_AVL_TreeSetContainersFromVA_List (avl_tree, arg_list)
	jr_AVL_Tree *			avl_tree;
	va_list					arg_list;
{
	jr_PrefixInfoType *		prefix_array;
	jr_int					prefix_size;
	jr_int					needs_transform_fn			= 0;


	prefix_array			= jr_PrefixArrayCreateFromVA_List	(arg_list, needs_transform_fn);
	prefix_size				= jr_PrefixArrayInitPrefixes		(prefix_array);

	jr_AVL_TreeSetPrefixSize (avl_tree, prefix_size);

	jr_PrefixArrayDestroy (prefix_array);
}


void jr_AVL_TreeTransformPrefix (avl_tree, el_ptr, magnitude, which_way, all_elements)
	jr_AVL_Tree *		avl_tree;
	void *				el_ptr;
	unsigned jr_int		magnitude;
	jr_int				which_way;
	jr_int				all_elements;
{
	char *				left_ptr;
	char *				right_ptr;
	void *				old_el_ptr;
	
	if (jr_AVL_TreeElementHeight (avl_tree, el_ptr) < 0) {
		/*
		 * Means it has been removed from the tree
		 */
		return;
	}

	old_el_ptr			= jr_UnTransformAListPtr (el_ptr, magnitude, which_way);

	if (old_el_ptr  ==  jr_AVL_TreeRootPtr (avl_tree)) {
		jr_AVL_TreeRootPtr (avl_tree)	= el_ptr;
	}

	if (all_elements) {
		left_ptr			= jr_AVL_TreeLeftChildPtr (avl_tree, el_ptr);
		right_ptr			= jr_AVL_TreeRightChildPtr (avl_tree, el_ptr);

		if (left_ptr) {
			left_ptr		= jr_TransformAListPtr (left_ptr, magnitude, which_way);
			jr_AVL_TreeLeftChildPtr (avl_tree, el_ptr)			= left_ptr;
		}

		if (right_ptr) {
			right_ptr		= jr_TransformAListPtr (right_ptr, magnitude, which_way);
			jr_AVL_TreeRightChildPtr (avl_tree, el_ptr)			= right_ptr;
		}
	}
	else {
		/*
		 * need to find parent of this element (already took care of root node case)
		 */
		void *	path [jr_AVL_TREE_MAX_HEIGHT];
		jr_int	path_length;
		void *	prev_node;

		path_length	= jr_AVL_TreeFillSearchPath (avl_tree, path, el_ptr);

		if (path_length > 1) {
			if (old_el_ptr  ==  path[path_length - 1]) {
				prev_node	= path [path_length - 2];

				if (jr_AVL_TreeLeftChildPtr (avl_tree, prev_node)  ==  old_el_ptr) {
					jr_AVL_TreeLeftChildPtr (avl_tree, prev_node)  = el_ptr;
				}
				else if (jr_AVL_TreeRightChildPtr (avl_tree, prev_node)  ==  old_el_ptr) {
					jr_AVL_TreeRightChildPtr (avl_tree, prev_node)  = el_ptr;
				}
			}
		}
	}
}

