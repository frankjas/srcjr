#ifndef _htable_proj_h__
#define _htable_proj_h__

#include "ezport.h"

#include <assert.h>
#include <stdarg.h>

#include "jr/htable.h"
#include "jr/malloc.h"
#include "jr/misc.h"
#include "jr/prefix.h"

extern void jr_HTableFreeElement			PROTO ((jr_HTable *htable, jr_int entry));

extern void jr_HTableIncreaseNumBuckets		PROTO ((jr_HTable *htable));
extern void jr_HTableAddToBucketList		PROTO ((jr_HTable *htable, jr_int index));

extern jr_int	jr_HTableAllocateNewIndex		PROTO ((jr_HTable *		htable));

extern void			jr_HTableDeleteFromBucketList		PROTO ((
						jr_HTable *						htable,
						jr_int							delete_index
					));

extern void			jr_HTableAddToDeleteList			PROTO ((
						jr_HTable *						htable,
						jr_int							delete_index
					));
#endif
