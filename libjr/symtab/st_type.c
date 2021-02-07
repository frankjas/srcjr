#include "project.h"


jr_ST_TypeStruct *jr_ST_TypeCreate (type_kind, base_sym, num_stars, list)
	jr_int					type_kind;
	jr_ST_Symbol *			base_sym;
	jr_int					num_stars;
	jr_AList *				list;
{
	jr_ST_TypeStruct *new_type = jr_malloc (sizeof (jr_ST_TypeStruct));

	jr_ST_TypeInit (new_type, type_kind, base_sym, num_stars, list);

	return (new_type);
}

void jr_ST_TypeInitForLookup (new_type, type_kind, base_sym, num_stars, list)
	jr_ST_TypeStruct *		new_type;
	jr_int					type_kind;
	jr_ST_Symbol *			base_sym;
	jr_int					num_stars;
	jr_AList *				list;
{
	new_type->type_kind		= type_kind;
	new_type->base_sym		= base_sym;
	new_type->num_stars		= num_stars;
	new_type->num_refs		= 0;

	if (list) {
		jr_AListInitCopy (new_type->list, list);
	}
	else {
		jr_ST_ArrayListInit (new_type->list);
		/*
		 * no list, never be added to so the type of list is arbitrary
		 * having a list always inited makes undoing easier
		 */
	}
}

void jr_ST_TypeUndoForLookup (type)
	jr_ST_TypeStruct *		type;
{
	jr_AListUndo (type->list);
	/*
	 * only the list was separately allocated
	 */
}


void jr_ST_TypeInit (new_type, type_kind, base_sym, num_stars, list)
	jr_ST_TypeStruct *		new_type;
	jr_int					type_kind;
	jr_ST_Symbol *			base_sym;
	jr_int					num_stars;
	jr_AList *				list;
{
	jr_ST_TypeInitForLookup (new_type, type_kind, base_sym, num_stars, list);

	switch (type_kind) {
		case jr_ST_VARIABLE	: {
			jr_ST_ArrayDimension *tmp_dimension;


			/*
			 * save the array length strings
			 */
			jr_AListForEachElementPtr (new_type->list, tmp_dimension) {
				if (tmp_dimension->length_expr) {
					tmp_dimension->length_expr = jr_C_ExprNodeCreateCopy (tmp_dimension->length_expr);
				}
				if (tmp_dimension->length_str) {
					tmp_dimension->length_str = jr_strdup (tmp_dimension->length_str);
				}
			}
			break;
		}

		default			: {
			/*
			 * recursively save the jr_ST_FieldType pointers, since they
			 * are the same as the list parameter
			 */
			jr_ST_FieldType *new_field;

			jr_ST_FieldListForEachElementPtr (new_type->list, new_field) {
				new_field->id = jr_ST_IdentifierCreate (new_field->id->str);

				new_field->type = jr_ST_TypeCreate (
					new_field->type->type_kind,
					new_field->type->base_sym,
					new_field->type->num_stars,
					new_field->type->list
				);
				if (new_field->bit_field_expr) {
					new_field->bit_field_expr = jr_C_ExprNodeCreateCopy (new_field->bit_field_expr); 
				}
				/*
				 * this type kind is guaranteed to be simple
				 * since field types are always simple
				 */
			}
			break;
		}
	}
}

void jr_ST_TypeLookupInitDuplicate (new_type, symbol_table)
	jr_ST_TypeStruct *		new_type;
	jr_ST_SymbolTable *		symbol_table;
{
	switch (new_type->type_kind) {
		case jr_ST_VARIABLE : {
			jr_ST_ArrayDimension *tmp_dimension;


			/*
			 * save the array length strings
			 */
			jr_AListForEachElementPtr (new_type->list, tmp_dimension) {
				if (tmp_dimension->length_expr) {
					tmp_dimension->length_expr = jr_C_ExprNodeCreateCopy (tmp_dimension->length_expr);
				}
				if (tmp_dimension->length_str) {
					tmp_dimension->length_str = jr_strdup (tmp_dimension->length_str);
				}
			}
			break;
		}
		case jr_ST_ENUM :	{
			jr_ST_EnumType *	enum_ptr;

			/*
			 * save all the enum fields
			 */
			jr_ST_EnumListForEachElementPtr (new_type->list, enum_ptr) {
				enum_ptr->id = jr_ST_EnterIdentifier (
					symbol_table, jr_ST_IdentifierStr (enum_ptr->id)
				);

				if (enum_ptr->value_expr) {
					enum_ptr->value_expr = jr_C_ExprNodeCreateCopy (enum_ptr->value_expr);
				}
			}

			break;
		}
		default : {
			jr_ST_FieldType *new_field;

			/*
			 * recursively save the fields in the list
			 * the new type list is already non recursive copy of the list
			 * go through and change the references to new
			 * type table entries
			 */

			jr_ST_FieldListForEachElementPtr (new_type->list, new_field) {
				new_field->id = jr_ST_EnterIdentifier (
					symbol_table, jr_ST_IdentifierStr (new_field->id)
				);

				new_field->type = jr_ST_EnterType (
					symbol_table,
					new_field->type->type_kind,
					new_field->type->base_sym,
					new_field->type->num_stars,
					new_field->type->list
				);
				if (new_field->bit_field_expr) {
					new_field->bit_field_expr = jr_C_ExprNodeCreateCopy (new_field->bit_field_expr); 
				}
			}
		}
	}
}


