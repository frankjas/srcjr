#include "project.h"


jr_ST_SymbolTable *jr_ST_SymbolTableCreate (data_size, field_data_size)
	jr_int					data_size;
	jr_int					field_data_size;
{
	jr_ST_SymbolTable *		symbol_table;


	symbol_table = jr_malloc (sizeof (jr_ST_SymbolTable));

	jr_ST_SymbolTableInit (symbol_table, data_size, field_data_size);

	return (symbol_table);
}

void jr_ST_SymbolTableInit (symbol_table, data_size, field_data_size)
	jr_ST_SymbolTable *		symbol_table;
	jr_int					data_size;
	jr_int					field_data_size;
{
	memset (symbol_table, 0, sizeof (jr_ST_SymbolTable));

	jr_AVL_TreeInit (symbol_table->type_table, sizeof (jr_ST_TypeStruct), jr_ST_TypeCmp);
	jr_AVL_TreeInit (symbol_table->id_table, sizeof (jr_ST_Identifier), jr_ST_IdentifierCmp);

	jr_AListInit (symbol_table->scope_headers, sizeof(jr_ST_ScopeHeader));

	jr_HTableInit (
		symbol_table->string_table, sizeof (jr_ST_StringType), jr_ST_StringHash, jr_ST_StringCmp
	);

	symbol_table->data_size			= data_size;
	symbol_table->field_data_size	= field_data_size;

	symbol_table->semantic_error		= 0;
	symbol_table->fatal_error			= 0;
	symbol_table->internal_error		= 0;
	symbol_table->print_errors			= 0;


	jr_ST_OpenNewScope (symbol_table);
	/*
	 * open the keyword scope, scope 0
	 */

	jr_ST_OpenNewScope (symbol_table);
	/*
	 * open the jr_MatchPatternLikeCshal scope, scope  1
	 */
}

void jr_ST_SymbolTableDestroy (symbol_table)
	jr_ST_SymbolTable *		symbol_table;
{
	jr_ST_SymbolTableUndo (symbol_table);
	jr_free (symbol_table);
}

void jr_ST_SymbolTableUndo (symbol_table)
	jr_ST_SymbolTable *		symbol_table;
{
	jr_ST_ScopeHeader *		scope_header;


	jr_AVL_TreeTraverseInOrder (symbol_table->type_table, jr_ST_TypeUndo, 0);
	jr_AVL_TreeUndo (symbol_table->type_table);


	jr_AListForEachElementPtr (symbol_table->scope_headers, scope_header) {
		jr_LListUndo (scope_header->symbol_list);
	}
	jr_AListUndo (symbol_table->scope_headers);


	jr_AVL_TreeTraverseInOrder (symbol_table->id_table, jr_ST_IdentifierUndo, 0);
	jr_AVL_TreeUndo (symbol_table->id_table);

	{
		jr_ST_StringType *	string_info;

		jr_HTableForEachElementPtr (symbol_table->string_table, string_info) {
			jr_ST_StringUndo (string_info);
		}
	}
	jr_HTableUndo (symbol_table->string_table);

	if (symbol_table->default_file_name) {
		jr_free (symbol_table->default_file_name);
	}
}

void jr_ST_OpenNewScope (symbol_table)
	jr_ST_SymbolTable *			symbol_table;
{
	jr_ST_ScopeHeader *			scope_header;

	scope_header				= jr_AListNewTailPtr (symbol_table->scope_headers);
	scope_header->scope_num		= jr_AListTailIndex(symbol_table->scope_headers);

	jr_LListInit (
		scope_header->symbol_list, 
		symbol_table->data_size + sizeof (jr_ST_Symbol)
	);
}

void jr_ST_CloseCurrScope (symbol_table)
	jr_ST_SymbolTable *		symbol_table;
{
	jr_ST_Symbol *			curr_symbol;


	if (jr_ST_CurrScopeNum (symbol_table) == 0) return;
	/*
	 * can't close the keyword scope
	 */

	jr_LListForEachElementPtr (jr_ST_CurrScopeList (symbol_table), curr_symbol) {
		if (curr_symbol->id) {
			/*
			 * identifier could be 0 for un-named symbols
			 */
			jr_ST_IdentifierSetSymbol (curr_symbol->id, curr_symbol->prev_definition);

			jr_ST_UnreferenceIdentifier (symbol_table, curr_symbol->id);
		}
		if (curr_symbol->type) {
			jr_ST_UnreferenceType (symbol_table, curr_symbol->type);
		}
	}

	jr_LListUndo (jr_ST_CurrScopeHeader (symbol_table)->symbol_list);
	jr_AListDeletedTailPtr (symbol_table->scope_headers);
}


