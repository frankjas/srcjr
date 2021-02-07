#include "esis_prj.h"

void jr_ESIS_TreeInit (tree_info, attr_manager)
	jr_ESIS_TreeType *			tree_info;
	jr_AttributeManagerType *	attr_manager;
{
	tree_info->attr_manager	= attr_manager;

	jr_HTableInit (
		tree_info->generic_element_table, sizeof (jr_ESIS_GenericElementType),
		jr_ESIS_GenericElementHash, jr_ESIS_GenericElementCmp
	);

	jr_ESIS_GlobalElementListInit (tree_info->element_list);

	jr_ESIS_ElementStackInit (tree_info->element_stack);
	jr_ESIS_ElementStackPush (tree_info->element_stack, 0);
}


void jr_ESIS_TreeUndo (tree_info)
	jr_ESIS_TreeType *			tree_info;
{
	jr_ESIS_GenericElementType *generic_info;
	jr_ESIS_ElementType *		element_ptr;


	jr_HTableForEachElementPtr (tree_info->generic_element_table, generic_info) {
		jr_ESIS_GenericElementUndo (generic_info);
	}
	jr_HTableUndo (tree_info->generic_element_table);

	jr_ESIS_ElementStackUndo (tree_info->element_stack);


	jr_ForEachESIS_ElementListPtr (tree_info->element_list, element_ptr) {
		jr_ESIS_ElementUndo (element_ptr);
	}
	jr_ESIS_ElementListUndo (tree_info->element_list);
}


jr_ESIS_ElementType *jr_ESIS_TreeNewElementPtr (tree_info, parent_element_ptr)
	jr_ESIS_TreeType *				tree_info;
	jr_ESIS_ElementType *			parent_element_ptr;
{
	jr_ESIS_ElementType *			element_ptr;

	element_ptr			= jr_ESIS_ElementListNewElementPtr (tree_info->element_list);

	jr_ESIS_ElementInit (element_ptr, tree_info, parent_element_ptr);

	if (parent_element_ptr) {
		jr_ESIS_ElementAddSubElement (parent_element_ptr, element_ptr);
	}
	return element_ptr;
}


void jr_ESIS_GenericElementInit (generic_info, name_str)
	jr_ESIS_GenericElementType *		generic_info;
	char *								name_str;
{
	generic_info->name_str		= jr_strdup (name_str);
	jr_ESIS_GenericElementListInit (generic_info->element_list);
}

void jr_ESIS_GenericElementUndo (generic_info)
	jr_ESIS_GenericElementType *		generic_info;
{
	jr_free (generic_info->name_str);
	jr_ESIS_ElementListUndo (generic_info->element_list);
}

jr_int jr_ESIS_GenericElementHash (void_arg_1)
	const void *						void_arg_1;
{
	const jr_ESIS_GenericElementType *	generic_info		= void_arg_1;
	jr_int								hash_value			= 0;

	hash_value				+= jr_strhash_pjw (generic_info->name_str);

	return hash_value;
}

jr_int jr_ESIS_GenericElementCmp (void_arg_1, void_arg_2)
	const void *						void_arg_1;
	const void *						void_arg_2;
{
	const jr_ESIS_GenericElementType *	generic_info_1		= void_arg_1;
	const jr_ESIS_GenericElementType *	generic_info_2		= void_arg_2;
	jr_int								diff;

	diff	= strcmp (generic_info_1->name_str, generic_info_2->name_str);
	if (diff != 0) {
		return diff;
	}

	return 0;
}

void jr_ESIS_ElementInit (element_ptr, tree_info, parent_element_ptr)
	jr_ESIS_ElementType *			element_ptr;
	jr_ESIS_TreeType *				tree_info;
	jr_ESIS_ElementType *			parent_element_ptr;
{
	jr_AttributeListInit (element_ptr->attr_list, tree_info->attr_manager);

	jr_ESIS_SubElementListInit (element_ptr->sub_element_list);
	element_ptr->parent_element_ptr	= parent_element_ptr;

	jr_AListInit (element_ptr->data_list, sizeof (char *));
}

void jr_ESIS_ElementUndo (element_ptr)
	jr_ESIS_ElementType *		element_ptr;
{
	jr_int						i;

	jr_AttributeListUndo (element_ptr->attr_list);
	jr_ESIS_ElementListUndo (element_ptr->sub_element_list);

	jr_AListForEachElementIndex (element_ptr->data_list, i) {
		jr_free (jr_AListNativeElement (element_ptr->data_list, i, char *));
	}
	jr_AListUndo (element_ptr->data_list);
}

void jr_ESIS_ElementSetData (element_ptr, data_str)
	jr_ESIS_ElementType *			element_ptr;
	char *							data_str;
{
	char *							new_data_str			= jr_strdup (data_str);

	jr_AListNativeSetNewTail (element_ptr->data_list, new_data_str, char *);
}

void jr_ESIS_ElementAddAttribute (element_ptr, attr_name_str, attr_value_str)
	jr_ESIS_ElementType *			element_ptr;
	char *							attr_name_str;
	char *							attr_value_str;
{
	if (attr_value_str) {
		jr_AttributeListAddAttribute (element_ptr->attr_list, attr_name_str, attr_value_str);
	}
}

