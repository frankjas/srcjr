#include "project.h"

jr_int jr_ST_FieldListAddField (	list, str, storage_class, user_storage_sym, user_qualifier_sym,
									base_sym, num_stars, array, bit_field_value, bit_field_expr
							)
	jr_ST_FieldList *		list; 
	char *					str;
	jr_int					storage_class;
	jr_ST_Symbol *			user_storage_sym;
	jr_ST_Symbol *			user_qualifier_sym;
	jr_ST_Symbol *			base_sym;
	jr_int					num_stars;
	jr_ST_ArrayList *		array;
	jr_int					bit_field_value;
	jr_C_ExprNodeType *		bit_field_expr;
{
	jr_ST_FieldType *field;

	if (*str) {
		jr_ST_FieldListForEachElementPtr (list, field) {
			if (strcmp (str, jr_ST_IdentifierStr (field->id)) == 0) {
				return (-1);
			}
		}
	}

	field = jr_AListNewTailPtr (list);

	memset (field + 1, 0, jr_AListElementSize (list) - sizeof (jr_ST_FieldType));

	jr_ST_FieldInit (
		field, str, storage_class, user_storage_sym, user_qualifier_sym,
		base_sym, num_stars, array,
		bit_field_value, bit_field_expr
	);

	return (jr_AListTailIndex (list));
}

void jr_ST_FieldListDestroy (list)
	jr_ST_FieldList *list;
{
	jr_ST_FieldListUndo (list);
	jr_free (list);
}

void jr_ST_FieldListUndo (list)
	jr_ST_FieldList *list;
{
	jr_ST_FieldType *field;

	jr_ST_FieldListForEachElementPtr (list, field) {
		jr_ST_FieldUndo (field);
	}
	jr_AListUndo (list);
}

void jr_ST_FieldInit (			field, str, storage_class, user_storage_sym, user_qualifier_sym,
								base_sym, num_stars, array, bit_field_value, bit_field_expr
							)
	jr_ST_FieldType *		field; 
	char *					str;
	jr_int					storage_class;
	jr_ST_Symbol *			user_storage_sym;
	jr_ST_Symbol *			user_qualifier_sym;
	jr_ST_Symbol *			base_sym;
	jr_int					num_stars;
	jr_ST_ArrayList *		array;
	jr_int					bit_field_value;
	jr_C_ExprNodeType *		bit_field_expr;
{
	memset (field, 0, sizeof (jr_ST_FieldType));

	field->id				= jr_ST_IdentifierCreate (str);
	field->type				= jr_ST_TypeCreate (jr_ST_VARIABLE, base_sym, num_stars, array);
	/*
	 * this type is only good as long as the base symbol is good
	 */
	field->storage_class		= storage_class;
	field->user_storage_sym		= user_storage_sym;
	field->user_qualifier_sym	= user_qualifier_sym;

	field->bit_field_value		= bit_field_value;

	if (bit_field_expr) {
		field->bit_field_expr	= jr_C_ExprNodeCreateCopy (bit_field_expr);
	}

	field->file_name		= "unknown";
}

void jr_ST_FieldUndo (field)
	jr_ST_FieldType *	field;
{
	if (! jr_ST_IdentifierIsInUse (field->id)) {
		jr_ST_IdentifierDestroy (field->id);
	}
	if (! jr_ST_TypeIsInUse (field->type)) {
		jr_ST_TypeDestroy (field->type);
	}
	if (field->bit_field_expr) {
		jr_C_ExprNodeDestroy (field->bit_field_expr);
	}
}

void jr_ST_FieldSetBitFieldExpr (field, length_expr)
	jr_ST_FieldType *	field;
	jr_C_ExprNodeType *	length_expr;
{
	if (field->bit_field_expr) {
		jr_C_ExprNodeDestroy (field->bit_field_expr);
	}
	field->bit_field_expr	= jr_C_ExprNodeCreateCopy (length_expr);
}

jr_int jr_ST_FieldCmp (field1, field2)
	jr_ST_FieldType *	field1;
	jr_ST_FieldType *	field2;
{
	jr_int			diff;

	diff = strcmp (jr_ST_FieldStr (field1), jr_ST_FieldStr (field2));

	if (diff != 0) {
		return diff;
	}

	diff = field1->storage_class - field2->storage_class;
	if (diff != 0) {
		return diff;
	}

	diff = field1->user_storage_sym - field2->user_storage_sym;
	if (diff != 0) {
		return diff;
	}

	diff = field1->user_qualifier_sym - field2->user_qualifier_sym;
	if (diff != 0) {
		return diff;
	}

	diff = field1->bit_field_value - field2->bit_field_value;
	if (diff != 0) {
		return diff;
	}

	diff = jr_ST_TypeCmp (jr_ST_FieldTypePtr (field1), jr_ST_FieldTypePtr (field2));

	if (diff != 0) {
		return diff;
	}

	return 0;
}



