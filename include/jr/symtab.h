#ifndef _stableh
#define _stableh

#include "ezport.h"

#include <stdio.h>

#include "jr/avl_tree.h"
#include "jr/llist.h"
#include "jr/alist.h"
#include "jr/htable.h"
#include "jr/ansi_c/exprnode.h"



/******** jr_ST_TypeStructs, jr_ST_Symbols, and jr_ST_Identifiers ********/

typedef struct {
	char *		str;
	void *		symbol;
	jr_int		num_refs;
} jr_ST_Identifier;

#define jr_ST_IdentifierStr(id)		((id)->str)



typedef struct {
	void *		base_sym;		/* pointer to st symbol, 0 for struct/unions */

	jr_int		num_stars;		/* levels of indirection */
	jr_int		type_kind;		/* either variable, function, struct, union */

	jr_AList	list[1];		/* holds the array sizes, fields, parameters */

	jr_int		num_refs;
} jr_ST_TypeStruct;

#define jr_ST_TypeKind(t)			((t)->type_kind)
#define jr_ST_TypeBaseSymbol(t)		((jr_ST_Symbol *) (t)->base_sym)
#define jr_ST_TypeNumStars(t)		((t)->num_stars)
#define jr_ST_TypeFieldList(t)		((t)->list)
#define jr_ST_TypeParamList(t)		((t)->list)
#define jr_ST_TypeArrayList(t)		((t)->list)
#define jr_ST_TypeEnumList(t)		((t)->list)

#define jr_ST_TypeNumFields(t)				jr_AListSize ((t)->list)
#define jr_ST_TypeNumParams(t)				jr_AListSize ((t)->list)
#define jr_ST_TypeNumEnums(t)				jr_AListSize ((t)->list)
#define jr_ST_TypeNumArrayDimensions(t)		jr_AListSize ((t)->list)


#define jr_ST_TypeIsVariable(t)		(jr_ST_TypeKind (t) == jr_ST_VARIABLE)
#define jr_ST_TypeIsFunction(t)		(jr_ST_TypeKind (t) == jr_ST_FUNCTION)
#define jr_ST_TypeIsStruct(t)		(jr_ST_TypeKind (t) == jr_ST_STRUCT)
#define jr_ST_TypeIsUnion(t)		(jr_ST_TypeKind (t) == jr_ST_UNION)
#define jr_ST_TypeIsEnum(t)			(jr_ST_TypeKind (t) == jr_ST_ENUM)


/******** jr_ST_Symbol Definition ********/

typedef struct symbol_struct {
	struct symbol_struct *	prev_definition;		/* prev definition of id */

	jr_ST_Identifier *		id;						/* id associated with sym */
	jr_ST_TypeStruct *		type;					/* associated type */
	jr_int					storage_class;

	struct symbol_struct *	user_storage_class;
	struct symbol_struct *	user_type_qualifier;

	char *					str;					/* a copy of the id pointer */

	jr_int					symbol_kind;			/* keyword, typedef etc. */
	jr_int					scope_num;				/* which scope it belongs to */

	jr_int					token_num;				/* used for keywords mostly */
	jr_int					keyword_group;			/* only used for keywords */

	char *					file_name;
	jr_int					line_number;
} jr_ST_Symbol;


#define jr_ST_KEYWORD					1
#define jr_ST_BUILTIN					2

#define jr_ST_TYPEDEF					4
#define jr_ST_VARIABLE					5

#define jr_ST_STRUCT					6
#define jr_ST_UNION						7
#define jr_ST_FUNCTION					8

#define jr_ST_STRING					9

#define jr_ST_USER_STORAGE_CLASS		10
#define jr_ST_CONSTANT					11
#define jr_ST_ENUM						12
#define jr_ST_USER_TYPE_QUALIFIER		13

#define jr_ST_PRE_PROCESSOR_DEFINE		14
#define jr_ST_PRE_PROCESSOR_MACRO		15



/******** Array Specification ********/

typedef struct {
	jr_int				length_num;
	char *				length_str;
	jr_C_ExprNodeType *	length_expr;
} jr_ST_ArrayDimension;

typedef jr_AList jr_ST_ArrayList;

#define			jr_ST_ArrayListCreate()			jr_AListCreate(sizeof (jr_ST_ArrayDimension))
#define			jr_ST_ArrayListInit(list)		jr_AListInit (list, sizeof (jr_ST_ArrayDimension))

