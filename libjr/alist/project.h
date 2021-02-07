#ifndef __project_h___
#define __project_h___

#define _POSIX_SOURCE 1

#include "ezport.h"

#include <assert.h>
#include <stdarg.h>

#include "jr/alist.h"
#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/string.h"
#include "jr/prefix.h"

extern void			jr_AListTransformPrefixes	PROTO ((
						jr_AList *				list,
						void *					old_buffer_ptr,
						void					(*prefix_transform_fn) (),
						void *					prefix_transform_arg
					));


#endif