jr_int jr_ST_EnumListAddEnum (list, str, enum_value, value_expr)
	jr_ST_EnumList *	list; 
	char *				str;
	jr_int				enum_value;
	jr_C_ExprNodeType *	value_expr;
{
	jr_ST_EnumType *enum_ptr;

	jr_ST_EnumListForEachElementPtr (list, enum_ptr) {
		if (strcmp (str, jr_ST_EnumStr (enum_ptr)) == 0) {
			return (-1);
		}
	}

	enum_ptr = jr_AListNewTailPtr (list);

	enum_ptr->id			= jr_ST_IdentifierCreate (str);
	enum_ptr->enum_value	= enum_value;

	if (value_expr) {
		enum_ptr->value_expr	= jr_C_ExprNodeCreateCopy (value_expr);
	}
	else {
		enum_ptr->value_expr	= 0;
	}

	return 0;
}

void jr_ST_EnumListDestroy (list)
	jr_ST_EnumList *list;
{
	jr_ST_EnumListUndo (list);
	jr_free (list);
}

void jr_ST_EnumListUndo (list)
	jr_ST_EnumList *list;
{
	jr_ST_EnumType *enum_ptr;

	jr_ST_EnumListForEachElementPtr (list, enum_ptr) {
		if (! jr_ST_IdentifierIsInUse (enum_ptr->id)) {
			jr_ST_IdentifierDestroy (enum_ptr->id);
		}
		/*
		 * if the id num or type num is non-zero, then
		 * these ids and types are in id/type tables
		 * and they shouldn't be jr_freed
		 */
		if (enum_ptr->value_expr) {
			jr_C_ExprNodeDestroy (enum_ptr->value_expr);
		}
	}
	jr_AListUndo (list);
}


jr_int jr_ST_ArrayListAddDimension (list, length_num, length_str, length_expr)
	jr_AList *			list;
	jr_int				length_num;
	char *				length_str;
	jr_C_ExprNodeType *	length_expr;
{
	jr_ST_ArrayDimension *new_dimension;

	new_dimension = jr_AListNewTailPtr (list);

	new_dimension->length_num		= length_num;

	if (length_str) {
		new_dimension->length_str	= jr_strdup (length_str);
	}
	else {
		new_dimension->length_str	= 0;
	}
	if (length_expr) {
		new_dimension->length_expr	= jr_C_ExprNodeCreateCopy (length_expr);
	}
	else {
		new_dimension->length_expr	= 0;
	}
	return (jr_AListTailIndex (list));
}


void jr_ST_ArrayListDestroyLengthStrings (list)
	jr_AList *list;
{
	jr_ST_ArrayDimension *curr_dimension;
	
	jr_AListForEachElementPtr (list, curr_dimension) {
		if (curr_dimension->length_str) {
			jr_free (curr_dimension->length_str);
		}
		if (curr_dimension->length_expr) {
			jr_C_ExprNodeDestroy (curr_dimension->length_expr);
		}
	}
}


void jr_ST_ArrayListDestroy (list)
	jr_AList *list;
{
	jr_ST_ArrayListDestroyLengthStrings (list);

	jr_AListDestroy (list);
}


void jr_ST_ArrayListUndo (list)
	jr_AList *list;
{
	jr_ST_ArrayListDestroyLengthStrings (list);

	jr_AListUndo (list);
}


void jr_ST_ArrayListInitCopy (dest_list, src_list)
	jr_AList *		dest_list;
	jr_AList *		src_list;
{
	jr_int			i;
	

	jr_ST_ArrayListInit (dest_list);

	jr_ST_ArrayForEachDimension (src_list, i) {
		jr_ST_ArrayListAddDimension (
			dest_list, 
			jr_ST_ArrayDimensionLengthNum (src_list, i),
			jr_ST_ArrayDimensionLengthStr (src_list, i),
			jr_ST_ArrayDimensionLengthExpr (src_list, i)
		);
	}
}

jr_AList *jr_ST_ArrayListCreateCopy (src_list)
	jr_AList *		src_list;
{
	jr_AList *		dest_list;

	dest_list		= jr_malloc (sizeof (*dest_list));

	jr_ST_ArrayListInitCopy (dest_list, src_list);

	return dest_list;
}