extern void		jr_ST_ArrayListInitCopy			PROTO ((
					jr_AList *					dest_list,
					jr_AList *					src_list
				));

extern jr_AList *jr_ST_ArrayListCreateCopy		PROTO ((jr_AList *src_list));
extern void		jr_ST_ArrayListDestroy			PROTO ((jr_AList *list));
extern void		jr_ST_ArrayListUndo				PROTO ((jr_AList *list));


extern jr_int	jr_ST_ArrayListAddDimension		PROTO ((
					jr_AList *					list,
					jr_int						length_num,
					char *						length_str,
					jr_C_ExprNodeType *			length_expr
				));

#define			jr_ST_ArrayForEachDimension(list, i)	\
				jr_AListForEachElementIndex(list, i)

#define			jr_ST_ArrayListDimensionPtr(list, i)	\
				((jr_ST_ArrayDimension *) jr_AListElementPtr (list, i))

#define			jr_ST_ArrayDimensionLengthNum(list, i)	\
				(jr_ST_ArrayListDimensionPtr (list, i)->length_num)

#define			jr_ST_ArrayDimensionLengthStr(list, i)	\
				(jr_ST_ArrayListDimensionPtr (list, i)->length_str)

#define			jr_ST_ArrayDimensionLengthExpr(list, i)	\
				(jr_ST_ArrayListDimensionPtr (list, i)->length_expr)

#define			jr_ST_ArrayListNumDimensions(list)		\
				(jr_AListSize (list))
	

/******** Field Specification ********/

typedef struct {
	jr_ST_Identifier *		id;
	jr_ST_TypeStruct *		type;
	jr_C_ExprNodeType *		bit_field_expr;
	jr_int					bit_field_value;
	jr_int					storage_class;
	jr_ST_Symbol *			user_storage_sym;
	jr_ST_Symbol *			user_qualifier_sym;

	char *					file_name;
	jr_int					line_number;
} jr_ST_FieldType;

typedef jr_AList jr_ST_FieldList;


/******** jr_ST_Fields ********/

extern void		jr_ST_FieldInit				PROTO ((
					jr_ST_FieldType *		field, 
					char *					str,
					jr_int					storage_class,
					jr_ST_Symbol *			user_storage_sym,
					jr_ST_Symbol *			user_qualifier_sym,
					jr_ST_Symbol *			base_sym,
					jr_int					num_stars,
					jr_ST_ArrayList *		array,
					jr_int					bit_field_value,
					jr_C_ExprNodeType *		bit_field_expr
				));

extern void		jr_ST_FieldUndo				PROTO ((
					jr_ST_FieldType *		field
				));

extern jr_int	jr_ST_FieldCmp				PROTO ((
					jr_ST_FieldType *		field1,
					jr_ST_FieldType *		field2
				));



#define			jr_ST_FieldListCreate(stable) \
				jr_AListCreate((stable)->field_data_size + sizeof(jr_ST_FieldType))

#define			jr_ST_FieldListInit(list, stable) \
				jr_AListInit (list, (stable)->field_data_size + sizeof(jr_ST_FieldType))

extern void		jr_ST_FieldListDestroy PROTO((jr_ST_FieldList *list));
extern void		jr_ST_FieldListUndo PROTO((jr_ST_FieldList *list));

#define			jr_ST_FieldListSize(list) \
				jr_AListSize (list)

#define			jr_ST_FieldListDataSize(list) \
				(jr_AListElementSize (list)  -  sizeof (jr_ST_FieldType))


extern jr_int	jr_ST_FieldListAddField			PROTO((
					jr_ST_FieldList *			list, 
					char *						str,
					jr_int						storage_class,
					jr_ST_Symbol *				user_storage_sym,
					jr_ST_Symbol *				user_qualifier_sym,
					jr_ST_Symbol *				base_sym,
					jr_int						num_stars,
					jr_ST_ArrayList *			array,
					jr_int						bit_field_value,
					jr_C_ExprNodeType *			bit_field_expr
				));

#define			jr_ST_FieldListElementPtr(field_list, i) \
				((jr_ST_FieldType *) jr_AListElementPtr (field_list, i))

#define			jr_ST_FieldListForEachElementIndex(list, i)	\
				jr_AListForEachElementIndex (list, i)

