#include "project.h"

jr_AVL_Tree *jr_AVL_TreeCreate (element_size, cmpfn)
	jr_int			element_size;
	jr_int			(*cmpfn)() ;
{
	jr_AVL_Tree *	avl_tree;

	avl_tree = jr_malloc (sizeof(jr_AVL_Tree));

	jr_AVL_TreeInit (avl_tree, element_size, cmpfn);

	return (avl_tree);
}
	
void jr_AVL_TreeInit (avl_tree, element_size, cmpfn)
	jr_AVL_Tree *					avl_tree;
	jr_int							element_size;
	jr_int							(*cmpfn)() ;
{
	avl_tree->element_size			= element_size;
	avl_tree->cmpfn					= cmpfn;
	avl_tree->cmpfn_arg				= 0;

	avl_tree->root_node 			= 0;

#	ifdef has_jr_avl_tree_with_prefix
		avl_tree->prefix_size			= 0;
		avl_tree->allocates_elements	= 0;
		{
			static jr_int		added_prefix_info		= 0;

			if (!added_prefix_info) {
				added_prefix_info		= 1;

				jr_PrefixAddContainerType (
					"jr_AVL_Tree",
					jr_AVL_TreeElementPrefixSize,
					jr_AVL_TreeTransformPrefix,
					jr_AVL_TreeSetPrefixSize,
					0
				);
			}
		}

		if (cmpfn == jr_AVL_TreeIndexedCmp) {
			avl_tree->maintains_size	= 1;
		}
#	endif
}

void jr_AVL_TreeDestroy (avl_tree)
	jr_AVL_Tree *	avl_tree;
{
	jr_AVL_TreeUndo (avl_tree);
	jr_free (avl_tree);
}


/*| begin jr_AVL_TreeUndo_decl |*/
void jr_AVL_TreeUndo (avl_tree)
	jr_AVL_Tree *	avl_tree;
{
	jr_AVL_TreeEmpty (avl_tree);
}

void jr_AVL_TreeEmpty (avl_tree)
	jr_AVL_Tree *	avl_tree;
{
	if (avl_tree->root_node) {
		jr_AVL_TreeTraverseFree (avl_tree, avl_tree->root_node);
	}
	avl_tree->root_node = 0;
}

void jr_AVL_TreeTraverseFree (avl_tree, curr_node)
	jr_AVL_Tree *	avl_tree;
	void *			curr_node;
{
	void *			right_child ;
	void *			left_child ;

/*| end jr_AVL_TreeUndo_decl |*/
	if (! jr_AVL_TreeAllocatesElements (avl_tree)) {
		return;
	}
/*| begin jr_AVL_TreeUndo_decl |*/

	left_child = jr_AVL_TreeLeftChildPtr (avl_tree, curr_node);

	if (left_child) {
		jr_AVL_TreeTraverseFree (avl_tree, left_child);
	}

	right_child = jr_AVL_TreeRightChildPtr (avl_tree, curr_node);

	if (right_child) {
		jr_AVL_TreeTraverseFree (avl_tree, right_child);
	}

	jr_AVL_TreeFreeElement (avl_tree, curr_node);
}

/*| end jr_AVL_TreeUndo_decl |*/

#ifdef has_jr_avl_tree_with_prefix
jr_int jr_AVL_TreeElementPrefixSize (avl_tree)
	jr_AVL_Tree *		avl_tree;
{
	jr_int				element_prefix_size		= sizeof (jr_AVL_TreeElementStruct);

	if (avl_tree  &&  avl_tree->maintains_size) {
		element_prefix_size	+= sizeof (jr_AVL_TreeSizeStruct);
	}

	return element_prefix_size;
}

void jr_AVL_TreeSetPrefixSize (avl_tree, prefix_size)
	jr_AVL_Tree *		avl_tree;
	jr_int				prefix_size;
{
	assert (jr_AVL_TreeIsEmpty (avl_tree));

	avl_tree->prefix_size	= prefix_size;
}
#endif


void *jr_AVL_TreeAllocateElementWithSize (avl_tree, length)
	jr_AVL_Tree *			avl_tree;
	jr_int					length ;
{
	char *					new_node;
	jr_int					element_prefix_size			= 0;


#ifdef has_jr_avl_tree_with_prefix
	avl_tree->allocates_elements		= 1;

	element_prefix_size		+= jr_AVL_TreePrefixSize (avl_tree);

	if (avl_tree->maintains_size) {
		element_prefix_size	+= sizeof (jr_AVL_TreeSizeStruct);
	}
#endif

	element_prefix_size		+= sizeof (jr_AVL_TreeElementStruct);

	new_node = jr_malloc (element_prefix_size + length) ;

	new_node += element_prefix_size;

	return new_node;
}


void jr_AVL_TreeFreeElement (avl_tree, node_ptr)
	jr_AVL_Tree *			avl_tree;
	void *					node_ptr;
{
	jr_int					element_prefix_size			= 0;

#ifdef has_jr_avl_tree_with_prefix

	element_prefix_size		+= jr_AVL_TreePrefixSize (avl_tree);

	if (avl_tree->maintains_size) {
		element_prefix_size	+= sizeof (jr_AVL_TreeSizeStruct);
	}
#endif

	element_prefix_size		+= sizeof (jr_AVL_TreeElementStruct);

	jr_free ((char *) (node_ptr) - element_prefix_size);
}



/*
 * For use within the debugger
 */
static jr_AVL_TreeElementStruct *_jr_AVL_TreeElementPre (avl_tree, node_ptr)
	jr_AVL_Tree *	avl_tree;
	void *			node_ptr;
{
	if (0) {
		_jr_AVL_TreeElementPre (avl_tree);
	}
	return jr_AVL_ElementInfoPtrWithPrefix (node_ptr, jr_AVL_TreePrefixSize (avl_tree));
}

static void *_jr_AVL_TreeLeftChildPtr (avl_tree, node_ptr)
	jr_AVL_Tree *	avl_tree;
	void *			node_ptr;
{
	if (0) {
		_jr_AVL_TreeLeftChildPtr (avl_tree, node_ptr);
	}
	return jr_AVL_TreeLeftChildPtr (avl_tree, node_ptr);
}

static void *_jr_AVL_TreeRightChildPtr (avl_tree, node_ptr)
	jr_AVL_Tree *	avl_tree;
	void *			node_ptr;
{
	if (0) {
		_jr_AVL_TreeRightChildPtr (avl_tree, node_ptr);
	}
	return jr_AVL_TreeRightChildPtr (avl_tree, node_ptr);
}


static jr_int _jr_AVL_TreeSubTreeSize (avl_tree, node_ptr)
	jr_AVL_Tree *	avl_tree;
	void *			node_ptr;
{
	if (0) {
		_jr_AVL_TreeSubTreeSize (avl_tree, node_ptr);
	}
	return jr_AVL_TreeSubTreeSize (avl_tree, node_ptr);
}

