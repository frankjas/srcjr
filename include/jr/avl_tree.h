#ifndef __AVL_HEADER__
#define __AVL_HEADER__

#include "ezport.h"

#include <stdio.h>
#include <stdarg.h>


typedef struct {
	void *		root_node;

	jr_int		element_size;

	jr_int		(*cmpfn) ();
	void *		cmpfn_arg;

	jr_int		prefix_size;

	unsigned	allocates_elements		: 1;
	unsigned	maintains_size			: 1;
} jr_AVL_Tree;


#define jr_AVL_TREE_MAX_HEIGHT			(12 * sizeof (void *))
/*
 * height 48 means the tree has more than 2^32 elements,
 * height 96 is more than 2^64 (I'm guessing here)
 * so if a void* is 8 bytes this will scale up to a tree with
 * more elements than addressable memory
 * AVL height <  1.44 * log2(n)  <  1.44 * 8 * sizeof(void*)  <  12 * sizeof(void*)
 */


typedef struct {
	void *		left_node_ptr ;
	void *		right_node_ptr ;
	jr_short	node_height;

	/*
	 * Height will never be greater than 100, on a typical system.
	 * The size of this structure will be padded by 2 bytes on
	 * systems with 'int' alignment requirements, so it shouldn't
	 * keep 'int' alignment on the application's data as well.
	 */
} jr_AVL_TreeElementStruct ;


typedef struct {
	jr_int		tree_size;
} jr_AVL_TreeSizeStruct ;


extern jr_AVL_Tree *	jr_AVL_TreeCreate		PROTO ((
							jr_int				element_size,
							jr_int				(*cmpfn) ()
						));

extern void				jr_AVL_TreeInit			PROTO ((
							jr_AVL_Tree *		avl_tree,
							jr_int				element_size,
							jr_int				(*cmpfn) ()
						));

extern void				jr_AVL_TreeDestroy		PROTO ((jr_AVL_Tree *avl_tree));
extern void				jr_AVL_TreeUndo			PROTO ((jr_AVL_Tree *avl_tree));

extern void				jr_AVL_TreeEmpty		PROTO ((jr_AVL_Tree *avl_tree));

extern void				jr_AVL_TreeSetMaintainsSize	PROTO ((
							jr_AVL_Tree *			avl_tree,
							jr_int					value
						));

extern jr_int			jr_AVL_TreeElementPrefixSize	PROTO ((
							jr_AVL_Tree *			avl_tree
						));

extern void				jr_AVL_TreeSetPrefixSize	PROTO ((
							jr_AVL_Tree *			avl_tree,
							jr_int					prefix_size
						));

extern void				jr_AVL_TreeSetContainers	PROTO ((
							jr_AVL_Tree *			avl_tree,
							...
						));

extern void				jr_AVL_TreeSetContainersFromVA_List	PROTO ((
							jr_AVL_Tree *			avl_tree,
							va_list					arg_list
						));

extern jr_int			jr_AVL_TreeInsertElementUsingKey	PROTO ((
							jr_AVL_Tree *			avl_tree,
							const void *			new_node,
							const void *			key_node
						));

extern void				jr_AVL_TreeInsertElementUsingIndex	PROTO ((
							jr_AVL_Tree *			avl_tree,
							const void *			new_node,
							jr_int					index
						));

extern void *			jr_AVL_TreeExtractElementUsingKey	PROTO ((
							jr_AVL_Tree *			avl_tree,
							const void *			key_node
						));

extern void *			jr_AVL_TreeExtractElementUsingIndex	PROTO ((
							jr_AVL_Tree *			avl_tree,
							jr_int					index_value
						));

extern void				jr_AVL_TreeAppendLastIndexedElement	PROTO ((
							jr_AVL_Tree *			avl_tree,
							const void *			new_node
						));

extern void *			jr_AVL_TreeFindElementPtrUsingKey PROTO ((
							jr_AVL_Tree *			avl_tree,
							const void *			obj
						));

extern void *			jr_AVL_TreeFindElementPtrUsingIndex PROTO ((
							jr_AVL_Tree *			avl_tree,
							jr_int					index
						));


