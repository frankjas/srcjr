#include "project.h"


void jr_AVL_TreePrint (avl_tree, wfp, print_node_fn, print_width)
	jr_AVL_Tree *		avl_tree;
	FILE *				wfp;
	void				(*print_node_fn) ();
	jr_int				print_width;
{
	void **				node_array;
	jr_int				max_height;
	jr_int				max_num_leaves;
	jr_int				leaf_index, array_index;
	jr_int				curr_height;
	jr_int				remainder;


	if (jr_AVL_TreeIsEmpty (avl_tree)) {
		return;
	}

	/*
	 * height of 0 is root with no children
	 * the width of the bottom is 2^(max_height)
	 */
	max_height		= jr_AVL_TreeElementHeight (avl_tree, jr_AVL_TreeRootPtr (avl_tree));

	max_num_leaves	= jr_get2power (max_height);

	/*
	 * load up the node array
	 */
	node_array = jr_AVL_TreeToElementArray (avl_tree);


	/*
	 * each leaf gets a row
	 */
	for (leaf_index = 0;  leaf_index < max_num_leaves;  leaf_index++) {

		/*
		 * for each level, decide whether to print that level
		 */
		for (curr_height = 0;  curr_height <= max_height;  curr_height++) {
			
			if (curr_height > 0) {
				/*
				 * if you draw a tree on its side
				 * you see why this remainder when moding by
				 * get2power (curr_height) gives you the correct
				 * line to print
				 0
				 1 x
				 2   x
				 3 x
				 4     x
				 5 x
				 6   x
				 7 x
				 *
				 */
				remainder = jr_get2power (curr_height - 1);
			}
			else {
				remainder = 0;
			}

			array_index = jr_get2power (max_height - curr_height);
			array_index += leaf_index / jr_get2power (curr_height);
			array_index -= 1;
			/*
			 * flip curr_height so it is 0 for the root, 1 for the next
			 * and max_height for the leaves, that gives the correct
			 * starting index, 1 or 2, or three (if you index from 1).
			 * Add which sibling of that level, gotten by the div,
			 * subtract 1 since the array is indexed from 0.
			 *
			 *                   level
			 * 0 1 1 2 2 2 2 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4
			 * 1 2 3 4 5 6 7 8 9 ....
			 *                   index
			 */

			if (	leaf_index % jr_get2power (curr_height) == (unsigned) remainder
				&&  node_array [array_index] != 0) {

				/*
				 * if there is an item in the path, print it out, otherwise print a blank
				 */
				print_node_fn (node_array [array_index], wfp);
				putchar (' ');

			}
			else {
				jr_int i;

				for (i = 0;  i < print_width;  i++) {
					putchar (' ');
				}
				putchar (' ');
			}
		}
		putchar ('\n');
	}
	jr_free (node_array);
}

void **jr_AVL_TreeToElementArray (avl_tree)
	jr_AVL_Tree *		avl_tree;
{
	void **				node_array;
	jr_int				max_height, max_num_leaves;

	max_height		= jr_AVL_TreeElementHeight (avl_tree, jr_AVL_TreeRootPtr (avl_tree));
	max_num_leaves	= jr_get2power (max_height);

	node_array		= jr_malloc (2 * max_num_leaves * sizeof (void *));
	memset (node_array, 0, 2 * max_num_leaves * sizeof (void *));

	jr_AVL_TreeTraverseForElementArray (avl_tree, jr_AVL_TreeRootPtr (avl_tree), node_array, 0);


	return (node_array);
}


void jr_AVL_TreeTraverseForElementArray (avl_tree, node, node_array, array_index)
	jr_AVL_Tree *	avl_tree;
	const void *	node ;
	void **			node_array;
	jr_int			array_index;
{
	if (! node) return ;

	node_array [array_index] = (void *) node;

	if (jr_AVL_TreeLeftChildPtr (avl_tree, node)) {
		jr_AVL_TreeTraverseForElementArray(
			avl_tree, jr_AVL_TreeLeftChildPtr (avl_tree, node), node_array, 2 * array_index + 1
		) ;
	}

	if (jr_AVL_TreeRightChildPtr (avl_tree, node)) {
		jr_AVL_TreeTraverseForElementArray(
			avl_tree, jr_AVL_TreeRightChildPtr (avl_tree, node), node_array, 2 * array_index + 2
		) ;
	}
}


/*
 * for use in the debugger
 */
void jr_AVL_TreePrintElement (avl_tree, node)
	jr_AVL_Tree *		avl_tree;
	const void *		node;
{
	fprintf (stderr, "node %p: left %p, right %p, height %d\n",
		node, 
		jr_AVL_TreeLeftChildPtr (avl_tree, node),
		jr_AVL_TreeRightChildPtr (avl_tree, node),
		jr_AVL_TreeElementHeight (avl_tree, node)
	);
}