void jr_ESIS_ElementAddSubElement (parent_element_ptr, element_ptr)
	jr_ESIS_ElementType *			parent_element_ptr;
	jr_ESIS_ElementType *			element_ptr;
{
	jr_ESIS_ElementListAppendToTail (parent_element_ptr->sub_element_list, element_ptr);
}

	
void jr_ESIS_IndexInit (esis_index, attr_name_str, opt_generic_name_str)
	jr_ESIS_IndexType *			esis_index;
	char *						attr_name_str;
	char *						opt_generic_name_str;
{
	esis_index->attr_name_str	= jr_strdup (attr_name_str);

	if (opt_generic_name_str) {
		esis_index->opt_generic_name_str	= jr_strdup (opt_generic_name_str);
	}

	jr_HTableInit (
		esis_index->attr_value_table, sizeof (jr_ESIS_IndexValueType),
		jr_ESIS_IndexValueHash, jr_ESIS_IndexValueCmp
	);
}

void jr_ESIS_IndexUndo (esis_index)
	jr_ESIS_IndexType *			esis_index;
{
	jr_ESIS_IndexValueType *	esis_index_value;

	jr_free (esis_index->attr_name_str);

	if (esis_index->opt_generic_name_str) {
		jr_free (esis_index->opt_generic_name_str);
	}

	jr_HTableForEachElementPtr (esis_index->attr_value_table, esis_index_value) {
		jr_ESIS_IndexValueUndo (esis_index_value);
	}
	jr_HTableUndo (esis_index->attr_value_table);
}

void jr_ESIS_IndexValueInit (esis_index_value, attr_value_str)
	jr_ESIS_IndexValueType *	esis_index_value;
	char *						attr_value_str;
{
	esis_index_value->attr_value_str	= jr_strdup (attr_value_str);

	jr_ESIS_MatchingElementListInit (esis_index_value->matching_element_list);
}

void jr_ESIS_IndexValueUndo (esis_index_value)
	jr_ESIS_IndexValueType *	esis_index_value;
{
	jr_free (esis_index_value->attr_value_str);

	jr_ESIS_ElementListUndo (esis_index_value->matching_element_list);
}

jr_int jr_ESIS_IndexValueHash (void_arg_1)
	const void *					void_arg_1;
{
	const jr_ESIS_IndexValueType *	esis_index_value		= void_arg_1;
	jr_int							hash_value			= 0;

	hash_value				+= jr_strhash_pjw (esis_index_value->attr_value_str);

	return hash_value;
}

jr_int jr_ESIS_IndexValueCmp (void_arg_1, void_arg_2)
	const void *					void_arg_1;
	const void *					void_arg_2;
{
	const jr_ESIS_IndexValueType *	esis_index_value_1		= void_arg_1;
	const jr_ESIS_IndexValueType *	esis_index_value_2		= void_arg_2;
	jr_int							diff;

	diff	= strcmp (esis_index_value_1->attr_value_str, esis_index_value_2->attr_value_str);

	if (diff != 0) {
		return diff;
	}

	return 0;
}


void jr_ESIS_ElementListInit (element_list)
	jr_ESIS_ElementList *		element_list;
{
	jr_LListInit (element_list->data_list, sizeof (jr_ESIS_ElementType));
}

void jr_ESIS_ElementListUndo (element_list)
	jr_ESIS_ElementList *		element_list;
{
	jr_LListUndo (element_list->data_list);
}

jr_ESIS_ElementType *jr_ESIS_ElementListNewElementPtr (element_list)
	jr_ESIS_ElementList *		element_list;
{
	jr_ESIS_ElementType *		element_ptr;

	element_ptr			= jr_LListNewTailPtr (element_list->data_list);

	return element_ptr;
}

void jr_ESIS_GlobalElementListInit (element_list)
	jr_ESIS_ElementList *		element_list;
{
	jr_ESIS_ElementListInit (element_list);

	jr_SetContainers (
		/* global list */	"jr_LList", 0,
		/* generic list */	"jr_LList", 0,
		/* sub element */	"jr_LList", 0,
		/* matching */		"jr_LList", 0,
		0
	);
}

void jr_ESIS_GenericElementListInit (element_list)
	jr_ESIS_ElementList *		element_list;
{
	jr_ESIS_ElementListInit (element_list);

	jr_SetContainers (
		/* global list */	"jr_LList", 0,
		/* generic list */	"jr_LList", element_list->data_list, 
		/* sub element */	"jr_LList", 0,
		/* matching */		"jr_LList", 0,
		0
	);
}

void jr_ESIS_SubElementListInit (element_list)
	jr_ESIS_ElementList *		element_list;
{
	jr_ESIS_ElementListInit (element_list);

	jr_SetContainers (
		/* global list */	"jr_LList", 0,
		/* generic list */	"jr_LList", 0,
		/* sub element */	"jr_LList", element_list->data_list,
		/* matching */		"jr_LList", 0,
		0
	);
}

void jr_ESIS_MatchingElementListInit (element_list)
	jr_ESIS_ElementList *		element_list;
{
	jr_ESIS_ElementListInit (element_list);

	jr_SetContainers (
		/* global list */	"jr_LList", 0,
		/* generic list */	"jr_LList", 0,
		/* sub element */	"jr_LList", 0,
		/* matching */		"jr_LList", element_list->data_list,
		0
	);
}

void jr_ESIS_ElementStackInit (element_stack)
	jr_ESIS_ElementStack *		element_stack;
{
	jr_AListInit (element_stack->data_list, sizeof (jr_ESIS_ElementType *));
}


void jr_ESIS_ElementStackUndo (element_stack)
	jr_ESIS_ElementStack *		element_stack;
{
	jr_AListUndo (element_stack->data_list);
}

