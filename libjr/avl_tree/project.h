#ifndef __project_h___
#define __project_h___

#include "ezport.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "jr/avl_tree.h"
#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/alist.h"
#include "jr/prefix.h"

#define has_jr_avl_tree_with_prefix
		/*
		 * These ifdefs enable this source code to be used as
		 * class examples.  Creating separate copies, like is
		 * done for the other data structures, creates more of
		 * an update problem, since the avl tree code is more
		 * complex.
		 */

extern void				jr_AVL_TreeTraverseFree		PROTO ((
							jr_AVL_Tree *			avl_tree,
							void *					node
						));

extern void				jr_AVL_TreeAppendToPath		PROTO ((
							jr_AVL_Tree *			avl_tree,
							void *					path[],
							jr_int					path_length,
							jr_int					is_left_child,
							const void *			new_node
						));

extern void *			jr_AVL_TreeExtractPathEnd	PROTO ((
							jr_AVL_Tree *			avl_tree,
							void *					path[],
							jr_int					path_length
						));

extern void				jr_AVL_TreeBalancePath		PROTO ((
							jr_AVL_Tree *			avl_tree,
							void **					path,
							jr_int					path_length
						));

extern void				jr_AVL_TreeBalanceElement  PROTO ((
							jr_AVL_Tree *		avl_tree, 
							void *				curr_node,
							void *				parent_node
						));


extern jr_int			jr_AVL_TreeFillSearchPath		PROTO ((
								jr_AVL_Tree *	avl_tree,
								void **			path,
								const void *	obj
						));

extern void *			jr_AVL_TreeGetPrevPtrFromPath		PROTO ((
								jr_AVL_Tree *	avl_tree,
								void **			path,
								jr_int			path_length
						));

extern void *			jr_AVL_TreeGetNextPtrFromPath		PROTO ((
								jr_AVL_Tree *	avl_tree,
								void **			path,
								jr_int			path_length
						));


extern void **			jr_AVL_TreeToElementArray				PROTO ((
								jr_AVL_Tree *		avl_tree
						));


extern void				jr_AVL_TreeTraverseForElementArray		PROTO ((
								jr_AVL_Tree *	avl_tree,
								const void *	node,
								void **			node_array,
								jr_int			array_index
						));

extern void				jr_AVL_TreeElementUpdateTreeSize		PROTO ((
							jr_AVL_Tree *	avl_tree,
							void *			node_ptr
						));

#define					jr_AVL_TreeIncrementElementTreeSize(avl_tree, node_ptr)		\
						(jr_AVL_TreeSubTreeSize (avl_tree, node_ptr) ++)

#define					jr_AVL_TreeDecrementElementTreeSize(avl_tree, node_ptr)		\
						(jr_AVL_TreeSubTreeSize (avl_tree, node_ptr) --)


#endif