extern void *			jr_AVL_TreeSmallestElementPtr	PROTO ((
								jr_AVL_Tree *			avl_tree
						));

extern void *			jr_AVL_TreeGreatestElementPtr	PROTO ((
								jr_AVL_Tree *			avl_tree
						));



extern void *			jr_AVL_TreePrevElementPtr		PROTO ((
								jr_AVL_Tree *			avl_tree,
								const void *			obj
						));

extern void *			jr_AVL_TreeNextElementPtr		PROTO ((
								jr_AVL_Tree *			avl_tree,
								const void *			obj
						));



extern void				jr_AVL_TreeTransformPrefix	PROTO ((
							jr_AVL_Tree *			avl_tree,
							void *					el_ptr,
							unsigned jr_int			magnitude,
							jr_int					which_way,
							jr_int					all_elements
						));

extern void				jr_AVL_TreeMakeNullElement	PROTO ((
							jr_AVL_Tree *			avl_tree,
							void *					el_ptr
						));

extern jr_int			jr_AVL_TreeIndexedCmp		PROTO ((
							const void *			void_ptr1,
							const void *			void_ptr2
						));



#define					jr_AVL_TreeRootPtr(avl_tree)									\
						((avl_tree)->root_node)

#define					jr_AVL_TreeElementSize(avl_tree)								\
						((avl_tree)->element_size)

#define					jr_AVL_TreePrefixSize(avl_tree)									\
						((avl_tree)->prefix_size)

#define					jr_AVL_TreeAllocatesElements(avl_tree)							\
						((avl_tree)->allocates_elements)

#define					jr_AVL_TreeMaintainsSize(avl_tree)								\
						((avl_tree)->maintains_size)

#define					jr_AVL_TreeIsEmpty(avl_tree)									\
						(jr_AVL_TreeRootPtr (avl_tree) == 0)

#define					jr_AVL_TreeIsRootElement(avl_tree, node)						\
						((void *)(node) == jr_AVL_TreeRootPtr (avl_tree))

#define					jr_AVL_TreeSetCmpFn(avl_tree, v)								\
						((avl_tree)->cmpfn = (v))

#define					jr_AVL_TreeSetCmpFnArg(avl_tree, v)								\
						((avl_tree)->cmpfn_arg = (v))

#define					jr_AVL_TreeSize(avl_tree)										\
						((avl_tree)->root_node											\
							? jr_AVL_TreeSubTreeSize (avl_tree, (avl_tree)->root_node)	\
							: 0															\
						)


/******** Element Operations ********/

extern void *			jr_AVL_TreeAllocateElementWithSize	PROTO ((
							jr_AVL_Tree *			avl_tree,
							jr_int					length
						));

extern void				jr_AVL_TreeFreeElement		PROTO ((
							jr_AVL_Tree *			avl_tree,
							void *					node_ptr
						));


#define					jr_AVL_ElementInfoPtrWithPrefix(node_ptr, prefix_size)					\
						((jr_AVL_TreeElementStruct *) 											\
							((char *) (node_ptr) - (prefix_size)) - 1							\
						)
					
#define					jr_AVL_ElementSizeInfoPtrWithPrefix(nptr, pre_size)						\
						((jr_AVL_TreeSizeStruct *) 												\
							jr_AVL_ElementInfoPtrWithPrefix (nptr, pre_size)  -  1				\
						)
					
#define					jr_AVL_ElementLeftChildPtrWithPrefix(node_ptr, prefix_size)				\
						(jr_AVL_ElementInfoPtrWithPrefix (node_ptr, prefix_size)->left_node_ptr)

#define					jr_AVL_ElementRightChildPtrWithPrefix(node_ptr, prefix_size)			\
						(jr_AVL_ElementInfoPtrWithPrefix (node_ptr, prefix_size)->right_node_ptr)

#define					jr_AVL_ElementHeightWithPrefix(node_ptr, prefix_size)					\
						(jr_AVL_ElementInfoPtrWithPrefix (node_ptr, prefix_size)->node_height)