void jr_ST_TypeDestroy (type)
	jr_ST_TypeStruct *type;
{
	jr_ST_TypeUndo (type);
	jr_free (type);
}

void jr_ST_TypeUndo (type)
	jr_ST_TypeStruct *type;
{
	switch (jr_ST_TypeKind (type)) {
		case jr_ST_STRUCT :
		case jr_ST_UNION :
		case jr_ST_FUNCTION : {
			jr_ST_FieldListUndo (type->list);
			break;
		}
		case jr_ST_VARIABLE : {
			jr_ST_ArrayListUndo (type->list);
			break;
		}
		case jr_ST_ENUM : {
			jr_ST_EnumListUndo (type->list);
			break;
		}
	}
}

jr_int jr_ST_TypeHash (void_arg_1)
	const void *				void_arg_1;
{
	const jr_ST_TypeStruct *	type				= void_arg_1;
	jr_int						hash_value			= 0;

	if (jr_ST_TypeBaseSymbol (type) &&
		jr_ST_SymbolIdentifier (jr_ST_TypeBaseSymbol(type))) {

		hash_value += jr_strhash (jr_ST_SymbolStr(jr_ST_TypeBaseSymbol(type)));
	}
	else {
		hash_value += jr_ST_TypeKind(type) << 6;
	}

	switch (jr_ST_TypeKind (type)) {
		case jr_ST_VARIABLE : {
			jr_int num_dimension	= jr_ST_TypeNumArrayDimensions (type);
			jr_int sub				= num_dimension / 3;

			if (num_dimension) {
				hash_value += jr_ST_ArrayDimensionLengthNum (type->list, 0);
				hash_value += jr_ST_ArrayDimensionLengthNum (type->list, sub);
				hash_value += jr_ST_ArrayDimensionLengthNum (type->list, sub * 2);
				hash_value += jr_ST_ArrayDimensionLengthNum (
					type->list, num_dimension-1
				);

				hash_value *= num_dimension;
			}
			if (jr_ST_TypeNumStars (type)) {
				hash_value *= jr_ST_TypeNumStars (type);
			}

			break;
		}
		case jr_ST_STRUCT :
		case jr_ST_UNION :
		case jr_ST_FUNCTION : {
			jr_int						num_fields			= jr_ST_TypeNumFields (type);
			jr_int						sub					= num_fields / 3;
			jr_ST_FieldType *			field;
			const jr_ST_FieldList *		field_list			= jr_ST_TypeFieldList (type);
		
			if (num_fields) {
				field = jr_ST_FieldListElementPtr (field_list, 0);
				hash_value += jr_strhash (jr_ST_FieldStr (field));

				field = jr_ST_FieldListElementPtr (field_list, sub);
				hash_value += jr_strhash (jr_ST_FieldStr (field));

				field = jr_ST_FieldListElementPtr (field_list, sub * 2);
				hash_value += jr_strhash (jr_ST_FieldStr (field));

				field = jr_ST_FieldListElementPtr (field_list, num_fields - 1);
				hash_value += jr_strhash (jr_ST_FieldStr(field));

				hash_value *= num_fields;
			}

			if (jr_ST_TypeNumStars (type)) {
				hash_value *= jr_ST_TypeNumStars (type);
			}

			break;
		}
		case jr_ST_ENUM : {
			jr_int					num_enums = jr_ST_TypeNumEnums (type);
			jr_int					sub = num_enums / 3;
			jr_ST_EnumType *		enum_ptr;
			const jr_ST_EnumList *	enum_list = jr_ST_TypeEnumList (type);
		
			if (num_enums) {
				enum_ptr = jr_AListElementPtr (enum_list, 0);
				hash_value += jr_strhash (jr_ST_EnumStr (enum_ptr));

				enum_ptr = jr_AListElementPtr (enum_list, sub);
				hash_value += jr_strhash (jr_ST_EnumStr (enum_ptr));

				enum_ptr = jr_AListElementPtr (enum_list, sub * 2);
				hash_value += jr_strhash (jr_ST_EnumStr (enum_ptr));

				enum_ptr = jr_AListElementPtr (enum_list, num_enums - 1);
				hash_value += jr_strhash (jr_ST_EnumStr(enum_ptr));

				hash_value *= num_enums;
			}

			break;
		}
	}
	return (hash_value);
}


