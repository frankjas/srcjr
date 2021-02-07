#include "ptrmap.h"


void jr_PtrMapInit (ptr_map, max_category_size, ptr_data_size)
	jr_PtrMapType *				ptr_map;
	jr_int						max_category_size;
	jr_int						ptr_data_size;
{
	memset (ptr_map, 0, sizeof (jr_PtrMapType));

	ptr_map->max_category_size	= max_category_size;
	ptr_map->ptr_data_size		= ptr_data_size;

	jr_AListInit (ptr_map->category_list, sizeof (jr_PtrCategoryType));
}

void jr_PtrMapUndo (ptr_map)
	jr_PtrMapType *				ptr_map;
{
	jr_PtrCategoryType *		ptr_category;

	jr_AListForEachElementPtr (ptr_map->category_list, ptr_category) {
		jr_PtrCategoryUndo (ptr_category);
	}
	jr_AListUndo (ptr_map->category_list);
}


jr_PtrMapType *jr_PtrMapCreate (max_category_size, ptr_data_size)
	jr_int						max_category_size;
	jr_int						ptr_data_size;
{
	jr_PtrMapType *				ptr_map					= jr_malloc (sizeof (jr_PtrMapType));

	jr_PtrMapInit (ptr_map, max_category_size, ptr_data_size);

	return ptr_map;
}


void jr_PtrMapDestroy (ptr_map)
	jr_PtrMapType *				ptr_map;
{
	jr_PtrMapUndo (ptr_map);
	jr_free (ptr_map);
}

jr_int jr_PtrMapAddNewPtrWithFileName (ptr_map, category_kind, ptr_value, file_name, line_number)
	jr_PtrMapType *				ptr_map;
	jr_int						category_kind;
	void *						ptr_value;
	const char *				file_name;
	jr_int						line_number;
{
	jr_PtrCategoryType *		ptr_category;


	while (jr_AListSize (ptr_map->category_list)  <=  category_kind) {
		ptr_category	= jr_AListNewTailPtr (ptr_map->category_list);

		jr_PtrCategoryInit (
			ptr_category,
			ptr_map->max_category_size * jr_AListTailIndex (ptr_map->category_list),
			ptr_map->max_category_size,
			ptr_map->ptr_data_size
		);
	}
	ptr_category	= jr_AListElementPtr (ptr_map->category_list, category_kind);

	return jr_PtrCategoryOpenPtr (ptr_category, ptr_value, file_name, line_number);
}


void jr_PtrCategoryInit (ptr_category, min_value, num_values, ptr_data_size)
	jr_PtrCategoryType *		ptr_category;
	jr_int						min_value;
	jr_int						num_values;
	jr_int						ptr_data_size;
{
	memset (ptr_category, 0, sizeof (jr_PtrCategoryType));

	ptr_category->min_value				= min_value;
	ptr_category->num_values			= num_values;
	ptr_category->next_deleted_desc		= -1;

	jr_AListInit (ptr_category->ptr_list, sizeof (jr_PtrInfoStruct)  +  ptr_data_size);
}


void jr_PtrCategoryUndo (ptr_category)
	jr_PtrCategoryType *		ptr_category;
{
	jr_AListUndo (ptr_category->ptr_list);
}


jr_int jr_PtrCategoryOpenPtr (ptr_category, ptr_value, file_name, line_number)
	jr_PtrCategoryType *		ptr_category;
	void *						ptr_value;
	const char *				file_name;
	jr_int						line_number;
{
	jr_PtrInfoStruct *			ptr_info;
	jr_int						ptr_desc;


	if (ptr_category->next_deleted_desc >= 0) {
		ptr_desc							= ptr_category->next_deleted_desc;

		ptr_category->next_deleted_desc		= jr_PtrCategoryNextDeletedDesc (ptr_category, ptr_desc);

		ptr_category->num_deletes--;
	}
	else {
		if (jr_AListSize (ptr_category->ptr_list)  >=  ptr_category->num_values) {
			return -1;
		}

		jr_AListNewTailPtr (ptr_category->ptr_list);
		ptr_desc		= ptr_category->min_value  +  jr_AListTailIndex (ptr_category->ptr_list);
	}

	ptr_info		= jr_PtrCategoryPtrInfo (ptr_category, ptr_desc);

	jr_PtrInfoInit (ptr_info, ptr_value, file_name, line_number);

	memset (
		jr_PtrCategoryDataPtr (ptr_category, ptr_desc), 0, jr_PtrCategoryDataPtrSize (ptr_category)
	);

	return ptr_desc;
}


