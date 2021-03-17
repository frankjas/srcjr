#ifndef __jr_attribute_h___
#define __jr_attribute_h___

#include "ezport.h"

#include "jr/htable.h"
#include "jr/dlist.h"

typedef struct {
	jr_HTable					string_table[1] ;
	jr_HTable					attribute_table[1] ;

	jr_int						next_list_id;
} jr_AttributeManagerType ;


typedef struct {
	jr_AttributeManagerType *	manager ;
	jr_DList					list_of_pairs[1] ;
	jr_int						list_id;
} jr_AttributeList ;

typedef struct {
	char *						name_string ;
	char *						value_string ;
	jr_int						list_id;
} jr_AttributeType ;

extern jr_AttributeManagerType *jr_AttributeManagerCreate	PROTO((void)) ;

extern void				jr_AttributeManagerInit				PROTO((
							jr_AttributeManagerType *		attribute_manager
						)) ;

extern void				jr_AttributeManagerDestroy			PROTO((
							jr_AttributeManagerType *		attribute_manager
						)) ;

extern void				jr_AttributeManagerUndo				PROTO((
							jr_AttributeManagerType *		attribute_manager
						)) ;


extern const char *		jr_AttributeManagerAddString		PROTO((
							jr_AttributeManagerType *		attribute_manager,
							const char *					string_arg,
							unsigned jr_int					mask_arg
						)) ;

extern void				jr_AttributeManagerDeleteString		PROTO((
							jr_AttributeManagerType *		attribute_manager,
							const char *					string_arg
						)) ;


#define					jr_AttributeManagerAddNameString(mngr, string) \
						jr_AttributeManagerAddString (mngr, string, jr_ATTRIBUTE_NAME)

#define					jr_AttributeManagerAddValueString(mngr, string) \
						jr_AttributeManagerAddString (mngr, string, jr_ATTRIBUTE_VALUE)

#define					jr_AttributeManagerDeleteNameString(mngr, string) \
						jr_AttributeManagerDeleteString (mngr, string)

#define					jr_AttributeManagerDeleteValueString(mngr, string) \
						jr_AttributeManagerDeleteString (mngr, string)



/******** Attribute List ********/

extern jr_AttributeList *	jr_AttributeListCreate			PROTO((
								jr_AttributeManagerType *	attribute_manager
							)) ;

extern void					jr_AttributeListInit			PROTO((
								jr_AttributeList *			attribute_list,
								jr_AttributeManagerType *	attribute_manager
							)) ;

extern void					jr_AttributeListDestroy			PROTO((
								jr_AttributeList *			attribute_list
							)) ;

extern void					jr_AttributeListUndo			PROTO((
								jr_AttributeList *			attribute_list
							)) ;

extern void					jr_AttributeListAddAttribute	PROTO((
								jr_AttributeList *			list,
								const char *				name_string,
								const char *				value_string
							)) ;

const char *				jr_AttributeListGetValue		PROTO ((
								jr_AttributeList *			list,
								const char *				name_string
							));

extern void					jr_AttributeListDeleteAttribute	PROTO ((
								jr_AttributeList *			attribute_list,
								const char *				name_string
							));

extern void					jr_AttributeListEmpty			PROTO ((
								jr_AttributeList *			attribute_list
							));

#define jr_AttributeListForEachAttributePtr(attribute_list, attribute)	\
		jr_DListForEachElementPtr(attribute_list->list_of_pairs, attribute)


#define jr_ATTRIBUTE_NAME	0x00000010
#define jr_ATTRIBUTE_VALUE	0x00000020


#define jr_AttributeNameString(attr) \
		((attr)->name_string)

#define jr_AttributeValueString(attr) \
		((attr)->value_string)


#endif
