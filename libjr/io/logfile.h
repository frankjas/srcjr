#ifndef __log_proj_h___
#define __log_proj_h___

#define _POSIX_SOURCE 1

#include "ezport.h"

#include <errno.h>
#include <string.h>

#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/log.h"
#include "jr/io.h"
#include "jr/syscalls.h"
#include "jr/time.h"
#include "jr/error.h"

extern jr_int	jr_LogEntryInitFromFilePtr		PROTO ((
					jr_LogEntryType *			entry_info,
					jr_file_t *					file_info,
					jr_int						block_size,
					char *						opt_error_buf
				));

#define			jr_LogEntryInitFromLog(entry_info, li, opt_error_buf)	\
				jr_LogEntryInitFromFilePtr (entry_info, (li)->file_info, (li)->block_size, opt_error_buf)

#define			jr_LogEntrySetOffset(entry_info, v)		\
				jr_ULongAsgn(&(entry_info)->entry_offset, (v))


extern jr_int	jr_LogHeaderEntryLength		PROTO ((void));


/******** Log File Data Structures ********/

#define jr_LOG_MAGIC_NUMBER		0x5a5b5a5b

typedef struct {
	jr_NetInt		magic_number;	
	jr_NetInt		is_synced;		
	jr_NetInt		block_size;				/* store the block size in case log file is moved */
} jr_LogHeaderStruct;


typedef struct {
	jr_NetInt		magic_number;	
    jr_NetShort		type_number;    
    jr_NetChar		num_int_values;    
	jr_NetChar		num_ptr_values;
    jr_NetULong		entry_time;
} jr_LogEntryHeaderStruct;
/*
 * in the log, the entry header is followed by
 * an array of ints, num_int_values long, and
 * an array of ints, num_ptr_values long, and
 * then the variable data pieces, one after another
 */

extern void		jr_LogEntryHeaderInit			PROTO ((
					jr_LogEntryHeaderStruct *	entry_header,
					jr_int						type_number,
					jr_int						num_int_values,
					jr_int						num_ptr_values,
					jr_ULong					entry_time
				));


/******** Transaction Support Data Structures ********/

typedef struct {
	jr_ULong	begin_offset;
	unsigned	requires_sync			: 1;
} jr_LogTransInfoType;


extern void		jr_LogTransInfoInit				PROTO ((
					jr_LogTransInfoType *		trans_info,
					jr_ULong					begin_offset
				));

extern void		jr_LogTransInfoUndo				PROTO ((
					jr_LogTransInfoType *		trans_info
				));

#define jr_LogTransInfoSetSyncBit(trans_info, v)	((trans_info)->requires_sync = (v))

#endif
