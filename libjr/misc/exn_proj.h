#ifndef __exn_proj_h___
#define __exn_proj_h___

#define _POSIX_SOURCE 1

#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"
#include "jr/trace.h"
#include "jr/misc.h"
#include "jr/vector.h"
#include "jr/alist.h"
#include "jr/exceptn.h"


extern jr_ExceptionType *	jr_ExceptionCreate		PROTO ((char *error_buf));
extern void					jr_ExceptionDestroy		PROTO ((jr_ExceptionType *	exception_ptr));


typedef struct {
	jr_AList			jmp_buf_list [1];

	unsigned			core_dump : 1;
} jr_ExceptionStack;


extern jr_ExceptionStack *jr_ExceptionStackCreate	PROTO ((void));

extern void			jr_ExceptionStackInit			PROTO ((
							jr_ExceptionStack *	exception_stack
));

extern void			jr_ExceptionStackDestroy		PROTO ((
							jr_ExceptionStack *	exception_stack
));

extern void			jr_ExceptionStackUndo			PROTO ((
							jr_ExceptionStack *	exception_stack
));

extern jr_ExceptionType *	jr_ExceptionStackPush				PROTO ((
								jr_ExceptionStack *	exception_stack,
								jr_ExceptionType *	exception_ptr
));

extern jr_ExceptionStack *	jr_ExceptionStackPop			PROTO ((
								jr_ExceptionStack *	exception_stack
							));


#endif