void jr_PtrCategoryCloseDesc (ptr_category, ptr_desc)
	jr_PtrCategoryType *		ptr_category;
	jr_int						ptr_desc;
{
	if (!jr_PtrCategoryIsValidDesc (ptr_category, ptr_desc)) {
		return;
	}

	if (jr_PtrCategoryPtrValue (ptr_category, ptr_desc)  ==  0) {
		return;
	}

	jr_PtrCategorySetPtrValue (ptr_category, ptr_desc, 0);

	jr_PtrCategorySetNextDeletedDesc (ptr_category, ptr_desc, ptr_category->next_deleted_desc);
	ptr_category->next_deleted_desc		= ptr_desc;

	ptr_category->num_deletes++;
}


void *jr_PtrCategoryGetPtr (ptr_category, ptr_desc)
	jr_PtrCategoryType *		ptr_category;
	jr_int						ptr_desc;
{
	if (jr_PtrCategoryIsValidDesc (ptr_category, ptr_desc)) {
		return jr_PtrCategoryPtrValue (ptr_category, ptr_desc);
	}

	return 0;
}


void jr_PtrCategorySetPtr (ptr_category, ptr_desc, ptr_value)
	jr_PtrCategoryType *		ptr_category;
	jr_int						ptr_desc;
	void *						ptr_value;
{
	if (jr_PtrCategoryIsValidDesc (ptr_category, ptr_desc)) {
		jr_PtrCategorySetPtrValue (ptr_category, ptr_desc, ptr_value);
	}
}


const char *jr_PtrCategoryGetFileName (ptr_category, ptr_desc)
	jr_PtrCategoryType *		ptr_category;
	jr_int						ptr_desc;
{
	if (jr_PtrCategoryIsValidDesc (ptr_category, ptr_desc)) {
		return jr_PtrCategoryFileName (ptr_category, ptr_desc);
	}

	return 0;
}

jr_int jr_PtrCategoryGetLineNumber (ptr_category, ptr_desc)
	jr_PtrCategoryType *		ptr_category;
	jr_int						ptr_desc;
{
	if (jr_PtrCategoryIsValidDesc (ptr_category, ptr_desc)) {
		return jr_PtrCategoryLineNumber (ptr_category, ptr_desc);
	}

	return 0;
}

void *jr_PtrCategoryGetDataPtr (ptr_category, ptr_desc)
	jr_PtrCategoryType *		ptr_category;
	jr_int						ptr_desc;
{
	if (jr_PtrCategoryIsValidDesc (ptr_category, ptr_desc)) {
		return jr_PtrCategoryDataPtr (ptr_category, ptr_desc);
	}

	return 0;
}



void jr_PtrInfoInit (ptr_info, ptr_value, file_name, line_number)
	jr_PtrInfoStruct *			ptr_info;
	void *						ptr_value;
	const char *				file_name;
	jr_int						line_number;
{
	ptr_info->ptr_value				= ptr_value;
	ptr_info->next_deleted_desc		= -1;
	ptr_info->file_name				= file_name;
	ptr_info->line_number			= line_number;
}

void *_jr_PtrMapDataPtr (ptr_map, category_kind, ptr_desc)
	jr_PtrMapType *				ptr_map;
	jr_int						category_kind;
	jr_int						ptr_desc;
{
	return jr_PtrMapDataPtr (ptr_map, category_kind, ptr_desc);
}

	
