#ifndef __jr_strtab_h___
#define __jr_strtab_h___

#include "ezport.h"

#include "jr/htable.h"


typedef struct {
	jr_HTable			string_table [1];
} jr_StringTableType;

/*
** 11/3/05: Benefit of adding suffix "Type", can grep for all occurences
** of variable declarations more easily.
*/


typedef struct {
	const char *		string;
} jr_StringTableEntryType;


extern void				jr_StringTableInit				PROTO ((
							jr_StringTableType *		str_table_ptr,
							jr_int						user_data_size
						));

extern void				jr_StringTableUndo				PROTO ((
							jr_StringTableType *		str_table_ptr
						));

extern jr_StringTableType *jr_StringTableCreate			PROTO ((
							jr_int						user_data_size
						));

extern void				jr_StringTableDestroy			PROTO ((
							jr_StringTableType *		str_table_ptr
						));

extern jr_int			jr_StringTableAddNewString		PROTO ((
							jr_StringTableType *		str_table_ptr,
							const char *				new_string
						));

extern jr_int			jr_StringTableFindString		PROTO ((
							jr_StringTableType *		str_table_ptr,
							const char *				string
						));

#define					jr_StringTableSize(str_table_ptr)								\
						jr_HTableSize ((str_table_ptr)->string_table)

#define					jr_StringTableString(str_table_ptr, string_index)				\
						jr_StringTableEntryString (										\
							jr_StringTablePrefixEntryPtr (str_table_ptr, string_index)	\
						)

#define					jr_StringTableElementPtr(str_table_ptr, string_index)			\
						jr_HTableElementPtr (											\
							(str_table_ptr)->string_table, string_index					\
						)
						
#define					jr_StringTableForEachElementIndex(str_table_ptr, string_index)	\
						jr_HTableForEachElementIndex (									\
							(str_table_ptr)->string_table, string_index					\
						)

#define					jr_StringTableForEachElementPtr(str_table_ptr, el_ptr)			\
						jr_HTableForEachElementPtr (									\
							(str_table_ptr)->string_table, el_ptr						\
						)

#define					jr_StringTablePrefixEntryPtr(str_table_ptr, string_index)		\
						((jr_StringTableEntryType *)									\
							jr_HTableElementPtr (										\
								(str_table_ptr)->string_table, string_index				\
							) - 1														\
						)
						
#define					jr_StringTableEntryString(entry_ptr)							\
						((entry_ptr)->string)



#endif
