#include "ezport.h"

#include <string.h>

#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/ansi_c/exprnode.h"


jr_C_ExprNodeType *	jr_C_ExprNodeCreate (node_type, child0, child1, child2)
	jr_int					node_type;
	jr_C_ExprNodeType *		child0;
	jr_C_ExprNodeType *		child1;
	jr_C_ExprNodeType *		child2;
{
	jr_C_ExprNodeType *		c_expr_node;


	c_expr_node		= jr_malloc (sizeof (*c_expr_node));

	memset (c_expr_node, 0, sizeof (*c_expr_node));

	c_expr_node->node_type		= node_type;

	c_expr_node->children[0]	= child0;
	c_expr_node->children[1]	= child1;
	c_expr_node->children[2]	= child2;

	jr_C_ExprNodeSetVoidValue (c_expr_node);

	return (c_expr_node);
}


void jr_C_ExprNodeDestroy (c_expr_node)
	jr_C_ExprNodeType *	c_expr_node;
{
	jr_C_ExprNodeTraverse (c_expr_node, 0, 0, jr_C_ExprNodeTraversalDestroy, 0, 0);
	/*
	 * post order free, no error can be generated
	 */
}

jr_C_ExprNodeType *	jr_C_ExprNodeListCreate (node_type, child0, child1, child2)
	jr_int					node_type;
	jr_C_ExprNodeType *		child0;
	jr_C_ExprNodeType *		child1;
	jr_C_ExprNodeType *		child2;
{
	jr_C_ExprNodeType *		new_list_node;
	jr_C_ExprNodeType *		c_expr_node;

	c_expr_node	= jr_C_ExprNodeCreate( node_type, child0, child1, child2);

	new_list_node =  jr_C_ExprNodeListCreateWithNode( c_expr_node);

	return new_list_node;
}

jr_C_ExprNodeType *	jr_C_ExprNodeListCreateWithNode (c_expr_node)
	jr_C_ExprNodeType *		c_expr_node;
{
	jr_C_ExprNodeType *		new_list_node;

	new_list_node =  jr_C_ExprNodeCreate( jr_C_EXPR_NODE_LIST_TYPE, c_expr_node, 0, 0);

	return new_list_node;
}

void jr_C_ExprNodeListAppendNew (list_node, node_type, child0, child1, child2)
	jr_C_ExprNodeType *		list_node;
	jr_int					node_type;
	jr_C_ExprNodeType *		child0;
	jr_C_ExprNodeType *		child1;
	jr_C_ExprNodeType *		child2;
{
	jr_C_ExprNodeType *		new_node;

	new_node	= jr_C_ExprNodeCreate( node_type, child0, child1, child2);

	jr_C_ExprNodeListAppendNode (list_node, new_node);
}


void jr_C_ExprNodeListAppendNode (list_node, new_node)
	jr_C_ExprNodeType *		list_node;
	jr_C_ExprNodeType *		new_node;
{
	jr_C_ExprNodeType *		new_list_node;

	if (list_node->node_type != jr_C_EXPR_NODE_LIST_TYPE) {
		jr_coredump();
	}

	new_list_node = jr_C_ExprNodeListCreateWithNode (new_node);

	while (jr_C_ExprNodeRightChildPtr (list_node) != 0) {
		list_node = jr_C_ExprNodeRightChildPtr (list_node);
	}
	jr_C_ExprNodeSetRightChild(list_node, new_list_node);
}


void jr_C_ExprNodeListAppendList (list_node, append_list_node)
	jr_C_ExprNodeType *		list_node;
	jr_C_ExprNodeType *		append_list_node;
{
	if (list_node->node_type != jr_C_EXPR_NODE_LIST_TYPE) {
		jr_coredump();
	}

	while (jr_C_ExprNodeRightChildPtr (list_node) != 0) {
		list_node = jr_C_ExprNodeRightChildPtr (list_node);
	}

	jr_C_ExprNodeSetRightChild(list_node, append_list_node);
}


jr_int jr_C_ExprNodeTraversalDestroy (c_expr_node)
	jr_C_ExprNodeType *	c_expr_node;
{
	/*
	 * needs to take an expression first since it is 
	 * a traversal function
	 */
	if (jr_C_ExprNodeAllocFn( c_expr_node)) {
		jr_C_ExprNodeCallAllocFn( c_expr_node, jr_C_EXPR_VOID_VALUE, c_expr_node, 0);
	}

	jr_free (c_expr_node);

	return (0);
	/*
	 * return good status so we can use this as a traversal function
	 */
}


