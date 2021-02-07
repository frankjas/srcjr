#ifndef __c_expr_node_h__
#define __c_expr_node_h__

#include "ezport.h"


typedef struct c_expr_node {
	jr_int					node_type;

	struct c_expr_node *	children [3];

	char *					identifier_name;		/* used for identifiers only, see below */

	jr_int					value_type;

	void					(*alloc_fn)(
								jr_int					op_type,
								struct c_expr_node *	c_expr_node, 
								struct c_expr_node *	opt_c_expr_node
							);

	union {
		jr_int				ival;
		unsigned jr_int		uval;
		double				dval;
		char *				sval;
		void *				pval;
	} c_expr_value;

} jr_C_ExprNodeType;


#define c_expr_ival			c_expr_value.ival
#define c_expr_uval			c_expr_value.uval
#define c_expr_fval			c_expr_value.dval
#define c_expr_dval			c_expr_value.dval
#define c_expr_sval			c_expr_value.sval
#define c_expr_pval			c_expr_value.pval



/******** C Expression Nodes ********/


extern jr_C_ExprNodeType *	jr_C_ExprNodeCreate		PROTO ((
								jr_int					node_type,
								jr_C_ExprNodeType *		child0,
								jr_C_ExprNodeType *		child1,
								jr_C_ExprNodeType *		child2
							));

extern void					jr_C_ExprNodeDestroy		PROTO ((
								jr_C_ExprNodeType *	c_expr_node
							));

extern jr_C_ExprNodeType *	jr_C_ExprNodeListCreate		PROTO ((
								jr_int					node_type,
								jr_C_ExprNodeType *		child0,
								jr_C_ExprNodeType *		child1,
								jr_C_ExprNodeType *		child2
							));

extern jr_C_ExprNodeType *	jr_C_ExprNodeListCreateWithNode PROTO((
								jr_C_ExprNodeType *		c_expr_node
							));

extern void				jr_C_ExprNodeListAppendNew	PROTO ((
							jr_C_ExprNodeType *		list_node,
							jr_int					node_type,
							jr_C_ExprNodeType *		child0,
							jr_C_ExprNodeType *		child1,
							jr_C_ExprNodeType *		child2
						));

extern void				jr_C_ExprNodeListAppendNode	PROTO ((
							jr_C_ExprNodeType *		list_node,
							jr_C_ExprNodeType *		new_list_node
						));

extern void				jr_C_ExprNodeListAppendList	PROTO ((
							jr_C_ExprNodeType *		list_node,
							jr_C_ExprNodeType *		other_list_node
						));

extern jr_int			jr_C_ExprNodeTraverse		PROTO ((
								jr_C_ExprNodeType *	curr_node,
								jr_int				(*pre_order_fn) (),
								jr_int				(*in_order_fn) (),
								jr_int				(*post_order_fn) (),
								void *				data_ptr,
								char *				opt_error_buf
						));


extern jr_int			jr_C_ExprNodeTraversalDestroy	PROTO ((
								jr_C_ExprNodeType *	c_expr_node
						));

extern void				jr_C_ExprNodeInitCopy			PROTO ((
							jr_C_ExprNodeType *			new_node,
							jr_C_ExprNodeType *			c_expr_node
						));

extern jr_C_ExprNodeType *	jr_C_ExprNodeCreateCopy		PROTO ((
								jr_C_ExprNodeType *		c_expr_node
						));

extern jr_int			jr_C_ExprNodeToString			PROTO ((
								char *					dest_buf,
								jr_int					buf_length,
								jr_C_ExprNodeType *		c_expr_node,
								char *					error_buf
						));

extern jr_int			jr_C_ExprNodeEvaluate		PROTO ((
							jr_C_ExprNodeType *		c_expr_node,
							void *					opt_c_expr,
							jr_int					(*identifier_fn) (),
							jr_int					(*function_call_fn) (),
							jr_int					(*sizeof_fn) (),
							void *					data_ptr,
							char *					opt_error_buf
						));

