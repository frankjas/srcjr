#ifndef __esis_proj_h___
#define __esis_proj_h___

#define _POSIX_SOURCE 1

#include "ezport.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/prefix.h"
#include "jr/misc.h"
#include "jr/esis/lib.h"


extern void		jr_ESIS_GenericElementInit			PROTO ((
					jr_ESIS_GenericElementType *	generic_info,
					char *							name_str
				));
				
extern void		jr_ESIS_GenericElementUndo			PROTO ((
					jr_ESIS_GenericElementType *	generic_info
				));

extern jr_int	jr_ESIS_GenericElementHash			PROTO ((
					const void *					void_arg_1
				));

extern jr_int	jr_ESIS_GenericElementCmp			PROTO ((
					const void *					void_arg_1,
					const void *					void_arg_2
				));


/******** Element ********/

extern void		jr_ESIS_ElementInit					PROTO ((
					jr_ESIS_ElementType *			element_ptr,
					jr_ESIS_TreeType *				esis_grove,
					jr_ESIS_ElementType *			parent_element_ptr
				));


extern void		jr_ESIS_ElementUndo					PROTO ((
					jr_ESIS_ElementType *			entity_info
				));

extern void		jr_ESIS_ElementSetData				PROTO ((
					jr_ESIS_ElementType *			element_ptr,
					char *							data_str
				));


extern void		jr_ESIS_ElementAddAttribute			PROTO ((
					jr_ESIS_ElementType *			element_ptr,
					char *							attr_name_str,
					char *							attr_value_str
				));

extern void		jr_ESIS_ElementAddSubElement		PROTO ((
					jr_ESIS_ElementType *			parent_element_ptr,
					jr_ESIS_ElementType *			element_ptr
				));


/******** Index ********/

extern void		jr_ESIS_IndexInit					PROTO ((
					jr_ESIS_IndexType *				esis_index,
					char *							attr_name_str,
					char *							opt_generic_name_str
				));

extern void		jr_ESIS_IndexUndo					PROTO ((
					jr_ESIS_IndexType *				esis_index
				));


/******** Index Value ********/

extern void		jr_ESIS_IndexValueInit				PROTO ((
					jr_ESIS_IndexValueType *		esis_index,
					char *							attr_value_str
				));

extern void		jr_ESIS_IndexValueUndo				PROTO ((
					jr_ESIS_IndexValueType *		esis_index_value
				));

extern jr_int	jr_ESIS_IndexValueHash				PROTO ((
					const void *					void_arg_1
				));

extern jr_int	jr_ESIS_IndexValueCmp				PROTO ((
					const void *					void_arg_1,
					const void *					void_arg_2
				));


/******** Element List ********/

extern void		jr_ESIS_ElementListInit				PROTO ((
					jr_ESIS_ElementList *			element_list
				));

extern void		jr_ESIS_ElementListUndo				PROTO ((
					jr_ESIS_ElementList *			element_list
				));

extern jr_ESIS_ElementType *jr_ESIS_ElementListNewElementPtr	PROTO ((
					jr_ESIS_ElementList *			element_list
				));

extern void		jr_ESIS_GlobalElementListInit		PROTO ((
					jr_ESIS_ElementList *			element_list
				));

extern void		jr_ESIS_GenericElementListInit		PROTO ((
					jr_ESIS_ElementList *			element_list
				));

extern void		jr_ESIS_SubElementListInit			PROTO ((
					jr_ESIS_ElementList *			element_list
				));

extern void		jr_ESIS_MatchingElementListInit		PROTO ((
					jr_ESIS_ElementList *			element_list
				));


#define			jr_ESIS_ElementListAppendToTail(list, el_ptr)							\
				jr_LListAppendElement (													\
					(list)->data_list, jr_LListTailPtr ((list)->data_list), el_ptr		\
				)

#define			jr_ForEachESIS_ElementListPtr(list, el_ptr)								\
				jr_LListForEachElementPtr ((list)->data_list, el_ptr)


/******* Element Stack ********/

extern void		jr_ESIS_ElementStackInit			PROTO ((
					jr_ESIS_ElementStack *			element_stack
				));

extern void		jr_ESIS_ElementStackUndo			PROTO ((
					jr_ESIS_ElementStack *			element_stack
				));

#endif
