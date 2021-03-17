#ifndef __esis_lib_h___
#define __esis_lib_h___

#include "ezport.h"

#include <stdio.h>

#include "jr/alist.h"
#include "jr/llist.h"
#include "jr/htable.h"
#include "jr/attribut.h"



#define jr_ESIS_ILLEGAL_INPUT_ERROR				-1;
#define jr_ESIS_NOT_IMPLEMENTED_ERROR			-2;
#define jr_ESIS_NOT_CONFORMING_ERROR			-3;

#define jr_ESIS_EMBEDDED_NEWLINE	(jr_INT_MIN + 1)
#define jr_ESIS_SDATA_BRACKET_CHAR	(jr_INT_MIN + 2)


/******** Parsing Support ********/

extern jr_int	jr_ESIS_getc						PROTO ((
					FILE *							rfp
				));

extern jr_int	jr_ESIS_putc						PROTO ((
					jr_int							c,
					FILE *							wfp
				));

extern char *	jr_ESIS_ReadWordWithIsLast			PROTO ((
					FILE *							rfp,
					jr_AList *						char_list,
					jr_int *						is_last_ptr
				));

extern char *	jr_ESIS_SaveWordWithIsLast			PROTO ((
					FILE *							rfp,
					jr_AList *						char_list,
					jr_int *						is_last_ptr
				));

#define			jr_ESIS_ReadWord(rfp, char_list)	jr_ESIS_ReadWordWithIsLast (rfp, char_list, 0)
#define			jr_ESIS_SaveWord(rfp, char_list)	jr_ESIS_SaveWordWithIsLast (rfp, char_list, 0)

extern char *	jr_ESIS_ReadData					PROTO ((
					FILE *							rfp,
					jr_AList *						char_list
				));

extern char *	jr_ESIS_SaveData					PROTO ((
					FILE *							rfp,
					jr_AList *						char_list
				));


/******** Tree Support ********/


typedef struct {
	jr_LList					data_list [1];
} jr_ESIS_ElementList;

typedef struct {
	jr_AList					data_list [1];
} jr_ESIS_ElementStack;


typedef struct {
	char *						name_str;
	jr_ESIS_ElementList			element_list [1];
} jr_ESIS_GenericElementType;

typedef struct {
	jr_AttributeList			attr_list[1];
	jr_ESIS_ElementList			sub_element_list [1];
	void *						parent_element_ptr;

	jr_AList					data_list[1];
} jr_ESIS_ElementType;


typedef struct {
	char *						attr_name_str;
	char *						opt_generic_name_str;
	jr_HTable					attr_value_table[1];
} jr_ESIS_IndexType;


typedef struct {
	char *						attr_value_str;
	jr_ESIS_ElementList			matching_element_list[1];
} jr_ESIS_IndexValueType;



/******* ESIS Tree ********/

typedef struct {
	jr_AttributeManagerType *	attr_manager;

	jr_HTable					generic_element_table[1];

	jr_ESIS_ElementList			element_list[1];
	jr_ESIS_ElementStack		element_stack[1];
} jr_ESIS_TreeType;


extern void			jr_ESIS_TreeInit					PROTO ((
						jr_ESIS_TreeType *				tree_info,
						jr_AttributeManagerType *		attr_manager
					));

extern void			jr_ESIS_TreeUndo					PROTO ((
						jr_ESIS_TreeType *				tree_info
					));

extern jr_ESIS_ElementType *jr_ESIS_TreeNewElementPtr	PROTO ((
						jr_ESIS_TreeType *				tree_info,
						jr_ESIS_ElementType *			parent_element_ptr
					));


#define	jr_ESIS_TreeStackTop(tree_info)				\
		jr_ESIS_ElementStackTop ((tree_info)->element_stack)

#define	jr_ESIS_TreeStackPop(tree_info)				\
		jr_ESIS_ElementStackPop ((tree_info)->element_stack)

#define	jr_ESIS_TreeStackPush(tree_info, v)			\
		jr_ESIS_ElementStackPush ((tree_info)->element_stack, v)



/******** Element Stack ********/

#define			jr_ESIS_ElementStackPush(stack, el_ptr)				\
				jr_AListNativeSetNewTail ((stack)->data_list, el_ptr, jr_ESIS_ElementType *)

#define			jr_ESIS_ElementStackTop(stack)						\
				jr_AListNativeTail ((stack)->data_list, jr_ESIS_ElementType *)

#define			jr_ESIS_ElementStackPop(stack)						\
				jr_AListNativeDeletedTail ((stack)->data_list, jr_ESIS_ElementType *)


#endif
