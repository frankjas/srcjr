#include "att_mngr.h"

jr_AttributeManagerType *	jr_AttributeManagerCreate()
{
	jr_AttributeManagerType *	new_manager ;

	new_manager = (jr_AttributeManagerType *) jr_malloc(sizeof(jr_AttributeManagerType)) ;

	jr_AttributeManagerInit(new_manager) ;

	return(new_manager) ;
}

void	jr_AttributeManagerInit(manager)
	jr_AttributeManagerType *	manager ;
{
	memset (manager, 0, sizeof (*manager));

	jr_HTableInit (
		manager->string_table,
		sizeof (jr_StringRefsStruct),
		jr_StringRefsHash,
		jr_StringRefsCmp
	) ;

	jr_HTableInitForPtrValues (manager->attribute_table, jr_AttributeHash, jr_AttributeCmp) ;

	manager->next_list_id		= 1;
}

void	jr_AttributeManagerUndo(manager)
	jr_AttributeManagerType *	manager ;
{
	jr_StringRefsStruct *		string_info;

	jr_HTableForEachElementPtr(manager->string_table, string_info) {
		jr_StringRefsUndo (string_info);
	}

	jr_HTableUndo(manager->string_table) ;

	jr_HTableUndo(manager->attribute_table) ;
}

void	jr_AttributeManagerDestroy(manager)
	jr_AttributeManagerType *	manager ;
{
	jr_AttributeManagerUndo(manager) ;
	jr_free(manager) ;
}

const char *	jr_AttributeManagerAddString(manager, string_arg, mask_arg)
	jr_AttributeManagerType *	manager ;
	const char *				string_arg ;
	unsigned jr_int				mask_arg ;
{
	jr_StringRefsStruct			tmp_string_info[1];
	jr_StringRefsStruct *		string_info;


	jr_StringRefsInit (tmp_string_info, string_arg);

	string_info				= jr_HTableFindElementPtr (manager->string_table, tmp_string_info);

	if (string_info == 0) {
		string_info			= jr_HTableSetNewElement (manager->string_table, tmp_string_info);
	}
	else {
		jr_StringRefsUndo (tmp_string_info);
	}

	string_info->num_references++;
	string_info->mask_value	|= mask_arg;

	return string_info->string;
}

void	jr_AttributeManagerDeleteString(manager, string_arg)
	jr_AttributeManagerType *	manager ;
	const char *				string_arg ;
{
	jr_StringRefsStruct			tmp_string_info[1];
	jr_StringRefsStruct *		string_info;
	jr_int						index;


	jr_StringRefsInit (tmp_string_info, string_arg);

	index				= jr_HTableFindElementIndex (manager->string_table, tmp_string_info);

	if (index >= 0) {
		string_info		= jr_HTableElementPtr (manager->string_table, index);

		string_info->num_references--;

		if (string_info->num_references <= 0) {
			jr_StringRefsUndo (string_info);
			jr_HTableDeleteIndex (manager->string_table, index);
		}
	}
	jr_StringRefsUndo (tmp_string_info);
}

jr_int jr_AttributeManagerGetListId (manager)
	jr_AttributeManagerType *	manager ;
{
	jr_int						next_list_id;

	next_list_id				= manager->next_list_id;

	manager->next_list_id ++;

	return next_list_id;
}

void jr_AttributeManagerAddAttribute (manager, attribute_ptr)
	jr_AttributeManagerType *	manager ;
	jr_AttributeType *		attribute_ptr;
{
	jr_HTableNewPtrValueIndex (manager->attribute_table, attribute_ptr);
}

jr_AttributeType *jr_AttributeManagerFindAttribute (manager, list_id, name_string)
	jr_AttributeManagerType *	manager ;
	jr_int						list_id;
	const char *				name_string;
{
	jr_AttributeType			tmp_attribute[1];
	jr_AttributeType *			attribute_ptr;
	jr_int						table_index;

	
	tmp_attribute->list_id		= list_id;
	tmp_attribute->name_string	= (char *) name_string;

	table_index					= jr_HTableFindPtrValueIndex (
									manager->attribute_table, tmp_attribute
								);

	if (table_index < 0) {
		return 0;
	}
	attribute_ptr				= jr_HTableKeyPtrValue (manager->attribute_table, table_index);

	return attribute_ptr;
}

void jr_AttributeManagerDeleteAttribute (manager, attribute_ptr)
	jr_AttributeManagerType *	manager ;
	jr_AttributeType *		attribute_ptr;
{
	jr_HTableDeletePtrValue (manager->attribute_table, attribute_ptr);
}

void jr_StringRefsInit (string_info, string)
	jr_StringRefsStruct *		string_info;
	const char *				string;
{
	string_info->string			= jr_strdup (string);
	string_info->mask_value		= 0;
	string_info->num_references	= 0;
}

void jr_StringRefsUndo (string_info)
	jr_StringRefsStruct *		string_info;
{
	jr_free (string_info->string);
}

jr_int jr_StringRefsHash (void_arg_1)
	const void *				void_arg_1;
{
	const jr_StringRefsStruct *	string_info			= void_arg_1;

	return jr_strhash_pjw (string_info->string);
}

jr_int jr_StringRefsCmp (void_arg_1, void_arg_2)
	const void *				void_arg_1;
	const void *				void_arg_2;
{
	const jr_StringRefsStruct *	string_info_1		= void_arg_1;
	const jr_StringRefsStruct *	string_info_2		= void_arg_2;

	return strcmp (string_info_1->string, string_info_2->string);
}

jr_int jr_AttributeHash (void_arg_1)
	const void *				void_arg_1;
{
	const jr_AttributeType *	attribute_ptr		= void_arg_1;
	jr_int						hash_value			= 0;

	hash_value += attribute_ptr->list_id;
	hash_value += jr_strhash_pjw (attribute_ptr->name_string);

	return hash_value;
}

jr_int jr_AttributeCmp (void_arg_1, void_arg_2)
	const void *				void_arg_1;
	const void *				void_arg_2;
{
	const jr_AttributeType *	attribute_ptr_1		= void_arg_1;
	const jr_AttributeType *	attribute_ptr_2		= void_arg_2;
	jr_int						diff;

	diff = attribute_ptr_1->list_id - attribute_ptr_2->list_id;

	if (diff != 0) {
		return diff;
	}

	diff = strcmp (attribute_ptr_1->name_string, attribute_ptr_2->name_string);

	if (diff != 0) {
		return diff;
	}

	return 0;
}
