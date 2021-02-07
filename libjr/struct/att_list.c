#include "att_mngr.h"

void jr_AttributeListInit(attribute_list, manager)
	jr_AttributeList *			attribute_list ;
	jr_AttributeManagerType *	manager ;
{
	attribute_list->list_id		= jr_AttributeManagerGetListId (manager);
	attribute_list->manager		= manager ;

	jr_DListInit(attribute_list->list_of_pairs, sizeof(jr_AttributeType)) ;
}

void jr_AttributeListUndo(attribute_list)
	jr_AttributeList *			attribute_list ;
{
	jr_AttributeType *		attribute_ptr;

	jr_DListForEachElementPtr (attribute_list->list_of_pairs, attribute_ptr) {
		jr_AttributeManagerDeleteAttribute (attribute_list->manager, attribute_ptr);
	}

	jr_DListUndo(attribute_list->list_of_pairs) ;
}

jr_AttributeList *	jr_AttributeListCreate(manager)
	jr_AttributeManagerType *	manager ;
{
	jr_AttributeList *		new_attribute_list ;

	new_attribute_list = (jr_AttributeList *) jr_malloc(sizeof(jr_AttributeList)) ;

	jr_AttributeListInit(new_attribute_list, manager) ;

	return(new_attribute_list) ;
}

void jr_AttributeListDestroy(attribute_list)
	jr_AttributeList *			attribute_list ;
{
	jr_AttributeListUndo(attribute_list) ;
	jr_free(attribute_list) ;
}

void jr_AttributeListEmpty(attribute_list)
	jr_AttributeList *			attribute_list ;
{
	jr_AttributeType *		attribute_ptr;

	jr_DListForEachElementPtr (attribute_list->list_of_pairs, attribute_ptr) {
		jr_AttributeManagerDeleteAttribute (attribute_list->manager, attribute_ptr);
	}

	jr_DListEmpty (attribute_list->list_of_pairs) ;
}

void jr_AttributeListAddAttribute (attribute_list, name_string, value_string)
	jr_AttributeList *	attribute_list ;
	const char *		name_string ;
	const char *		value_string ;
{
	jr_AttributeType *			old_attribute ;
	jr_AttributeType *			new_attribute ;

	old_attribute	= jr_AttributeManagerFindAttribute (
						attribute_list->manager, attribute_list->list_id, name_string
					) ;

	if (old_attribute) {
		if (old_attribute->value_string) {
			jr_AttributeManagerDeleteValueString(
				attribute_list->manager, old_attribute->value_string
			) ;
			old_attribute->value_string	= 0;
		}
		if (value_string) {
			old_attribute->value_string	= (char *) jr_AttributeManagerAddValueString(
											attribute_list->manager, value_string
										) ;
		}
		new_attribute = old_attribute ;
	}
	else {
		jr_AttributeType			new_attribute_buf[1] ;
		const char *				new_string_value ;
		const char *				new_name_string ;

		new_name_string			= jr_AttributeManagerAddNameString(
									attribute_list->manager, name_string
								) ;
		new_string_value		= jr_AttributeManagerAddValueString(
									attribute_list->manager, value_string
								) ;

		new_attribute_buf->name_string			= (char *) new_name_string ;
		new_attribute_buf->value_string			= (char *) new_string_value ;
		new_attribute_buf->list_id				= attribute_list->list_id;

		new_attribute = jr_DListSetNewHead(attribute_list->list_of_pairs, new_attribute_buf) ;

		jr_AttributeManagerAddAttribute (attribute_list->manager, new_attribute);
	}
}

const char *	jr_AttributeListGetValue(list, name_string)
	jr_AttributeList *	list ;
	const char *		name_string ;
{
	jr_AttributeType *	attribute ;

	attribute	= jr_AttributeManagerFindAttribute(
						list->manager, list->list_id, name_string
					) ;

	if (attribute) {
		return attribute->value_string;
	}
	return 0;
}

void jr_AttributeListDeleteAttribute (attribute_list, name_string)
	jr_AttributeList *	attribute_list ;
	const char *		name_string ;
{
	jr_AttributeType *			old_attribute ;

	old_attribute	= jr_AttributeManagerFindAttribute (
						attribute_list->manager, attribute_list->list_id, name_string
					) ;

	if (old_attribute) {
		jr_DListDeleteElement (attribute_list->list_of_pairs, old_attribute);
	}
}