#define			jr_ST_FieldListForEachElementPtr(list, field)	\
				jr_AListForEachElementPtr (list, field)


#define jr_ST_FieldTypePtr(field)			((field)-> type)
#define jr_ST_FieldIdentifier(field)		((field)-> id)
#define jr_ST_FieldStorageClass(field)		((field)-> user_storage_sym)
#define jr_ST_FieldTypeQualifier(field)		((field)-> user_qualifier_sym)
#define jr_ST_FieldBitFieldValue(field)		((field)-> bit_field_value)
#define jr_ST_FieldBitFieldExpr(field)		((field)-> bit_field_expr)

#define jr_ST_FieldFileName(field)			((field)-> file_name)
#define jr_ST_FieldLineNumber(field)		((field)-> line_number)

#define jr_ST_FieldStr(field)				jr_ST_IdentifierStr (jr_ST_FieldIdentifier(field))
#define jr_ST_FieldDataPtr(field)			((void *) (((jr_ST_FieldType *) (field)) + 1))

#define jr_ST_FieldPrintDecl(field, prefix, wfp) \
		jr_ST_PrintDeclaration ( \
			jr_ST_FieldTypePtr (field), jr_ST_FieldStr (field), prefix, wfp \
		)


extern void		jr_ST_FieldSetBitFieldExpr		PROTO ((
					jr_ST_FieldType *		field,
					jr_C_ExprNodeType *		length_expr
				));


typedef jr_ST_FieldType 			jr_ST_ParamType;
typedef jr_AList 					jr_ST_ParamList;


#define jr_ST_ParamListCreate		jr_ST_FieldListCreate
#define jr_ST_ParamListInit			jr_ST_FieldListInit
#define jr_ST_ParamListDestroy		jr_ST_FieldListDestroy
#define jr_ST_ParamListUndo			jr_ST_FieldListUndo

#define jr_ST_ParamListAddParam(list, str, sc, uss, uqs, bs, ns, array) \
		jr_ST_FieldListAddField(list, str, sc, uss, uqs, bs, ns, array, -1, 0)


#define jr_ST_ParamTypePtr			jr_ST_FieldTypePtr
#define jr_ST_ParamIdentifier		jr_ST_FieldIdentifier
#define jr_ST_ParamStr				jr_ST_FieldStr
#define jr_ST_ParamDataPtr			jr_ST_FieldDataPtr

#define jr_ST_ParamListElementPtr	jr_ST_FieldListElementPtr

#define jr_ST_ParamListForEachElementPtr(list, field)  \
		jr_ST_FieldListForEachElementPtr(list, field)

#define jr_ST_ParamListForEachElementIndex(list, i)  \
		jr_ST_FieldListForEachElementIndex(list, i)

#define jr_ST_ParamPrintDecl		jr_ST_FieldPrintDecl



/******** Enum Specification ********/

typedef struct {
	jr_ST_Identifier *		id;
	jr_int					enum_value;
	jr_C_ExprNodeType *		value_expr;
} jr_ST_EnumType;

typedef jr_AList jr_ST_EnumList;

#define			jr_ST_EnumListCreate()		jr_AListCreate(sizeof(jr_ST_EnumType))
#define			jr_ST_EnumListInit(list)	jr_AListInit (list, sizeof(jr_ST_EnumType))
extern void		jr_ST_EnumListDestroy		PROTO((jr_ST_EnumList *list));
extern void		jr_ST_EnumListUndo			PROTO((jr_ST_EnumList *list));

extern jr_int	jr_ST_EnumListAddEnum		PROTO ((
					jr_ST_FieldList *		list,
					char *					str,
					jr_int					enum_value,
					jr_C_ExprNodeType *		value_expr
				));

#define jr_ST_EnumIdentifier(enum_ptr)		((enum_ptr)->id)
#define jr_ST_EnumStr(enum_ptr)				jr_ST_IdentifierStr (jr_ST_EnumIdentifier(enum_ptr))
#define jr_ST_EnumValue(enum_ptr)			((enum_ptr)->enum_value)
#define jr_ST_EnumValueExpr(enum_ptr)		((enum_ptr)->value_expr)

#define jr_ST_EnumListForEachElementPtr(list, enum)	\
		jr_AListForEachElementPtr (list,enum)