jr_int jr_ST_TypeCmp (type1, type2)
	jr_ST_TypeStruct *type1, *type2;
{
	jr_int comp;
	
	comp = jr_ST_TypeKind (type1) - jr_ST_TypeKind (type2);
	if (comp) return (comp);

	comp = jr_ST_TypeNumStars (type1) - jr_ST_TypeNumStars(type2);
	if (comp) return (comp);

	/*
	 * The base sym could be 0 if its a struct
	 * compare the base symbols using pointer comparisons
	 */

	comp = jr_ST_TypeBaseSymbol (type1) - jr_ST_TypeBaseSymbol (type2);
	if (comp != 0) {
		return comp;
	}

	switch (jr_ST_TypeKind (type1)) {
		case jr_ST_VARIABLE : {
			jr_int i;

			comp = jr_ST_TypeNumArrayDimensions (type1) - 
				   jr_ST_TypeNumArrayDimensions(type2);
			if (comp) return (comp);

			jr_ST_ArrayForEachDimension (type1->list, i) {

				comp = jr_ST_ArrayDimensionLengthNum (type1->list, i) -
					   jr_ST_ArrayDimensionLengthNum (type2->list, i);

				if (comp) return (comp);
			}
			return (0);
		}
		case jr_ST_STRUCT :
		case jr_ST_UNION :
		case jr_ST_FUNCTION : {
			jr_ST_FieldType *field1, *field2;

			comp = jr_ST_TypeNumFields (type1) - jr_ST_TypeNumFields (type2);
			if (comp) return (comp);

			comp = jr_ST_FieldListDataSize (type1->list) - jr_ST_FieldListDataSize (type2->list);
			if (comp) return (comp);

			field2 = jr_AListElementPtr (type2->list, 0);

			jr_ST_FieldListForEachElementPtr (type1->list, field1) {
				comp = jr_ST_FieldCmp (field1, field2);
				if (comp) return (comp);

				if (jr_ST_FieldListDataSize (type1->list)  >  0) {
					comp = memcmp (
							jr_ST_FieldDataPtr (field1), jr_ST_FieldDataPtr (field2),
							jr_ST_FieldListDataSize (type1->list)
						);
					if (comp) return (comp);
				}

				field2 = jr_AListNextElementPtr (type2->list, field2);
			}
			return (0);
		}
		case jr_ST_ENUM : {
			jr_ST_EnumType *enum1, *enum2;

			comp = jr_ST_TypeNumEnums (type1) - jr_ST_TypeNumEnums(type2);
			if (comp) return (comp);

			enum2 = jr_AListElementPtr (type2->list, 0);

			jr_ST_EnumListForEachElementPtr (type1->list, enum1) {
				comp = strcmp (jr_ST_EnumStr (enum1), jr_ST_EnumStr (enum2));

				if (comp) return (comp);

				comp = jr_ST_EnumValue (enum1) - jr_ST_EnumValue (enum2);
				if (comp) return (comp);

				enum2 = jr_AListNextElementPtr (type2->list, enum2);
			}
			return (0);
		}
	}
	/* can't get here */
	return 0;
}


