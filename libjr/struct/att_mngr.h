#ifndef __att_mngr_h_
#define __att_mngr_h_

#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"
#include "jr/string.h"
#include "jr/misc.h"

#include "jr/attribut.h"


extern jr_int		jr_AttributeManagerGetListId	PROTO ((
						jr_AttributeManagerType *	manager
					));

extern void			jr_AttributeManagerAddAttribute	PROTO ((
						jr_AttributeManagerType *	manager,
						jr_AttributeType *		attribute_ptr
					));

extern jr_AttributeType *jr_AttributeManagerFindAttribute	PROTO ((
						jr_AttributeManagerType *	manager,
						jr_int						list_id,
						const char *				name_string
					));

extern void			jr_AttributeManagerDeleteAttribute	PROTO ((
						jr_AttributeManagerType *	manager,
						jr_AttributeType *		attribute_ptr
					));


typedef struct {
	char *		string;
	jr_int		num_references;
	jr_int		mask_value;
} jr_StringRefsStruct;


extern void			jr_StringRefsInit				PROTO ((
						jr_StringRefsStruct *		string_info,
						const char *				string
					));

extern void			jr_StringRefsUndo				PROTO ((
						jr_StringRefsStruct *		string_info
					));

extern jr_int		jr_StringRefsHash				PROTO ((
						const void *				void_arg_1
					));

extern jr_int		jr_StringRefsCmp				PROTO ((
						const void *				void_arg_1,
						const void *				void_arg_2
					));


extern jr_int		jr_AttributeHash				PROTO ((
						const void *				void_arg_1
					));

extern jr_int		jr_AttributeCmp					PROTO ((
						const void *				void_arg_1,
						const void *				void_arg_2
					));

#endif