#define					jr_AVL_ElementIsLeafWithPrefix(node_ptr, prefix_size)					\
						(jr_AVL_ElementHeightWithPrefix (node_ptr, prefix_size)  == 0)

#define					jr_AVL_ElementTreeSizeWithPrefix(node_ptr, prefix_size)					\
						(jr_AVL_ElementSizeInfoPtrWithPrefix (node_ptr, prefix_size)->tree_size)


/****** Convenience Functions ******/

#define					jr_AVL_TreeAllocateElement(avl_tree)					\
						jr_AVL_TreeAllocateElementWithSize (					\
							avl_tree, jr_AVL_TreeElementSize (avl_tree)			\
						)

extern void *			jr_AVL_TreeNewElementPtrWithSize	PROTO ((
							jr_AVL_Tree *				avl_tree,
							const void *				key_node,
							jr_int  					length
						));

extern void *			jr_AVL_TreeSetNewElementWithSize	PROTO ((
							jr_AVL_Tree *				avl_tree,
							const void *				key_node,
							jr_int  					length
						));

#define					jr_AVL_TreeNewElementPtr(avl_tree, obj) 				\
						jr_AVL_TreeNewElementPtrWithSize (						\
							avl_tree, obj, jr_AVL_TreeElementSize (avl_tree)	\
						)


#define					jr_AVL_TreeSetNewElement(avl_tree, obj) 				\
						jr_AVL_TreeSetNewElementWithSize (						\
							avl_tree, obj, jr_AVL_TreeElementSize (avl_tree)	\
						)


#define					jr_AVL_TreeInsertElement(avl_tree, new_el)		\
						jr_AVL_TreeInsertElementUsingKey (avl_tree, new_el, new_el)


/******** Index Insertions ********/

extern void *			jr_AVL_TreeNewIndexedElementPtrWithSize		PROTO ((
							jr_AVL_Tree *				avl_tree,
							jr_int						index,
							jr_int  					length
						));

extern void *			jr_AVL_TreeSetNewIndexedElementWithSize PROTO ((
							jr_AVL_Tree *				avl_tree,
							jr_int						index,
							const void *				new_element,
							jr_int  					length
						));

extern void *			jr_AVL_TreeNewLastIndexedElementPtrWithSize		PROTO ((
							jr_AVL_Tree *				avl_tree,
							jr_int  					length
						));

extern void *			jr_AVL_TreeSetNewLastIndexedElementWithSize		PROTO ((
							jr_AVL_Tree *				avl_tree,
							const void *				new_element,
							jr_int  					length
						));


#define					jr_AVL_TreeNewIndexedElementPtr(avl_tree, index)		\
						jr_AVL_TreeNewIndexedElementPtrWithSize (				\
							avl_tree, index, jr_AVL_TreeElementSize (avl_tree)	\
						)

#define					jr_AVL_TreeSetNewIndexedElement(avl_tree, index, obj)	\
						jr_AVL_TreeSetNewIndexedElementWithSize (				\
							avl_tree, index, obj, jr_AVL_TreeElementSize (avl_tree)	\
						)

#define					jr_AVL_TreeNewLastIndexedElementPtr(avl_tree)			\
						jr_AVL_TreeNewLastIndexedElementPtrWithSize (			\
							avl_tree, jr_AVL_TreeElementSize (avl_tree)			\
						)

#define					jr_AVL_TreeSetNewLastIndexedElement(avl_tree, obj)		\
						jr_AVL_TreeSetNewLastIndexedElementWithSize (			\
							avl_tree, obj, jr_AVL_TreeElementSize (avl_tree)	\
						)


#define					jr_AVL_TreeFindElementPtr(avl_tree, new_el)		\
						jr_AVL_TreeFindElementPtrUsingKey (avl_tree, new_el)

#define					jr_AVL_TreeFindIndexedElementPtr(avl_tree, index)		\
						jr_AVL_TreeFindElementPtrUsingIndex (avl_tree, index)