jr_ST_ScopeHeader *jr_ST_CloseCurrScopeAndSave (symbol_table)
	jr_ST_SymbolTable *			symbol_table;
{
	jr_ST_ScopeHeader *			saved_scope;
	jr_ST_Symbol *				curr_symbol;


	assert (jr_ST_CurrScopeNum (symbol_table) != 0);
	/*
	 * can't save the keyword scope
	 */

	jr_LListForEachElementPtr (jr_ST_CurrScopeList (symbol_table), curr_symbol) {
		if (curr_symbol->id) {
			/*
			 * identifier could be 0 for un-named symbols
			 */
			jr_ST_IdentifierSetSymbol (curr_symbol->id, curr_symbol->prev_definition);
		}
	}

	saved_scope = jr_memdup (jr_ST_CurrScopeHeader (symbol_table), sizeof (*saved_scope));

	jr_AListDeletedTailPtr (symbol_table->scope_headers);
	/*
	 * reclaim the space of this scope
	 */

	return (saved_scope);
}


jr_ST_Symbol *jr_ST_FindSymbol (symbol_table, str)
	jr_ST_SymbolTable *		symbol_table;
	const char *			str;
{
	jr_ST_Identifier		tmp_id[1];
	jr_ST_Identifier *		id_ptr;


	jr_ST_IdentifierInit (tmp_id, str);

	id_ptr = jr_AVL_TreeFindElementPtr (symbol_table->id_table, tmp_id);

	jr_ST_IdentifierUndo (tmp_id);

	if (id_ptr) {
		return jr_ST_IdentifierSymbol (id_ptr);
	}
	return 0;
}


jr_ST_Symbol *jr_ST_FindCompoundSymbolName (symbol_table, str, symbol_kind)
	jr_ST_SymbolTable *		symbol_table;
	char *					str;
	jr_int					symbol_kind;
{
	char buf[512];

	switch (symbol_kind) {
		case jr_ST_STRUCT :
			sprintf (buf, "struct %.256s", str);
			str = buf;
			break;

		case jr_ST_UNION :
			sprintf (buf, "union %.256s", str);
			str = buf;
			break;

		case jr_ST_ENUM :
			sprintf (buf, "enum %.256s", str);
			str = buf;
			break;

		case jr_ST_PRE_PROCESSOR_DEFINE :
			sprintf (buf, "define %.256s", str);
			str = buf;
			break;

		case jr_ST_PRE_PROCESSOR_MACRO :
			sprintf (buf, "macro %.256s", str);
			str = buf;
			break;
	}
	/*
	 * prepend the word "struct" or "union" to the string
	 */
	return jr_ST_FindSymbol (symbol_table, str);
}


jr_ST_Identifier *jr_ST_EnterIdentifier (symbol_table, str)
	jr_ST_SymbolTable *		symbol_table;
	const char *			str;
{
	jr_ST_Identifier		tmp_id[1];
	jr_ST_Identifier *		id_ptr;


	jr_ST_IdentifierInit (tmp_id, str);

	id_ptr = jr_AVL_TreeFindElementPtr (symbol_table->id_table, tmp_id);

	if (id_ptr) {
		jr_ST_IdentifierUndo (tmp_id);
	}
	else {
		id_ptr		= jr_AVL_TreeSetNewElement (symbol_table->id_table, tmp_id);
	}

	jr_ST_IdentifierIncrementRefs (id_ptr);

	return id_ptr;
}


jr_ST_Symbol *		jr_ST_SymbolAllocate (scope_header, id)
	jr_ST_ScopeHeader *		scope_header;
	jr_ST_Identifier *		id;
{
	jr_ST_Symbol *			new_symbol;


	new_symbol = jr_LListNewTailPtr (scope_header->symbol_list);
	/*
	 * add to tail so the scope symbol list has the same order
	 * as the declarations.  A loop on the scope will generate
	 * the same order
	 */

	memset (new_symbol, 0, jr_LListElementSize (scope_header->symbol_list));
	/*
	 * zero the symbol and the data portion
	 */

	new_symbol->id			= id;
	new_symbol->scope_num	= scope_header->scope_num;

	if (id) {
		/*
		 * could be unnamed symbol
		 */
		new_symbol->prev_definition		= jr_ST_IdentifierSymbol(id);
		jr_ST_IdentifierSetSymbol (id, new_symbol);

		new_symbol->str = jr_ST_IdentifierStr (id);
	}
	else {
		new_symbol->str = 0;
	}

	new_symbol->file_name	= "unknown";

	return (new_symbol);
}