/******** jr_ST_TypeStruct Functions ********/

extern jr_ST_TypeStruct *jr_ST_TypeCreate	PROTO((
					jr_int					type_kind,
					jr_ST_Symbol *			base_sym,
					jr_int					num_stars,
					jr_AList *				list
				));

extern void		jr_ST_TypeInit PROTO((
					jr_ST_TypeStruct *		new_type,
					jr_int					type_kind,
					jr_ST_Symbol *			base_sym,
					jr_int					num_stars,
					jr_AList *				list
				));

extern void		jr_ST_TypeDestroy			PROTO ((jr_ST_TypeStruct *type));
extern void		jr_ST_TypeUndo				PROTO ((jr_ST_TypeStruct *type));

extern void		jr_ST_TypeCollapsedInit		PROTO((
					jr_ST_TypeStruct *		new_type,
					jr_ST_TypeStruct *		type
				));

extern jr_int		jr_ST_CanCollapseBaseSymbol	PROTO ((
					jr_ST_TypeStruct *		type
				));

extern jr_int	jr_ST_TypeCmp				PROTO ((
					jr_ST_TypeStruct *		type1,
					jr_ST_TypeStruct *		type2
				));

extern jr_int	jr_ST_TypeEquiv				PROTO ((
					jr_ST_TypeStruct *		type1,
					jr_ST_TypeStruct *		type2
				));

extern jr_int	jr_ST_TypeIsValid			PROTO((
					jr_ST_TypeStruct *		type,
					jr_int					array_of_func,
					jr_int					func_ret_array,
					jr_int					func_ret_func
				));


#define jr_ST_TypeVariableInit(type, base, num_stars, list) \
		jr_ST_TypeInit (type, jr_ST_VARIABLE, base, num_stars, list)

#define jr_ST_TypeFunctionInit(type, base, num_stars, list) \
		jr_ST_TypeInit (type, jr_ST_FUNCTION, base, num_stars, list)

#define jr_ST_TypeStructInit(type, num_stars, list) \
		jr_ST_TypeInit (type, jr_ST_STRUCT, 0, num_stars, list)

#define jr_ST_TypeUnionInit(type, num_stars, list) \
		jr_ST_TypeInit (type, jr_ST_UNION, 0, num_stars, list)

#define jr_ST_TypeEnumInit(type, num_stars, list) \
		jr_ST_TypeInit (type, jr_ST_ENUM, 0, num_stars, list)


#define jr_ST_TypeVariableCreate(type, base, num_stars, list) \
		jr_ST_TypeCreate (type, jr_ST_VARIABLE, base, num_stars, list)

#define jr_ST_TypeFunctionCreate(type, base, num_stars, list) \
		jr_ST_TypeCreate (type, jr_ST_FUNCTION, base, num_stars, list)

#define jr_ST_TypeStructCreate(type, num_stars, list) \
		jr_ST_TypeCreate (type, jr_ST_STRUCT, 0, num_stars, list)

#define jr_ST_TypeUnionCreate(type, num_stars, list) \
		jr_ST_TypeCreate (type, jr_ST_UNION, 0, num_stars, list)

#define jr_ST_TypeEnumCreate(type, num_stars, list) \
		jr_ST_TypeCreate (type, jr_ST_ENUM, 0, num_stars, list)


/******** Scopes and jr_ST_Symbol Table ********/

typedef struct {
	jr_LList		symbol_list[1];
	jr_int			scope_num;
} jr_ST_ScopeHeader;

typedef struct {
	jr_AList		scope_headers[1];

	jr_AVL_Tree		id_table[1];
	jr_AVL_Tree		type_table[1];

	jr_HTable		string_table[1];
	char *			default_file_name;

	jr_int			data_size;
	jr_int			field_data_size;

	unsigned		semantic_error		: 1;
	unsigned		fatal_error			: 1;
	unsigned		internal_error		: 1;
	unsigned		print_errors		: 1;
} jr_ST_SymbolTable;


extern jr_ST_SymbolTable *	jr_ST_SymbolTableCreate			PROTO ((
								jr_int						data_size,
								jr_int						field_data_size
							));
extern void					jr_ST_SymbolTableInit			PROTO ((
								jr_ST_SymbolTable *			symbol_table,
								jr_int						data_size,
								jr_int						field_data_size
							));