#define					jr_AVL_TreeExtractElement(avl_tree, key_el)		\
						jr_AVL_TreeExtractElementUsingKey (avl_tree, key_el)


extern jr_int			jr_AVL_TreeDeleteElement		PROTO ((
							jr_AVL_Tree *				avl_tree,
							void *						key_node
						));

extern jr_int			jr_AVL_TreeDeleteIndexedElement	PROTO ((
							jr_AVL_Tree *				avl_tree,
							jr_int						index
						));


extern jr_int			jr_AVL_TreeCalculateElementIndex	PROTO ((
							jr_AVL_Tree *				avl_tree,
							const void *				existing_node
						));


/******** Traversal & Misc *********/

#ifdef AVL_TRAV_SRC
#	define	jr_AVL_TreeTraverse_param		void *data_arg
#else
#	define	jr_AVL_TreeTraverse_param		...
#endif

extern void				jr_AVL_TreeTraverseInOrder		PROTO((
							jr_AVL_Tree *				avl_tree,
							void						(*printfn)(),
							jr_AVL_TreeTraverse_param
						));

#define					jr_AVL_TreeTraverseForUndo(avl_tree, undo_fn)		\
						jr_AVL_TreeTraverseInOrder (avl_tree, undo_fn)

extern void				jr_AVL_TreeTraverseElementInOrder	PROTO((
							jr_AVL_Tree *				avl_tree,
							const void *				node,
							void						(*printfn)(),
							jr_AVL_TreeTraverse_param
						));


extern void				jr_AVL_TreePrint				PROTO ((
								jr_AVL_Tree *			avl_tree,
								FILE *					wfp,
								void					(*print_node_fn) (),
								jr_int					print_width
						));



#define jr_AVL_TreeForEachElementIndex(avl_tree, index)					\
		for ((index) = 0;  (index) < jr_AVL_TreeSize (avl_tree);  (index)++)

#define jr_AVL_TreeForEachElementIndexRev(avl_tree, index)					\
		for ((index) = jr_AVL_TreeSize (avl_tree) - 1;  (index) >= 0;  (index)--)


#define jr_AVL_TreeForEachElementPtr(avl_tree, node_ptr)				\
		for (															\
			(node_ptr) = jr_AVL_TreeSmallestElementPtr (avl_tree);		\
			(node_ptr) != 0;											\
			(node_ptr) = jr_AVL_TreeNextElementPtr (avl_tree, node_ptr)	\
		)


#define jr_AVL_TreeForEachElementPtrRev(avl_tree, node_ptr)				\
		for (															\
			(node_ptr) = jr_AVL_TreeGreatestElementPtr (avl_tree);		\
			(node_ptr) != 0;											\
			(node_ptr) = jr_AVL_TreePrevElementPtr (avl_tree, node_ptr)	\
		)

#define jr_AVL_TreeLeftChildPtr(avl_tree, node_ptr)						\
		jr_AVL_ElementLeftChildPtrWithPrefix (node_ptr, jr_AVL_TreePrefixSize (avl_tree))


#define jr_AVL_TreeRightChildPtr(avl_tree, node_ptr)					\
		jr_AVL_ElementRightChildPtrWithPrefix (node_ptr, jr_AVL_TreePrefixSize (avl_tree))


#define jr_AVL_TreeElementHeight(avl_tree, node_ptr)					\
		jr_AVL_ElementHeightWithPrefix (node_ptr, jr_AVL_TreePrefixSize (avl_tree))


#define jr_AVL_TreeElementIsLeaf(avl_tree, node_ptr)					\
		jr_AVL_ElementIsLeafWithPrefix (node_ptr, jr_AVL_TreePrefixSize (avl_tree))

#define jr_AVL_TreeSubTreeSize(avl_tree, node_ptr)						\
		jr_AVL_ElementTreeSizeWithPrefix (node_ptr, jr_AVL_TreePrefixSize (avl_tree))
		
#define jr_AVL_TreeSetSubTreeSize(avl_tree, node_ptr, v)				\
		(jr_AVL_TreeSubTreeSize (avl_tree, node_ptr)  =  (v))
		

#endif