/******** Expression Node Macros ********/

#define jr_C_EXPR_NODE_LIST_TYPE		-1

#define jr_C_EXPR_VOID_VALUE			0
#define jr_C_EXPR_INT_VALUE				1
#define jr_C_EXPR_UNSIGNED_VALUE		2
#define jr_C_EXPR_FLOAT_VALUE			3
#define jr_C_EXPR_DOUBLE_VALUE			4
#define jr_C_EXPR_STRING_VALUE			5
#define jr_C_EXPR_PTR_VALUE				6

#define jr_C_ExprNodeTypeNumber(c_expr_node)		((c_expr_node)->node_type)
#define jr_C_ExprNodeValueTypeNumber(c_expr_node)	((c_expr_node)->value_type)
#define jr_C_ExprNodeChildPtr(c_expr_node, i)		((c_expr_node)->children[i])
#define jr_C_ExprNodeAllocFn(c_expr_node)			((c_expr_node)->alloc_fn)

#define jr_C_ExprNodeSetAllocFn(c_expr_node, fn)	((c_expr_node)->alloc_fn = (fn))
#define jr_C_ExprNodeCallAllocFn(c_expr_node, op_type, c_expr_node_1, c_expr_node_2)	\
		((c_expr_node)->alloc_fn( op_type, c_expr_node_1, c_expr_node_2))

#define jr_C_ExprNodeLeftChildPtr(c_expr_node)	jr_C_ExprNodeChildPtr(c_expr_node, 0)
#define jr_C_ExprNodeRightChildPtr(c_expr_node)	jr_C_ExprNodeChildPtr(c_expr_node, 1)
#define jr_C_ExprNodeExtraChildPtr(c_expr_node)	jr_C_ExprNodeChildPtr(c_expr_node, 2)
#define jr_C_ExprNodeLastChildPtr(c_expr_node)	jr_C_ExprNodeChildPtr(c_expr_node, 3)

#define jr_C_ExprNodeSetLeftChild( c_expr_node, child_node_ptr)							\
		(jr_C_ExprNodeLeftChildPtr( c_expr_node) = (child_node_ptr))

#define jr_C_ExprNodeSetRightChild( c_expr_node, child_node_ptr)						\
		(jr_C_ExprNodeRightChildPtr( c_expr_node) = (child_node_ptr))

#define jr_C_ExprNodeSetExtraChild( c_expr_node, child_node_ptr)						\
		(jr_C_ExprNodeExtraChildPtr( c_expr_node) = (child_node_ptr))

#define jr_C_ExprNodeSetLastChild( c_expr_node, child_node_ptr)							\
		(jr_C_ExprNodeLastChildPtr( c_expr_node) = (child_node_ptr))


#define jr_C_ExprNodeCopyValue(dest, src)			\
		((dest)->value_type = (src)->value_type, (dest)->c_expr_dval = (src)->c_expr_dval)


#define jr_C_ExprNodeSetIntValue(c_expr_node, v)		\
		((c_expr_node)->value_type = jr_C_EXPR_INT_VALUE, (c_expr_node)->c_expr_ival = (v)) 

#define jr_C_ExprNodeSetUnsignedValue(c_expr_node, v)		\
		((c_expr_node)->value_type = jr_C_EXPR_UNSIGNED_VALUE, (c_expr_node)->c_expr_ival = (v)) 

#define jr_C_ExprNodeSetFloatValue(c_expr_node, v)			\
		((c_expr_node)->value_type = jr_C_EXPR_FLOAT_VALUE, (c_expr_node)->c_expr_dval = (v)) 

#define jr_C_ExprNodeSetDoubleValue(c_expr_node, v)		\
		((c_expr_node)->value_type = jr_C_EXPR_DOUBLE_VALUE, (c_expr_node)->c_expr_dval = (v)) 

