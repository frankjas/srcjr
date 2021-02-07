#ifndef __strtab_h___
#define __strtab_h___

#define _POSIX_SOURCE	1

#include "ezport.h"

#include <assert.h>

#include "jr/strtab.h"
#include "jr/string.h"
#include "jr/misc.h"
#include "jr/malloc.h"


extern void				jr_StringTableEntryInit			PROTO ((
							jr_StringTableEntryType *	str_entry_ptr,
							const char *				string
						));

extern void				jr_StringTableEntryUndo			PROTO ((
							jr_StringTableEntryType *	str_entry_ptr
						));

extern jr_int			jr_StringTableEntryCmp			PROTO ((
							const void *				void_ptr_1,
							const void *				void_ptr_2
						));

extern jr_int			jr_StringTableEntryHash			PROTO ((
							const void *				void_ptr
						));


#endif
