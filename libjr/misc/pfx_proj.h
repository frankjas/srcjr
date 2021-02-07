#ifndef __misc_prefix_h____
#define __misc_prefix_h____

#include "ezport.h"

#include <stdlib.h>

#include "jr/prefix.h"
#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/syscalls.h"
#include "jr/apr.h"

#include "jr/alist.h"


#define jr_PREFIX_MAX_TYPES			256


extern jr_int		jr_PrefixInfoElementPrefixSize		PROTO ((
						jr_PrefixInfoType *				prefix_info
					));

#endif
