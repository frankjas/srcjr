#include "project.h"

jr_ST_Identifier *jr_ST_IdentifierCreate (str)
	char *					str;
{
	jr_ST_Identifier *		id_ptr;
	
	id_ptr = jr_malloc (sizeof(jr_ST_Identifier));

	jr_ST_IdentifierInit (id_ptr, str);

	return id_ptr;
}

void jr_ST_IdentifierInit (id_ptr, str)
	jr_ST_Identifier *		id_ptr;
	const char *			str;
{
	id_ptr->str			= jr_strdup (str);
	id_ptr->symbol		= 0;
	id_ptr->num_refs	= 0;
}

void jr_ST_IdentifierDestroy (id_ptr)
	jr_ST_Identifier *		id_ptr;
{
	jr_ST_IdentifierUndo (id_ptr);
	jr_free (id_ptr);
}

void jr_ST_IdentifierUndo (id_ptr)
	jr_ST_Identifier *		id_ptr;
{
	jr_free (id_ptr->str);
}


jr_int jr_ST_IdentifierHash (void_arg_1)
	const void *				void_arg_1;
{
	const jr_ST_Identifier *	id				= void_arg_1;

	return (jr_strhash (id->str));
}

jr_int jr_ST_IdentifierCmp (void_arg_1, void_arg_2)
	const void *				void_arg_1;
	const void *				void_arg_2;
{
	const jr_ST_Identifier *	id_1			= void_arg_1;
	const jr_ST_Identifier *	id_2			= void_arg_2;

	return (strcmp (id_1->str, id_2->str));
}


void jr_ST_StringInit (string_info, string)
	jr_ST_StringType *		string_info;
	char *					string;
{
	memset (string_info, 0, sizeof (*string_info));

	string_info->string		= jr_strdup (string);
}


void jr_ST_StringUndo (string_info)
	jr_ST_StringType *		string_info;
{
	jr_free (string_info->string);
}

void jr_ST_StringInitForLookUp (string_info, string)
	jr_ST_StringType *		string_info;
	char *					string;
{
	string_info->string		= string;
}

jr_int jr_ST_StringHash (void_arg_1)
	const void *				void_arg_1;
{
	const jr_ST_StringType *	string_info		= void_arg_1;
	jr_int						hash_value		= 0;

	hash_value	+= jr_strhash_pjw (string_info->string);

	return hash_value;
}

jr_int jr_ST_StringCmp (void_arg_1, void_arg_2)
	const void *				void_arg_1;
	const void *				void_arg_2;
{
	const jr_ST_StringType *	string_info_1	= void_arg_1;
	const jr_ST_StringType *	string_info_2	= void_arg_2;
	jr_int						diff;

	diff		= strcmp (string_info_1->string, string_info_2->string);

	if (diff != 0) {
		return diff;
	}

	return 0;
}
