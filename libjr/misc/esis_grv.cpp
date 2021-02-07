#include "ezport.h"

extern "C" {
#	include "esis_prj.h"
}

#include "jr/esis/grove.hpp"


void jr_ESIS_InputGroveInit (jr_ESIS_InputGroveClass *esis_grove)
{
	memset (esis_grove, 0, sizeof (jr_ESIS_InputGroveClass));

	esis_grove	= new (esis_grove) jr_ESIS_InputGroveClass;

	jr_ESIS_InputInit (esis_grove);
	jr_ESIS_TreeInit (esis_grove->tree_info, jr_ESIS_InputAttributeManager (esis_grove));
}

void jr_ESIS_InputGroveUndo (jr_ESIS_InputGroveClass *esis_grove)
{
	jr_ESIS_TreeUndo (esis_grove->tree_info);
	jr_ESIS_InputUndo (esis_grove);
}

jr_ESIS_InputGroveClass *jr_ESIS_InputGroveCreate ()
{
	jr_ESIS_InputGroveClass *		esis_grove;

	esis_grove		= (jr_ESIS_InputGroveClass *) jr_malloc (sizeof (jr_ESIS_InputGroveClass));

	jr_ESIS_InputGroveInit (esis_grove);

	return esis_grove;
}

void jr_ESIS_InputGroveDestroy (jr_ESIS_InputGroveClass *esis_grove)
{
	jr_ESIS_InputGroveUndo (esis_grove);
	jr_free (esis_grove);
}

void jr_ESIS_InputGroveClass::undo ()
{
	jr_ESIS_InputGroveUndo (this);
}


void jr_ESIS_InputGroveClass::destroy ()
{
	jr_ESIS_InputGroveDestroy (this);
}

void *jr_ESIS_InputGroveClass::operator new (size_t size, jr_ESIS_InputGroveClass *esis_grove)
{
	return esis_grove;
}


jr_int jr_ESIS_InputGroveClass::element_begin (
	char *						element_name,
	jr_int						keyword_number,
	char *						error_buf)
{
	jr_ESIS_ElementType *		element_ptr;
	jr_AttributeType *			attribute_ptr;


	element_ptr	= jr_ESIS_TreeNewElementPtr (
					this->tree_info, jr_ESIS_TreeStackTop (this->tree_info)
				);

	jr_ESIS_TreeStackPush (this->tree_info, element_ptr);

	jr_ESIS_InputForEachAttributePtr (this, attribute_ptr) {
		jr_ESIS_ElementAddAttribute (
			element_ptr,
			jr_AttributeNameString (attribute_ptr),
			jr_AttributeValueString (attribute_ptr)
		);
	}
	return 0;
}

jr_int jr_ESIS_InputGroveClass::element_end (
	char *						element_name,
	jr_int						keyword_number,
	char *						error_buf)
{
	(void) jr_ESIS_TreeStackPop (this->tree_info);

	return 0;
}

jr_int jr_ESIS_InputGroveClass::element_data (char *element_data, char *error_buf)
{
	jr_ESIS_ElementSetData (jr_ESIS_TreeStackTop (this->tree_info), element_data);

	return 0;
}

void jr_ESIS_InputGroveClass::set_is_conforming (jr_int value)
{
}