jr_ST_Symbol *jr_ST_EnterNativeSymbol (symbol_table, symbol_kind, str, token_num, keyword_group)
	jr_ST_SymbolTable *		symbol_table;
	jr_int					symbol_kind;
	const char *			str;
	jr_int					token_num;
	jr_int					keyword_group;
{
	jr_ST_Symbol *			symbol;
	jr_ST_Identifier *	id;

	/*
	 * scope 0 is for keywords and builtin types
	 * if a later scope has str defined then
	 * adding str will cause a data structure inconsistency
	 */
	if (symbol = jr_ST_FindSymbol (symbol_table, str)) {
		return (0);
		/*
		 * previously declared
		 */
	}

	id = jr_ST_EnterIdentifier (symbol_table, str);

	symbol					= jr_ST_SymbolAllocate (
								jr_AListElementPtr (symbol_table->scope_headers, 0), id
							);

	symbol->symbol_kind		= symbol_kind;
	symbol->token_num		= token_num;
	symbol->keyword_group	= keyword_group;
	symbol->type			= 0;

	return (symbol);
}

jr_ST_Symbol *jr_ST_EnterSymbol (symbol_table, symbol_kind, str, base_sym, num_star, list)
	jr_ST_SymbolTable *	symbol_table;
	jr_int				symbol_kind;
	const char *		str;
	jr_ST_Symbol *		base_sym;
	jr_int				num_star;
	jr_AList *			list;
{
	jr_ST_Symbol *		symbol;
	jr_ST_Identifier *	id;
	char				buf[512];

	if (str) {
		/*
		 * could be un-named struct
		 */
		switch (symbol_kind) {

			case jr_ST_STRUCT :
				sprintf (buf, "struct %.256s", str);
				str = buf;
				break;

			case jr_ST_UNION :
				sprintf (buf, "union %.256s", str);
				str = buf;
				break;

			case jr_ST_ENUM :
				sprintf (buf, "enum %.256s", str);
				str = buf;
				break;

			case jr_ST_PRE_PROCESSOR_DEFINE :
				sprintf (buf, "define %.256s", str);
				str = buf;
				break;

			case jr_ST_PRE_PROCESSOR_MACRO :
				sprintf (buf, "macro %.256s", str);
				str = buf;
				break;
		}
		/*
		 * prepend the word "struct" or "union" to the string
		 */

		symbol = jr_ST_FindSymbol (symbol_table, str);

		if (symbol  &&  jr_ST_SymbolScopeNum (symbol) == jr_ST_CurrScopeNum (symbol_table)) {
			/*
			 * already defined in the current scope
			 */

			if (symbol_kind != jr_ST_STRUCT  && symbol_kind != jr_ST_UNION) {
				return (0);
			}
			/*
			 * If its a struct or union, then we
			 * may redefine it if the current definition
			 * doesn't have any fields
			 */
			if (jr_ST_TypeNumFields (symbol->type) != 0) {
				return (0);
			}
		}
		else {
			id			= jr_ST_EnterIdentifier (symbol_table, str);

			symbol		= jr_ST_SymbolAllocate (jr_ST_CurrScopeHeader(symbol_table), id);
		}
	}
	else {
		symbol = jr_ST_SymbolAllocate (jr_ST_CurrScopeHeader(symbol_table), 0);
	}


	symbol->symbol_kind = symbol_kind;

	switch (symbol_kind) {
		case jr_ST_TYPEDEF:
		case jr_ST_VARIABLE:
		case jr_ST_STRUCT:
		case jr_ST_UNION:
		case jr_ST_ENUM:
		case jr_ST_FUNCTION:
			if (symbol_kind == jr_ST_TYPEDEF) {
				symbol_kind = jr_ST_VARIABLE;
			}
			symbol->type = jr_ST_EnterType (symbol_table, symbol_kind, base_sym, num_star, list);
			break;

		case jr_ST_PRE_PROCESSOR_DEFINE :
		case jr_ST_PRE_PROCESSOR_MACRO :
			symbol->token_num	= num_star;
			break;
	}

	return (symbol);
}

void jr_ST_SetSymbolType (stable, symbol, base_sym, num_star, list)
	jr_ST_SymbolTable *		stable;
	jr_ST_Symbol *			symbol;
	jr_ST_Symbol *			base_sym;
	jr_int					num_star;
	jr_AList *				list;
{
	if (symbol->symbol_kind != jr_ST_STRING) {
		return;
	}

	symbol->symbol_kind = jr_ST_VARIABLE;

	symbol->type = jr_ST_EnterType (stable, jr_ST_VARIABLE, base_sym, num_star, list);
}