extern void					jr_ST_SymbolTableDestroy		PROTO ((
								jr_ST_SymbolTable *			symbol_table
							));

extern void					jr_ST_SymbolTableUndo			PROTO ((
								jr_ST_SymbolTable *			symbol_table
							));



extern void					jr_ST_OpenNewScope				PROTO ((
								jr_ST_SymbolTable *			symbol_table
							));
extern void					jr_ST_CloseCurrScope			PROTO ((
								jr_ST_SymbolTable *			symbol_table
							));

extern jr_ST_ScopeHeader *	jr_ST_CloseCurrScopeAndSave		PROTO ((
								jr_ST_SymbolTable *			symbol_table
							));


#define jr_ST_CurrScopeNum(st)				\
		jr_AListTailIndex((st)->scope_headers)

#define jr_ST_CurrScopeHeader(st)			\
		((jr_ST_ScopeHeader *) jr_AListTailPtr((st)->scope_headers))

#define jr_ST_CurrScopeList(st)				\
		(jr_ST_CurrScopeHeader(st)->symbol_list)

#define jr_ST_ForEachScopeNum(st, scope_num) \
		for ((scope_num) = 1; (scope_num) <= jr_ST_CurrScopeNum(st); (scope_num)++)


#define jr_ST_ForEachSymbolInScope(st, scope_num, symbol) \
		jr_LListForEachElementPtr (jr_ST_GetScopeList(st, scope_num), symbol)

#define jr_ST_ForEachSymbolInCurrScope(st, symbol) \
		jr_LListForEachElementPtr (jr_ST_CurrScopeList(st), symbol)

#define jr_ST_ForEachNamedSymbolInScope(st, scope_num, symbol) \
		jr_ST_ForEachSymbolInScope (st, scope_num, symbol) \
			if (jr_ST_SymbolStr (symbol))

#define jr_ST_ForEachNamedSymbolInCurrScope(st, symbol) \
		jr_ST_ForEachSymbolInCurrScope (st, symbol) \
			if (jr_ST_SymbolStr (symbol))


#define jr_ST_GetScopeHeader(st, scope_num)	\
		((jr_ST_ScopeHeader *) jr_AListElementPtr((st)->scope_headers, scope_num))

#define jr_ST_GetScopeList(st, scope_num) \
		(jr_ST_GetScopeHeader(st, scope_num)->symbol_list)


/*
 * for the user to set and clear
 */
#define jr_ST_HasSemanticError(st)			((st)->semantic_error)
#define jr_ST_SetSemanticError(st)			((st)->semantic_error = 1)
#define jr_ST_ClearSemanticError(st)		((st)->semantic_error = 0)

#define jr_ST_HasFatalError(st)				((st)->fatal_error)
#define jr_ST_SetFatalError(st)				((st)->fatal_error = 1)
#define jr_ST_ClearFatalError(st)			((st)->fatal_error = 0)

#define jr_ST_HasInternalError(st)			((st)->internal_error)
#define jr_ST_SetInternalError(st)			((st)->internal_error = 1)
#define jr_ST_ClearInternalError(st)		((st)->internal_error = 0)

#define jr_ST_SetPrintErrorMessages(st)		((st)->print_errors = 1)
#define jr_ST_PrintErrorMessages(st)		((st)->print_errors)
#define jr_ST_ClearPrintErrorMessages(st)	((st)->print_errors = 0)


extern jr_ST_Symbol *	jr_ST_EnterNativeSymbol		PROTO ((
							jr_ST_SymbolTable *		symbol_table,
							jr_int					symbol_kind,
							const char *			str,
							jr_int					token_num,
							jr_int					keyword_group
						));


extern jr_ST_Symbol *	jr_ST_EnterSymbol			PROTO ((
							jr_ST_SymbolTable *		symbol_table,
							jr_int					symbol_kind,
							const char *			str,
							jr_ST_Symbol *			base_sym,
							jr_int					num_star,
							jr_AList *				list
						));

extern void				jr_ST_SetSymbolType			PROTO ((
							jr_ST_SymbolTable *		stable,
							jr_ST_Symbol *			symbol,
							jr_ST_Symbol *			base_sym,
							jr_int					num_star,
							jr_AList *				list
						));

