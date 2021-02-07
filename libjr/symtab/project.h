#ifndef _stable_localh
#define _stable_localh

#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/string.h"
#include "jr/symtab.h"


/******** jr_ST_TypeStructs, jr_ST_Symbols, and jr_ST_Identifiers ********/

extern jr_ST_Identifier *		jr_ST_IdentifierCreate		PROTO ((
									char *					str
								));

extern void						jr_ST_IdentifierInit		PROTO ((
									jr_ST_Identifier *		id_ptr,
									const char *			str
								));

extern void						jr_ST_IdentifierDestroy		PROTO ((
									jr_ST_Identifier *		id_ptr
								));

extern void						jr_ST_IdentifierUndo		PROTO ((
									jr_ST_Identifier *		id_ptr
								));

extern jr_int					jr_ST_IdentifierHash		PROTO ((
									const void *			void_arg_1
								));
extern jr_int					jr_ST_IdentifierCmp			PROTO ((
									const void *			void_arg_1,
									const void *			void_arg_2
								));

#define jr_ST_IdentifierSymbol(id)			((jr_ST_Symbol *)(id)->symbol)
#define jr_ST_IdentifierSetSymbol(id, v)	((id)->symbol = (v))

#define jr_ST_IdentifierIsInUse(id)			((id)->num_refs > 0)
#define jr_ST_IdentifierNumRefs(id)			((id)->num_refs)
#define jr_ST_IdentifierIncrementRefs(id)	((id)->num_refs++)
#define jr_ST_IdentifierDecrementRefs(id)	((id)->num_refs--)


#define jr_ST_TypeIsInUse(type)				((type)->num_refs > 0)
#define jr_ST_TypeNumRefs(type)				((type)->num_refs)
#define jr_ST_TypeIncrementRefs(type)		((type)->num_refs++)
#define jr_ST_TypeDecrementRefs(type)		((type)->num_refs--)



extern void jr_ST_ArrayListDestroyLengthStrings		PROTO ((jr_AList *list));
	

/******** jr_ST_TypeStruct Functions ********/

extern jr_int			jr_ST_TypeHash				PROTO ((
							const void *			void_arg_1
						));

extern void				jr_ST_TypeInitForLookup			PROTO ((
							jr_ST_TypeStruct *		new_type,
							jr_int					type_kind,
							jr_ST_Symbol *			base_sym,
							jr_int					num_stars,
							jr_AList *				list
						));

extern void				jr_ST_TypeUndoForLookup			PROTO ((
							jr_ST_TypeStruct *			type
						));

extern void				jr_ST_TypeLookupInitDuplicate	PROTO ((
							jr_ST_TypeStruct *		new_type,
							jr_ST_SymbolTable *		symbol_table
						));

/******** Scopes and jr_ST_Symbol Table ********/




extern jr_ST_Symbol *		jr_ST_SymbolAllocate		PROTO ((
								jr_ST_ScopeHeader *		scope_header,
								jr_ST_Identifier *		id
							));

extern jr_ST_Identifier *	jr_ST_EnterIdentifier		PROTO ((
								jr_ST_SymbolTable *		symbol_table,
								const char *			str
							));

extern jr_ST_TypeStruct *	jr_ST_EnterType				PROTO ((
								jr_ST_SymbolTable *		symbol_table, 
								jr_int					type_kind,
								jr_ST_Symbol *			base_sym,
								jr_int					num_stars,
								jr_AList *				list
							));

extern void					jr_ST_UnreferenceType		PROTO ((
								jr_ST_SymbolTable *		symbol_table,
								jr_ST_TypeStruct *		type_ptr
							));

extern void					jr_ST_UnreferenceIdentifier	PROTO ((
								jr_ST_SymbolTable *		symbol_table,
								jr_ST_Identifier *		id_ptr
							));



/******** Duplicate String Table (for file names) ********/

typedef struct {
	char *		string;
	jr_int		num_references;
} jr_ST_StringType;

extern char *				jr_ST_GetUniqueString		PROTO ((
								jr_ST_SymbolTable *		stable,
								char *					string
							));

extern void					jr_ST_StringInit			PROTO ((
								jr_ST_StringType *		string_info,
								char *					string
							));

extern void					jr_ST_StringUndo			PROTO ((
								jr_ST_StringType *		string_info
							));

extern jr_int				jr_ST_StringHash			PROTO ((
								const void *			void_arg_1
							));

extern jr_int				jr_ST_StringCmp				PROTO ((
								const void *			void_arg_1,
								const void *			void_arg_2
							));

extern void					jr_ST_StringInitForLookUp	PROTO ((
								jr_ST_StringType *		string_info,
								char *					string
							));

#define						jr_ST_StringUndoForLookUp(si)		((void) (si))


#define jr_ST_StringValue(string_info)					((string_info)->string)

#endif
