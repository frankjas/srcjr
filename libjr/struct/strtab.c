#include "strtab.h"


void jr_StringTableInit (str_table_ptr, user_data_size)
	jr_StringTableType *		str_table_ptr;
	jr_int						user_data_size;
{
	memset (str_table_ptr, 0, sizeof (*str_table_ptr));

	if (user_data_size == 0) {
		/*
		** 9/5/2005: Can't set an htable prefix without a data size,
		** otherwise the foreach loops miss the last element.
		*/
		user_data_size = 4;
	}

	jr_HTableInit (
		str_table_ptr->string_table, user_data_size,
		jr_StringTableEntryHash, jr_StringTableEntryCmp
	);
	jr_HTableSetPrefixSize (
		str_table_ptr->string_table, sizeof (jr_StringTableEntryType), 0, 0
	);
}


void jr_StringTableUndo (str_table_ptr)
	jr_StringTableType *		str_table_ptr;
{
	jr_StringTableEntryType *	str_entry_ptr;

	jr_HTableForEachElementPtr (str_table_ptr->string_table, str_entry_ptr) {
		jr_StringTableEntryUndo (str_entry_ptr - 1);
	}
	jr_HTableUndo (str_table_ptr->string_table);
}

jr_StringTableType *jr_StringTableCreate (user_data_size)
	jr_int						user_data_size;
{
	jr_StringTableType *		str_table_ptr;

	str_table_ptr	= jr_malloc (sizeof (jr_StringTableType));

	jr_StringTableInit (str_table_ptr, user_data_size);

	return str_table_ptr;
}


void jr_StringTableDestroy (str_table_ptr)
	jr_StringTableType *		str_table_ptr;
{
	jr_StringTableUndo (str_table_ptr);
	jr_free (str_table_ptr);
}


jr_int jr_StringTableAddNewString (str_table_ptr, new_string)
	jr_StringTableType *		str_table_ptr;
	const char *				new_string;
{
	jr_StringTableEntryType		tmp_str_entry [1];
	jr_int						new_index;

	jr_StringTableEntryInit		(tmp_str_entry, new_string);

	new_index	= jr_HTableFindElementIndex (str_table_ptr->string_table, tmp_str_entry + 1);
				/*
				** 11/1/05: the comp. and hash functions expect a pointer to the end
				** of the string table entry.
				*/

	if (new_index >= 0) {
		jr_StringTableEntryUndo (tmp_str_entry);
		return -(new_index + 1);
	}

	new_index	= jr_HTableNewElementIndex (str_table_ptr->string_table, tmp_str_entry + 1);
				/*
				** 11/1/05: the comp. and hash functions expect a pointer to the end
				** of the string table entry.
				*/
	
	/*
	** 9/5/2005: the string table entry is in the element prefix
	*/

	memcpy (
		jr_StringTablePrefixEntryPtr (str_table_ptr, new_index),
		tmp_str_entry,
		sizeof (jr_StringTableEntryType)
	);

	/*
	** 9/5/2005: the application data is uninitialized
	*/

	return new_index;
}


jr_int jr_StringTableFindString (str_table_ptr, string)
	jr_StringTableType *		str_table_ptr;
	const char *				string;
{
	jr_StringTableEntryType		tmp_str_entry [1];
	jr_int						string_index;

	jr_StringTableEntryInit		(tmp_str_entry, string);

	string_index	= jr_HTableFindElementIndex (str_table_ptr->string_table, tmp_str_entry + 1);
					/*
					** 11/1/05: the comp. and hash functions expect a pointer to the end
					** of the string table entry.
					*/

	jr_StringTableEntryUndo (tmp_str_entry);


	return string_index;
}


void jr_StringTableEntryInit (str_entry_ptr, string)
	jr_StringTableEntryType *	str_entry_ptr;
	const char *				string;
{
	memset (str_entry_ptr, 0, sizeof (*str_entry_ptr));

	str_entry_ptr->string		= jr_strdup (string);
}


void jr_StringTableEntryUndo (str_entry_ptr)
	jr_StringTableEntryType *	str_entry_ptr;
{
	jr_free (str_entry_ptr->string);
}


jr_int jr_StringTableEntryCmp (void_ptr_1, void_ptr_2)
	const void *				void_ptr_1;
	const void *				void_ptr_2;
{
	const jr_StringTableEntryType *	str_entry_ptr_1				= void_ptr_1;
	const jr_StringTableEntryType *	str_entry_ptr_2				= void_ptr_2;

	jr_int						diff;

	/*
	** 9/5/2005: the string table entry is in the element prefix
	*/

	str_entry_ptr_1--;
	str_entry_ptr_2--;

	diff		= strcmp (str_entry_ptr_1->string, str_entry_ptr_2->string);

	if (diff != 0) {
		return diff;
	}

	return 0;
}

jr_int jr_StringTableEntryHash (void_ptr)
	const void *				void_ptr;
{
	const jr_StringTableEntryType *	str_entry_ptr				= void_ptr;

	jr_int						hash_value					= 0;

	/*
	** 9/5/2005: the string table entry is in the element prefix
	*/
	str_entry_ptr --;


	hash_value	+= jr_strhash_pjw (str_entry_ptr->string);

	return hash_value;
}