extern void				jr_ST_SetSymbolLocation		PROTO ((
							jr_ST_SymbolTable *		stable,
							jr_ST_Symbol *			symbol,
							char *					file_name,
							jr_int					line_number
						));

extern void				jr_ST_SetFieldLocation		PROTO ((
							jr_ST_SymbolTable *		stable,
							jr_ST_FieldType *		field,
							char *					file_name,
							jr_int					line_number
						));


extern jr_ST_Symbol *	jr_ST_FindSymbol			PROTO ((
							jr_ST_SymbolTable *		symbol_table,
							const char *			str
						));

extern jr_ST_Symbol *	jr_ST_FindBaseTypeSymbol	PROTO ((
							jr_ST_TypeStruct *		type
						));

extern jr_ST_Symbol *	jr_ST_FindCompoundSymbolName	PROTO ((
							jr_ST_SymbolTable *		symbol_table,
							char *					str,
							jr_int					symbol_kind
						));


#define jr_ST_FindUnionSymbol(st, str)	\
		jr_ST_FindCompoundSymbolName(st, str, jr_ST_UNION)

#define jr_ST_FindStructSymbol(st, str) \
		jr_ST_FindCompoundSymbolName(st, str, jr_ST_STRUCT)

#define jr_ST_FindEnumSymbol(st, str) \
		jr_ST_FindCompoundSymbolName(st, str, jr_ST_ENUM)

#define jr_ST_FindPreProcessorDefine(st, str) \
		jr_ST_FindCompoundSymbolName(st, str, jr_ST_PRE_PROCESSOR_DEFINE)

#define jr_ST_FindPreProcessorMacro(st, str) \
		jr_ST_FindCompoundSymbolName(st, str, jr_ST_PRE_PROCESSOR_MACRO)

/*
 * didn't rename find_struct_union when I added the enum support
 */

#define jr_ST_EnterKeyword(st, str, token_num, keyword_group) \
		jr_ST_EnterNativeSymbol(st, jr_ST_KEYWORD, str, token_num, keyword_group)

#define jr_ST_EnterBuiltInType(st, str, token_num, keyword_group) \
		jr_ST_EnterNativeSymbol(st, jr_ST_BUILTIN, str, token_num, keyword_group)


#define jr_ST_EnterStringDef(st, str) \
		jr_ST_EnterSymbol(st, jr_ST_STRING, str, 0, 0, 0)

#define jr_ST_EnterVariableDef(st, str, base, num_star, array) \
		jr_ST_EnterSymbol(st, jr_ST_VARIABLE, str, base, num_star, array)

#define jr_ST_EnterTypedef(st, str, base, num_star, array) \
		jr_ST_EnterSymbol(st, jr_ST_TYPEDEF, str, base, num_star, array)

#define jr_ST_EnterUserStorageClass(st, str) \
		jr_ST_EnterSymbol(st, jr_ST_USER_STORAGE_CLASS, str, 0, 0, 0)

#define jr_ST_EnterUserTypeQualifier(st, str) \
		jr_ST_EnterSymbol(st, jr_ST_USER_TYPE_QUALIFIER, str, 0, 0, 0)

#define jr_ST_EnterConstant(st, str) \
		jr_ST_EnterSymbol(st, jr_ST_CONSTANT, str, 0, 0, 0)

#define jr_ST_EnterStructDef(st, str, fields) \
		jr_ST_EnterSymbol(st, jr_ST_STRUCT, str, 0, 0, fields)

#define jr_ST_EnterUnionDef(st, str, fields) \
		jr_ST_EnterSymbol(st, jr_ST_UNION, str, 0, 0, fields)

#define jr_ST_EnterEnumDef(st, str, values) \
		jr_ST_EnterSymbol(st, jr_ST_ENUM, str, 0, 0, values)

#define jr_ST_EnterFunctionDef(st, str, base, num_star, params) \
		jr_ST_EnterSymbol(st, jr_ST_FUNCTION, str, base, num_star, params)

#define jr_ST_EnterPreProcessorDefine(st, str, macro_index) \
		jr_ST_EnterSymbol(st, jr_ST_PRE_PROCESSOR_DEFINE, str, 0, macro_index, 0)

#define jr_ST_EnterPreProcessorMacro(st, str, macro_index) \
		jr_ST_EnterSymbol(st, jr_ST_PRE_PROCESSOR_MACRO, str, 0, macro_index, 0)