jr_int jr_C_ExprNodeTraverse (
								curr_node, pre_order_fn, in_order_fn, post_order_fn,
								data_ptr, opt_error_buf
							)
	jr_C_ExprNodeType *		curr_node;
	jr_int					(*pre_order_fn) ();
	jr_int					(*in_order_fn) ();
	jr_int					(*post_order_fn) ();
	void *					data_ptr;
	char *					opt_error_buf;
{
	jr_C_ExprNodeType *		left_node;
	jr_C_ExprNodeType *		right_node;
	jr_C_ExprNodeType *		extra_node;
	jr_int					status = 0;



	if (curr_node->node_type != jr_C_EXPR_NODE_LIST_TYPE) {

		left_node	= jr_C_ExprNodeLeftChildPtr (curr_node);
		right_node	= jr_C_ExprNodeRightChildPtr (curr_node);
		extra_node	= jr_C_ExprNodeExtraChildPtr (curr_node);

		if (pre_order_fn) {
			status = (*pre_order_fn) (curr_node, data_ptr, opt_error_buf);

			if (status != 0) {
				return (status);
			}
		}

		if (left_node) {
			status = jr_C_ExprNodeTraverse (
				left_node, pre_order_fn, in_order_fn, post_order_fn, data_ptr, opt_error_buf
			);

			if (status != 0) {
				return (status);
			}
		}

		if (in_order_fn) {
			status = (*in_order_fn) (curr_node, data_ptr, opt_error_buf);

			if (status != 0) {
				return (status);
			}
		}

		if (right_node) {
			status = jr_C_ExprNodeTraverse (
				right_node, pre_order_fn, in_order_fn, post_order_fn, data_ptr, opt_error_buf
			);

			if (status != 0) {
				return (status);
			}
		}

		if (extra_node) {
			status = jr_C_ExprNodeTraverse (
				extra_node, pre_order_fn, in_order_fn, post_order_fn, data_ptr, opt_error_buf
			);

			if (status != 0) {
				return (status);
			}
		}

		if (post_order_fn) {
			status = (*post_order_fn) (curr_node, data_ptr, opt_error_buf);

			if (status != 0) {
				return (status);
			}
		}
	}
	else {
		/*
		 * its a list token, and shouldn't be visited
		 * the left node is the expression, and the right node
		 * is the pointer to the next list element
		 */

		for (; curr_node != 0; curr_node = right_node) {
			left_node	= jr_C_ExprNodeLeftChildPtr (curr_node);
			right_node	= jr_C_ExprNodeRightChildPtr (curr_node);

			if (left_node) {
				status = jr_C_ExprNodeTraverse (
					left_node, pre_order_fn, in_order_fn, post_order_fn, data_ptr, opt_error_buf
				);

				if (status != 0) {
					return (status);
				}
			}

			if (post_order_fn == jr_C_ExprNodeTraversalDestroy) {
				jr_C_ExprNodeTraversalDestroy (curr_node);
			}
		}
	}

	return (status);
}


void jr_C_ExprNodeInitCopy (new_node, c_expr_node)
	jr_C_ExprNodeType *		new_node;
	jr_C_ExprNodeType *		c_expr_node;
{
	jr_C_ExprNodeType *		left_node;
	jr_C_ExprNodeType *		right_node;
	jr_C_ExprNodeType *		extra_node;

	void	(*node_alloc_fn)(
				jr_int				op_type,
				jr_C_ExprNodeType *	new_node,
				jr_C_ExprNodeType *	c_expr_node
			) = 0;


	memcpy (new_node, c_expr_node, sizeof (*new_node));

	left_node	= jr_C_ExprNodeLeftChildPtr (c_expr_node);
	right_node	= jr_C_ExprNodeRightChildPtr (c_expr_node);
	extra_node	= jr_C_ExprNodeExtraChildPtr (c_expr_node);

	if (left_node) {
		jr_C_ExprNodeLeftChildPtr (new_node)	= jr_C_ExprNodeCreateCopy (left_node);
		node_alloc_fn							= jr_C_ExprNodeAllocFn( left_node);
	}

	if (right_node) {
		jr_C_ExprNodeRightChildPtr (new_node)	= jr_C_ExprNodeCreateCopy (right_node);
		node_alloc_fn							= jr_C_ExprNodeAllocFn( right_node);
	}

	if (extra_node) {
		jr_C_ExprNodeExtraChildPtr (new_node)	= jr_C_ExprNodeCreateCopy (extra_node);
		node_alloc_fn							= jr_C_ExprNodeAllocFn( extra_node);
	}

	if (node_alloc_fn) {
		node_alloc_fn( jr_C_EXPR_PTR_VALUE, new_node, c_expr_node);
	}
}

jr_C_ExprNodeType *jr_C_ExprNodeCreateCopy (c_expr_node)
	jr_C_ExprNodeType *		c_expr_node;
{
	jr_C_ExprNodeType *		new_node;


	new_node = jr_malloc (sizeof (*new_node));

	jr_C_ExprNodeInitCopy (new_node, c_expr_node);

	return new_node;
}
