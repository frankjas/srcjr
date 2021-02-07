#include "jr/symtab.h"
#include "jr/trace.h"

jr_ST_Symbol *Int, *Char, *Float;

main (argc, argv)
	jr_int argc;
	char **argv;
{
	jr_ST_SymbolTable symbol_table[1];
	jr_ST_FieldList fields1[1], fields2[1];
	jr_ST_ArrayList arrays1[1], arrays2[1];
	jr_ST_Symbol *func_def, *symbol, *struct_def, *user_type;
	jr_ST_Symbol *sym1, *sym2;
	jr_ST_TypeStruct type[1];

	if (argc > 1) jr_set_trace (jr_malloc_calls);
	if (argc > 2) jr_set_trace (jr_malloc_trap);

	jr_ST_SymbolTableInit (symbol_table, 2 * sizeof (jr_int), sizeof (jr_int));

	jr_ST_EnterKeyword (symbol_table, "goto", 14, 0);
		if (symbol = jr_ST_FindSymbol (symbol_table, "goto")) {
			printf ("is keyword ? %d - ", 
				jr_ST_KeywordTokenNum (symbol)
			);
		}
		ST_PrintSymbol (symbol);

	jr_ST_EnterKeyword (symbol_table, "if", 15, 0);
		if (symbol = jr_ST_FindSymbol (symbol_table, "if")) {
			printf ("is keyword ? %d - ", 
				jr_ST_KeywordTokenNum (symbol)
			);
		}
		ST_PrintSymbol (symbol);

	jr_ST_EnterKeyword (symbol_table, "while", 16, 0);
		if (symbol = jr_ST_FindSymbol (symbol_table, "while")) {
			printf ("is keyword ? %d - ", 
				jr_ST_KeywordTokenNum (symbol)
			);
		}
		ST_PrintSymbol (symbol);

	jr_ST_EnterKeyword (symbol_table, "else", 17, 0);
		if (symbol = jr_ST_FindSymbol (symbol_table, "else")) {
			printf ("is keyword ? %d - ", 
				jr_ST_KeywordTokenNum (symbol)
			);
		}
		ST_PrintSymbol (symbol);

	Int = jr_ST_EnterBuiltInType (symbol_table, "int", 0, 0);
		symbol = jr_ST_FindSymbol(symbol_table, "int");
		ST_PrintSymbol (symbol);

	Char = jr_ST_EnterBuiltInType (symbol_table, "char", 0, 0);
		symbol = jr_ST_FindSymbol(symbol_table, "char");
		ST_PrintSymbol (symbol);

	Float = jr_ST_EnterBuiltInType (symbol_table, "float", 0, 0);
		symbol = jr_ST_FindSymbol(symbol_table, "float");
		ST_PrintSymbol (symbol);

	jr_ST_EnterStringDef (symbol_table, "generic string");
		symbol = jr_ST_FindSymbol(symbol_table, "generic string");
		ST_PrintSymbol (symbol);

	jr_ST_SetSymbolType (symbol_table, symbol, Int, 1, 0);
		symbol = jr_ST_FindSymbol(symbol_table, "generic string");
		ST_PrintSymbol (symbol);

	jr_ST_ArrayListInit (arrays1);
	jr_ST_ArrayListAddDimension (arrays1, 10, "5 + 5", 0);
	jr_ST_ArrayListAddDimension (arrays1, 0, 0, 0);

	jr_ST_ArrayListInit (arrays2);
	jr_ST_ArrayListAddDimension (arrays2, 10, "7 + 3", 0);
	jr_ST_ArrayListAddDimension (arrays2, 0, 0, 0);

	jr_ST_EnterVariableDef (symbol_table, "test_var", Int, 1, arrays1);
		symbol = jr_ST_FindSymbol (symbol_table, "test_var");
		ST_PrintSymbol (symbol);


	jr_ST_OpenNewScope (symbol_table);

	/*
	 * no test of renaming a keyword
	 */

	jr_ST_EnterVariableDef (symbol_table, "else", Float, 1, arrays1);
		symbol = jr_ST_FindSymbol (symbol_table, "else");
		ST_PrintSymbol (symbol);


	jr_ST_EnterVariableDef (symbol_table, "test_var", Int, 1, arrays1);
		symbol = jr_ST_FindSymbol (symbol_table, "test_var");
		ST_PrintSymbol (symbol);

	jr_ST_CloseCurrScope (symbol_table);

	jr_ST_EnterVariableDef (symbol_table, "test_var2", Int, 2, 0);
		symbol = jr_ST_FindSymbol (symbol_table, "test_var2");
		ST_PrintSymbol (symbol);

	jr_ST_EnterVariableDef (symbol_table, "test_var", Int, 0, 0);
		symbol = jr_ST_FindSymbol (symbol_table, "test_var");
		ST_PrintSymbol (symbol);

	jr_ST_FieldListInit (fields1, symbol_table);
	jr_ST_FieldListAddField (fields1, "field1", 0, 0, 0, Float, 0, 0, 0, 0);
	jr_ST_FieldListAddField (fields1, "field2", 0, 0, 0, Char, 0, arrays1, 0, 0);

	jr_ST_FieldListInit (fields2, symbol_table);
	jr_ST_FieldListAddField (fields2, "field1", 0, 0, 0, Float, 0, 0, 0, 0);
	jr_ST_FieldListAddField (fields2, "field2", 0, 0, 0, Char, 0, arrays2, 0, 0);

	struct_def = jr_ST_EnterStructDef (
		symbol_table, "test_struct", fields1
	);
		symbol = jr_ST_FindStructSymbol (symbol_table, "test_struct");
		ST_PrintSymbol (symbol);

	user_type = jr_ST_EnterTypedef (symbol_table,
		"TestStruct", struct_def, 1, 0
	);
		symbol = jr_ST_FindSymbol (symbol_table, "TestStruct");
		ST_PrintSymbol (symbol);

	jr_ST_EnterVariableDef (symbol_table, "new_type_var", user_type, 0, 0);
		symbol = jr_ST_FindSymbol (symbol_table, "new_type_var");
		ST_PrintSymbol (symbol);
		sym1 = symbol;

	struct_def = jr_ST_EnterStructDef (
		symbol_table, 0, fields1
	);

	jr_ST_EnterVariableDef (symbol_table, "no_name_var", struct_def, 1, 0);
		symbol = jr_ST_FindSymbol (symbol_table, "no_name_var");
		ST_PrintSymbol (symbol);
		sym2 = symbol;

	printf ("\njr_ST_TypeEquiv (%s, %s) ==  %d\n\n",
		jr_ST_SymbolStr (sym1), jr_ST_SymbolStr (sym2),
		jr_ST_TypeEquiv (jr_ST_SymbolType (sym1), jr_ST_SymbolType (sym2))
	);

	func_def = jr_ST_EnterFunctionDef (
		symbol_table, "my_func", Char, 1, fields1
	);
	symbol = jr_ST_FindSymbol (symbol_table, "my_func");
	ST_PrintSymbol (symbol);

	jr_ST_EnterVariableDef (symbol_table, 
		"my_func_ptr", func_def, 1, arrays1
	);
	symbol = jr_ST_FindSymbol (symbol_table, "my_func_ptr");
	ST_PrintSymbol (symbol);

	jr_ST_TypeInit (type, jr_ST_VARIABLE, Int, 1, arrays1);
	jr_ST_TypeUndo (type);

	jr_ST_TypeInit (type, jr_ST_STRUCT, 0, 1, fields1);
	jr_ST_TypeUndo (type);


	jr_ST_FieldListUndo (fields1);
	jr_ST_ArrayListUndo (arrays1);

	jr_ST_FieldListUndo (fields2);
	jr_ST_ArrayListUndo (arrays2);

	jr_ST_SymbolTableUndo (symbol_table);

	jr_malloc_stats (stdout, "After jr_ST_SymbolTableUndo");
}