#define jr_ST_SymbolHasType(symbol) jr_ST_SymbolType(symbol)

#define jr_ST_SymbolIsKeyword(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_KEYWORD)

#define jr_ST_SymbolIsBuiltInType(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_BUILTIN)

#define jr_ST_SymbolIsTypedef(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_TYPEDEF)

#define jr_ST_SymbolIsUserStorageClass(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_USER_STORAGE_CLASS)

#define jr_ST_SymbolIsUserTypeQualifier(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_USER_TYPE_QUALIFIER)

#define jr_ST_SymbolIsConstant(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_CONSTANT)

#define jr_ST_SymbolIsString(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_STRING)

#define jr_ST_SymbolIsVariable(symbol) \
		(jr_ST_SymbolKind (symbol) == jr_ST_VARIABLE)

#define jr_ST_SymbolIsFunction(symbol) \
		(jr_ST_SymbolKind(symbol) == jr_ST_FUNCTION)

#define jr_ST_SymbolIsStruct(symbol) \
		(jr_ST_SymbolKind(symbol) == jr_ST_STRUCT)

#define jr_ST_SymbolIsUnion(symbol) \
		(jr_ST_SymbolKind(symbol) == jr_ST_UNION)

#define jr_ST_SymbolIsEnum(symbol) \
		(jr_ST_SymbolKind(symbol) == jr_ST_ENUM)

#define jr_ST_SymbolIsPreProcessorDefine(symbol) \
		(jr_ST_SymbolKind(symbol) == jr_ST_PRE_PROCESSOR_DEFINE)

#define jr_ST_SymbolIsPreProcessorMacro(symbol) \
		(jr_ST_SymbolKind(symbol) == jr_ST_PRE_PROCESSOR_MACRO)


#define jr_ST_SymbolDataPtr(symbol)				((void *)((symbol)+1))
#define jr_ST_SymbolIdentifier(symbol)			((symbol)->id)
#define jr_ST_SymbolStr(symbol)					((symbol)->str)
#define jr_ST_SymbolKind(symbol)				((symbol)->symbol_kind)
#define jr_ST_SymbolType(symbol)				((symbol)->type)
#define jr_ST_SymbolStorageClass(symbol)		((symbol)->storage_class)
#define jr_ST_SymbolUserStorageClass(symbol)	((symbol)->user_storage_class)
#define jr_ST_SymbolUserTypeQualifier(symbol)	((symbol)->user_type_qualifier)
#define jr_ST_SymbolScopeNum(symbol)			((symbol)->scope_num)

#define jr_ST_SymbolFileName(symbol)			((symbol)->file_name)
#define jr_ST_SymbolLineNumber(symbol)			((symbol)->line_number)

#define jr_ST_KeywordTokenNum(symbol)			((symbol)->token_num)
#define jr_ST_SymbolTokenNum(symbol)			((symbol)->token_num)
#define jr_ST_SymbolPreProcessorIndex(symbol)	jr_ST_SymbolTokenNum (symbol)

#define jr_ST_SymbolSetUserStorageClass(symbol, v)		((symbol)->user_storage_class = (v))
#define jr_ST_SymbolSetUserTypeQualifier(symbol, v)		((symbol)->user_type_qualifier = (v))


#define jr_ST_SymbolIsInKeyWordGroup(symbol, group) \
		((symbol)->keyword_group & (group))



extern jr_int			jr_ST_PrintDeclaration			PROTO ((
							jr_ST_TypeStruct *			declarator_type,
							char *						opt_declarator_name,
							char *						prefix,
							FILE *						wfp
						));

extern jr_int			jr_ST_DeclarationToString		PROTO ((
							jr_ST_TypeStruct *			declarator_type,
							jr_ST_Symbol *				opt_base_type_sym,
							char *						opt_declarator_name,
							jr_int						nesting_level,
							char *						dest_buf,
							jr_int						buf_length
						));

extern jr_int			jr_ST_TypeSymbolToString		PROTO ((
							jr_ST_Symbol *				base_type_sym,
							jr_int						nesting_level,
							char *						dest_buf,
							jr_int						buf_length
						));

extern jr_int			jr_ST_TypeListToString			PROTO ((
							jr_ST_TypeStruct *			type,
							char *						dest_buf,
							jr_int						buf_length
						));


#endif