#define jr_C_ExprNodeSetStringValue(c_expr_node, v)		\
		((c_expr_node)->value_type = jr_C_EXPR_STRING_VALUE, (c_expr_node)->c_expr_sval = (v)) 

#define jr_C_ExprNodeSetPtrValue(c_expr_node, v)			\
		((c_expr_node)->value_type = jr_C_EXPR_PTR_VALUE, (c_expr_node)->c_expr_pval = (v))

#define jr_C_ExprNodeSetVoidValue(c_expr_node)				\
		((c_expr_node)->value_type = jr_C_EXPR_VOID_VALUE, (c_expr_node)->c_expr_dval = (0.0))


#define jr_C_ExprNodeIntValue(c_expr_node)			((c_expr_node)->c_expr_ival)
#define jr_C_ExprNodeUnsignedValue(c_expr_node)		((c_expr_node)->c_expr_uval)
#define jr_C_ExprNodeFloatValue(c_expr_node)		((c_expr_node)->c_expr_dval)
#define jr_C_ExprNodeDoubleValue(c_expr_node)		((c_expr_node)->c_expr_dval)
#define jr_C_ExprNodeStringValue(c_expr_node)		((c_expr_node)->c_expr_sval)
#define jr_C_ExprNodePtrValue(c_expr_node)			((c_expr_node)->c_expr_pval)

#define jr_C_ExprNodeIsVoidValue(c_expr_node)		\
		((c_expr_node)->value_type == jr_C_EXPR_VOID_VALUE)

#define jr_C_ExprNodeIsIntValue(c_expr_node)		\
		((c_expr_node)->value_type == jr_C_EXPR_INT_VALUE)

#define jr_C_ExprNodeIsUnsignedValue(c_expr_node)		\
		((c_expr_node)->value_type == jr_C_EXPR_UNSIGNED_VALUE)

#define jr_C_ExprNodeIsFloatValue(c_expr_node)		\
		((c_expr_node)->value_type == jr_C_EXPR_FLOAT_VALUE)

#define jr_C_ExprNodeIsDoubleValue(c_expr_node)		\
		((c_expr_node)->value_type == jr_C_EXPR_DOUBLE_VALUE)

#define jr_C_ExprNodeIsStringValue(c_expr_node)		\
		((c_expr_node)->value_type == jr_C_EXPR_STRING_VALUE)

#define jr_C_ExprNodeIsPtrValue(c_expr_node)		\
		((c_expr_node)->value_type == jr_C_EXPR_PTR_VALUE)


#define jr_C_ExprNodeIsIntegerValue(c_expr_node)		\
		(		jr_C_ExprNodeIsIntValue (c_expr_node)		\
			||  jr_C_ExprNodeIsUnsignedValue (c_expr_node)	\
		)

#define jr_C_ExprNodeIsNumericValue(c_expr_node)			\
		( 		jr_C_ExprNodeIsIntegerValue (c_expr_node)	\
			||	jr_C_ExprNodeIsFloatValue (c_expr_node)		\
			||	jr_C_ExprNodeIsDoubleValue (c_expr_node)	\
		)


#define jr_C_ExprNodeIsTrue(c_expr_node)						\
		(	jr_C_ExprNodeIsDoubleValue (c_expr_node)			\
			?	jr_C_ExprNodeDoubleValue (c_expr_node)	!= 0	\
			:	jr_C_ExprNodeIntValue (c_expr_node) 	!= 0	\
		)
		/*
		 * The int value will cover all 4 byte values,
		 * the double value will cover the 8 byte values
		 */

#define jr_C_ExprNodeIsFalse(c_expr_node)				(! jr_C_ExprNodeIsTrue (c_expr_node))

#define jr_C_ExprNodeIsIdentifier(c_expr_node)			((c_expr_node)->identifier_name)
#define jr_C_ExprNodeIdentifierName(c_expr_node)		((c_expr_node)->identifier_name)



#endif