jr_int jr_ST_TypeEquiv (type1, type2)
	jr_ST_TypeStruct *type1, *type2;
{
	jr_int comp;
	jr_ST_TypeStruct new_type1[1], new_type2[1];
	jr_ST_Symbol *base_sym1, *base_sym2;
	
	/*
	 * We will factor typedefs out of the base symbol collapsing
	 * the information into a new type until we hit a non-typedef base.
	 * 
	 * Then compare the types, if they are equal, call recursively
	 * on the new base syms.
	 *
	 * We can undo the new types before we call recursively.
	 *
	 * The collapsing may create an erroneous situation, i.e.
	 * an array of functions or such like.  We should check
	 * each type before entering to make sure this
	 * isn't possible.
	 */

	if (jr_ST_CanCollapseBaseSymbol (type1)) {
		jr_ST_TypeCollapsedInit (new_type1, type1);
		type1 = new_type1;
	}
	if (jr_ST_CanCollapseBaseSymbol (type2)) {
		jr_ST_TypeCollapsedInit (new_type2, type2);
		type2 = new_type2;
	}

	comp = jr_ST_TypeKind (type1) - jr_ST_TypeKind (type2);
	if (comp) goto not_equiv;

	comp = jr_ST_TypeNumStars (type1) - jr_ST_TypeNumStars(type2);
	if (comp) goto not_equiv;

	switch (jr_ST_TypeKind (type1)) {
		case jr_ST_VARIABLE : {
			jr_int i;

			comp = jr_ST_TypeNumArrayDimensions (type1) - 
				   jr_ST_TypeNumArrayDimensions(type2);
			if (comp) goto not_equiv;

			jr_ST_ArrayForEachDimension (type1->list, i) {

				comp = jr_ST_ArrayDimensionLengthNum (type1->list, i) -
					   jr_ST_ArrayDimensionLengthNum (type2->list, i);

				if (comp) goto not_equiv;
			}
			break;
		}
		case jr_ST_STRUCT :
		case jr_ST_UNION :
		case jr_ST_FUNCTION : {
			jr_ST_FieldType *field1, *field2;

			if (jr_ST_TypeKind (type1) == jr_ST_FUNCTION  && jr_ST_TypeNumFields (type1) == 0
				||  jr_ST_TypeNumFields (type2) == 0) {
				/*
				 * if its a function and one or both has
				 * no parameters, then that is type equivalent
				 * to any parameter list
				 */
				break;
			}

			comp = jr_ST_TypeNumFields (type1) - jr_ST_TypeNumFields(type2);
			if (comp) goto not_equiv;

			field2 = jr_AListElementPtr (type2->list, 0);

			jr_ST_FieldListForEachElementPtr (type1->list, field1) {
				if (!jr_ST_TypeEquiv (field1->type, field2->type)) {
					goto not_equiv;
				}

				field2 = jr_AListNextElementPtr (type2->list, field2);
			}
			break;
		}
		case jr_ST_ENUM : {
			jr_ST_EnumType *enum1, *enum2;

			comp = jr_ST_TypeNumEnums (type1) - jr_ST_TypeNumEnums(type2);
			if (comp) goto not_equiv;

			enum2 = jr_AListElementPtr (type2->list, 0);

			jr_ST_EnumListForEachElementPtr (type1->list, enum1) {
				comp = jr_ST_EnumValueExpr (enum1) - jr_ST_EnumValueExpr (enum2);

				if (comp) goto not_equiv;

				enum2 = jr_AListNextElementPtr (type2->list, enum2);
			}
			break;
		}
	}

	base_sym1 = jr_ST_TypeBaseSymbol (type1);
	base_sym2 = jr_ST_TypeBaseSymbol (type2);

	if (type1 == new_type1) jr_ST_TypeUndo (new_type1);
	if (type2 == new_type2) jr_ST_TypeUndo (new_type2);

	if (base_sym1 == base_sym2) {
		return (1);
	}
	else {
		if (jr_ST_SymbolType (base_sym1) && jr_ST_SymbolType (base_sym2)) {
			return (
				jr_ST_TypeEquiv (
					jr_ST_SymbolType (base_sym1), jr_ST_SymbolType (base_sym2)
				)
			);
			/*
			 * built-in types don't have base types
			 * if one's is a built-in and the other isn't
			 * they aren't equivalent
			 */
		}
		return (1);
	}

	not_equiv :
		if (type1 == new_type1) jr_ST_TypeUndo (new_type1);
		if (type2 == new_type2) jr_ST_TypeUndo (new_type2);

		return (0);
}