jr_ST_TypeStruct *jr_ST_EnterType (symbol_table, type_kind, base_sym, num_stars, list)
	jr_ST_SymbolTable *		symbol_table;
	jr_int					type_kind;
	jr_ST_Symbol *			base_sym;
	jr_int					num_stars;
	jr_AList *				list;
{
	jr_ST_TypeStruct		tmp_type[1];
	jr_ST_TypeStruct *		type_ptr;


	jr_ST_TypeInitForLookup (tmp_type, type_kind, base_sym, num_stars, list);

	type_ptr = jr_AVL_TreeFindElementPtr (symbol_table->type_table, tmp_type);

	if (type_ptr == 0) {
		/*
		 * else not in the table, copy the type information
		 */

		jr_ST_TypeLookupInitDuplicate (tmp_type, symbol_table);

		type_ptr = jr_AVL_TreeSetNewElement (symbol_table->type_table, tmp_type);
	}
	else {
		jr_ST_TypeUndoForLookup (tmp_type);
	}


	jr_ST_TypeIncrementRefs (type_ptr);

	return type_ptr;
}

void jr_ST_UnreferenceIdentifier (symbol_table, id_ptr)
	jr_ST_SymbolTable *	symbol_table;
	jr_ST_Identifier *	id_ptr;
{
	jr_ST_IdentifierDecrementRefs (id_ptr);

	if (jr_ST_IdentifierNumRefs (id_ptr) == 0) {
		id_ptr = jr_AVL_TreeExtractElement (symbol_table->id_table, id_ptr);
		jr_ST_IdentifierUndo (id_ptr);
		jr_AVL_TreeFreeElement (symbol_table->id_table, id_ptr);
	}
}


void jr_ST_UnreferenceType (symbol_table, type_ptr)
	jr_ST_SymbolTable *	symbol_table;
	jr_ST_TypeStruct *	type_ptr;
{
	jr_ST_TypeDecrementRefs (type_ptr);
	if (jr_ST_TypeNumRefs (type_ptr) == 0) {

		if (jr_ST_TypeKind (type_ptr) != jr_ST_VARIABLE) {
			jr_ST_FieldType *		field;

			jr_ST_FieldListForEachElementPtr (jr_ST_TypeFieldList (type_ptr), field) {
				jr_ST_UnreferenceIdentifier (symbol_table, jr_ST_FieldIdentifier (field));

				jr_ST_UnreferenceType (symbol_table, jr_ST_FieldTypePtr (field));
			}
		}

		type_ptr = jr_AVL_TreeExtractElement (symbol_table->type_table, type_ptr);
		jr_ST_TypeUndo (type_ptr);

		jr_AVL_TreeFreeElement (symbol_table->type_table, type_ptr);
	}
}

jr_ST_Symbol *jr_ST_FindBaseTypeSymbol (type)
	jr_ST_TypeStruct *		type;
{
	jr_ST_Symbol *	curr_base_type_sym;


	curr_base_type_sym = jr_ST_TypeBaseSymbol (type);

	if (curr_base_type_sym == 0) return (0);

	while (jr_ST_SymbolIsVariable (curr_base_type_sym) 
		|| jr_ST_SymbolIsFunction (curr_base_type_sym)) {
		/*
		 * bottom out when we hit the first typedef, builtin, struct
		 */

		curr_base_type_sym = jr_ST_TypeBaseSymbol (jr_ST_SymbolType (curr_base_type_sym));
	}
	return (curr_base_type_sym);
}


char *jr_ST_GetUniqueString (stable, string)
	jr_ST_SymbolTable *		stable;
	char *					string;
{
	jr_ST_StringType		tmp_string_info[1];
	jr_ST_StringType *		found_string_info;

	jr_ST_StringInitForLookUp (tmp_string_info, string);

	found_string_info		= jr_HTableFindElementPtr (stable->string_table, tmp_string_info);

	jr_ST_StringUndoForLookUp (tmp_string_info);

	if (found_string_info  ==  0) {
		jr_ST_StringInit (tmp_string_info, string);
		found_string_info	= jr_HTableSetNewElement (stable->string_table, tmp_string_info);
	}

	return jr_ST_StringValue (found_string_info);
}

void jr_ST_SetSymbolLocation (stable, symbol, file_name, line_number)
	jr_ST_SymbolTable *		stable;
	jr_ST_Symbol *			symbol;
	char *					file_name;
	jr_int					line_number;
{
	symbol->file_name		= jr_ST_GetUniqueString (stable, file_name);
	symbol->line_number		= line_number;
}

void jr_ST_SetFieldLocation (stable, field, file_name, line_number)
	jr_ST_SymbolTable *		stable;
	jr_ST_FieldType *		field;
	char *					file_name;
	jr_int					line_number;
{
	field->file_name		= jr_ST_GetUniqueString (stable, file_name);
	field->line_number		= line_number;
}