jr_int jr_ST_CanCollapseBaseSymbol (type)
	jr_ST_TypeStruct *type;
{
	if (jr_ST_TypeBaseSymbol (type) &&
		jr_ST_SymbolKind (jr_ST_TypeBaseSymbol (type)) == jr_ST_TYPEDEF) {

		jr_ST_Symbol *typedef_sym = jr_ST_TypeBaseSymbol (type);
		jr_ST_TypeStruct   *typedef_type = jr_ST_SymbolType (typedef_sym);

		if (type->num_stars == 0 ||
			jr_AListSize (typedef_type->list) == 0) {
			/*
			 * A typedef has the same semantics as parenthesizing
			 * declarators.  A typedef can be collapsed exactly
			 * when the parens around a declarator are trivial.
			 * See the production for DirectDeclarator in 
			 * parsegen's grammar.g for a description of the cases.
			 */

			return (1);
		}
	}
	return (0);
}

void jr_ST_TypeCollapsedInit (new_type, type)
	jr_ST_TypeStruct *new_type;
	jr_ST_TypeStruct *type;
{
	/*
	 * Remove reference to typedef symbol, and remove
	 * any trivial nesting caused by the typedef.  For example:
	 *
		typedef jr_int *int_pointer;
		int_pointer myvar[20];
	 *
	 * We must know that collapsing the typedef does not
	 * create array of functions or another nasty situation
	 */
	
	jr_ST_ArrayList		array_list[1];
	jr_AList *			list				= array_list;
	jr_int				kind				= jr_ST_TypeKind (type);
	jr_int				num_stars			= 0;
	/*
	 * if the kind is struct or union, this will break
	 * after the first loop.
	 * Otherwise its either function or variable
	 * If its a variable it will stay that way until a function
	 * typedef is encountered, adding array dimensions
	 */

	jr_ST_ArrayListInit (array_list);


	while (1) {
		num_stars += jr_ST_TypeNumStars (type);

		if (jr_ST_TypeKind (type) == jr_ST_FUNCTION) {
			kind = jr_ST_FUNCTION;
			list = jr_ST_TypeParamList (type);
		}
		if (kind == jr_ST_VARIABLE) {
			/*
			 * haven't encountered a function base symbol
			 */
			jr_int i;

			jr_ST_ArrayForEachDimension (type->list, i) {

				jr_ST_ArrayListAddDimension (
					array_list,
					jr_ST_ArrayDimensionLengthNum (type->list, i),
					jr_ST_ArrayDimensionLengthStr (type->list, i),
					jr_ST_ArrayDimensionLengthExpr (type->list, i)
				);
			}
		}
		if (!jr_ST_CanCollapseBaseSymbol (type)) break;
		type = jr_ST_SymbolType (jr_ST_TypeBaseSymbol (type));
	}

	jr_ST_TypeInit (new_type,
		kind,
		type->base_sym,
		num_stars,
		list
	);
	jr_ST_ArrayListUndo (array_list);
}


jr_int jr_ST_TypeIsValid (type, array_of_func, func_ret_array, func_ret_func)
	jr_ST_TypeStruct *type;
	jr_int array_of_func;
	jr_int func_ret_array;
	jr_int func_ret_func;
{
	/*
	 * Collapse typedefs and check for arrays of functions
	 * or functions returning arrays or returning functions
	 */
	
	jr_ST_Symbol *base_sym;
	jr_int has_array = 0, kind = jr_ST_VARIABLE;


	/*
	 * this loop changes base_sym only when the base symbol
	 * is not collapsable
	 * Otherwise it collapses the types looking for
	 * the 3 error cases
	 */
	for (	base_sym = jr_ST_TypeBaseSymbol (type); 
			!jr_ST_SymbolIsBuiltInType (base_sym);
			type = jr_ST_SymbolType (jr_ST_TypeBaseSymbol (type))
		) {

		if (jr_ST_TypeKind (type) == jr_ST_FUNCTION) {
			if (kind == jr_ST_FUNCTION) {
				return (func_ret_func);
			}
			if (has_array) {
				return (array_of_func);
			}
			kind = jr_ST_FUNCTION;
		}
		if (jr_ST_TypeKind (type) == jr_ST_VARIABLE) {
			has_array += jr_ST_TypeNumArrayDimensions (type);
			if (kind == jr_ST_FUNCTION) {
				return (func_ret_array);
			}
		}
		if (!jr_ST_CanCollapseBaseSymbol (type)) {
			base_sym = jr_ST_TypeBaseSymbol (type);
			has_array = 0;
			kind = jr_ST_VARIABLE;
			/*
			 * restart collapsing check from this base symbol
			 */
		}
	}
	/* can't get here */
	return 0;
}
