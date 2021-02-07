#ifndef __jr_log_h___
#define __jr_log_h___

#include "ezport.h"

#include <stdio.h>

#include "jr/alist.h"
#include "jr/nettype.h"
#include "jr/log.h"
#include "jr/apr.h"


typedef struct {
	char *			file_name;
	jr_file_t		file_info[1];
	jr_int          opened_how;

	jr_int			block_size;
	jr_ULong		start_time;

	jr_int			in_transaction;
	jr_AList		transaction_stack [1];

	jr_ULong		truncate_offset;

	unsigned 		is_file_synced				: 1;
	unsigned 		is_executing				: 1;
} jr_LogType;


#define jr_LogFileName(log_info)				((log_info)->file_name)
#define jr_LogFileIsSynced(log_info)			((log_info)->is_file_synced)
#define jr_LogOpenedReadOnly(log_info)			((log_info)->opened_how == 'r')
#define jr_LogGetStartTime(log_info, ulong_ref)	jr_ULongAsgn( ulong_ref, (log_info)->start_time)
#define jr_LogIsInTransaction(log_info)			((log_info)->in_transaction > 0)
#define jr_LogBlockSize(log_info)				((log_info)->block_size)
#define jr_LogIsExecuting(log_info)				((log_info)->is_executing)


#define jr_LOG_GENERIC_ERROR					-1
#define jr_LOG_CORRUPTED_ERROR					-2


extern jr_LogType *	jr_LogCreate(void);

extern void			jr_LogInit(
						jr_LogType *			log_info);

extern jr_int		jr_LogOpen					PROTO ((
						jr_LogType *			log_info,
						const char *			file_name,
						const char *			how_string,
						jr_int					block_size,
						char *					error_buf
					));

extern void			jr_LogDestroy				PROTO ((
						jr_LogType *			log_info
					));

extern void			jr_LogUndo					PROTO ((
						jr_LogType *			log_info
					));


extern jr_int		jr_LogSize					PROTO ((
						jr_LogType *			log_info,
						jr_ULong *				file_size_ref,
						char *					error_buf
					));

extern jr_int		jr_LogGetOffset(
						jr_LogType *	log_info,
						jr_ULong *		curr_offset_ref,
						char *			error_buf);

extern jr_int		jr_LogTransactionBegin		PROTO ((
						jr_LogType *			log_info,
						char *					opt_error_buf
					));

extern jr_int		jr_LogTransactionCommit		PROTO ((
						jr_LogType *			log_info,
						char *					opt_error_buf
					));

extern jr_int		jr_LogTransactionAbort		PROTO ((
						jr_LogType *			log_info,
						char *					opt_error_buf
					));


extern jr_int		jr_LogSync					PROTO ((
						jr_LogType *			log_info,
						char *					opt_error_buf
					));

extern jr_int		jr_LogSyncAllEntries		PROTO ((
						jr_LogType *			log_info,
						char *					opt_error_buf
					));


extern jr_int		jr_LogExecuteEntries		PROTO ((
						jr_LogType *			log_info,
						jr_int					execution_style,
						jr_ULong				start_value,
						jr_ULong				stop_value,
						jr_int					(*entry_fn) (),
						void *					entry_fn_arg,
						jr_ULong *				opt_offset_ref,
						char *          		opt_error_buf
					));

#define				jr_LOG_EXECUTE_BY_OFFSET					0x00000001
#define				jr_LOG_EXECUTE_BY_TIME						0x00000002
#define				jr_LOG_TRUNCATE_INCOMPLETE_TRANSACTION		0x00000004
#define				jr_LOG_EXECUTE_TO_TRANSACTION_END			0x00000008
#define				jr_LOG_EXECUTE_NEXT_ENTRY					0x00000010

/*
 * entry_fn return values, negative terminates the execution
 */
#define				jr_LOG_STOP_IF_NOT_IN_TRANSACTION		1



#define				jr_LogExecuteEntriesByOffset(												\
						log_info, start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb	\
					)																			\
					jr_LogExecuteEntries (														\
						log_info, jr_LOG_EXECUTE_BY_OFFSET,										\
						start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb					\
					)


#define				jr_LogExecuteEntriesByOffsetAndTruncate(									\
						log_info, start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb	\
					)																			\
					jr_LogExecuteEntries (														\
						log_info,																\
						jr_LOG_EXECUTE_BY_OFFSET												\
							| jr_LOG_TRUNCATE_INCOMPLETE_TRANSACTION,							\
						start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb					\
					)


#define				jr_LogExecuteTransactionByOffset(											\
						log_info, start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb	\
					)																			\
					jr_LogExecuteEntries (														\
						log_info, 																\
						jr_LOG_EXECUTE_BY_OFFSET												\
							| jr_LOG_EXECUTE_TO_TRANSACTION_END,								\
						start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb		\
					)

#define				jr_LogExecuteTransactionByOffsetAndTruncate(								\
						log_info, start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb	\
					)																			\
					jr_LogExecuteEntries (														\
						log_info, 																\
						jr_LOG_EXECUTE_BY_OFFSET												\
							| jr_LOG_EXECUTE_TO_TRANSACTION_END									\
							| jr_LOG_TRUNCATE_INCOMPLETE_TRANSACTION,							\
						start_offset, stop_offset, entry_fn, fn_arg, opt_off_ref, opt_eb		\
					)

#define				jr_LogExecuteNextTransactionByOffset(										\
						log_info, start_offset, entry_fn, fn_arg, opt_off_ref, opt_eb			\
					)																			\
					jr_LogExecuteEntries (														\
						log_info, 																\
						jr_LOG_EXECUTE_BY_OFFSET												\
							| jr_LOG_EXECUTE_TO_TRANSACTION_END									\
							| jr_LOG_EXECUTE_NEXT_ENTRY,										\
						start_offset, start_offset, entry_fn, fn_arg, opt_off_ref, opt_eb		\
					)

#define				jr_LogExecuteNextTransactionByOffsetAndTruncate(							\
						log_info, start_offset, entry_fn, fn_arg, opt_off_ref, opt_eb			\
					)																			\
					jr_LogExecuteEntries (														\
						log_info, 																\
						jr_LOG_EXECUTE_BY_OFFSET												\
							| jr_LOG_EXECUTE_TO_TRANSACTION_END									\
							| jr_LOG_TRUNCATE_INCOMPLETE_TRANSACTION							\
							| jr_LOG_EXECUTE_NEXT_ENTRY,										\
						start_offset, start_offset, entry_fn, fn_arg, opt_off_ref, opt_eb		\
					)



#define				jr_LogExecuteEntriesByTime(													\
						log_info, start_time, stop_time, entry_fn, fn_arg, opt_off_ref, opt_eb	\
					)																			\
					jr_LogExecuteEntries (														\
						log_info, jr_LOG_EXECUTE_BY_TIME,										\
						start_time, stop_time, entry_fn, fn_arg, opt_off_ref, opt_eb			\
					)

#define				jr_LogExecuteEntriesByTimeAndTruncate(										\
						log_info, start_time, stop_time, entry_fn, fn_arg, opt_off_ref, opt_eb	\
					)																			\
					jr_LogExecuteEntries (														\
						log_info,																\
						jr_LOG_EXECUTE_TO_TRANSACTION_END										\
							| jr_LOG_EXECUTE_BY_TIME											\
							| jr_LOG_TRUNCATE_INCOMPLETE_TRANSACTION,							\
						start_time, stop_time, entry_fn, fn_arg, opt_off_ref, opt_eb			\
					)


extern jr_int		jr_LogGetStartTimeFromFileName	PROTO ((
						char *						log_file_name,
						jr_ULong *					start_time_ref,
						char *          			error_buf
					));

extern jr_int		jr_LogSetFileSyncBit		PROTO ((
						jr_LogType *			log_info,
						jr_int					synced_bit,
						char *					opt_error_buf
					));

extern jr_int		jr_LogSetStartTime			PROTO ((
						jr_LogType *			log_info,
						jr_ULong				start_time,
						char *					error_buf
					));

extern jr_int		jr_AvoidLogBlockBoundary	PROTO ((
						jr_file_t *				file_info,
						jr_int					block_size,
						jr_int					log_entry_length,
						jr_ULong *				opt_end_offset_ref,
						char *					opt_error_buf
					));

#define				jr_LogPadToBlockBoundary(li, end_offset_ref, opt_error_buf)						\
					jr_AvoidLogBlockBoundary (														\
						(li)->file_info, (li)->block_size, jr_INT_MAX, end_offset_ref, opt_error_buf	\
					)

extern off_t		jr_NextLogEntryOffset		PROTO ((
						off_t					curr_end_offset,
						jr_int					log_entry_length,
						jr_int					block_size
					));

#define				jr_LogNextEntryOffset(log_info, ceo, lel)				\
					jr_NextLogEntryOffset (ceo, lel, (log_info)->block_size)


extern jr_int		jr_LogDeleteRange			PROTO ((
						jr_LogType *			log_info,
						jr_ULong				start_offset,
						jr_ULong				end_offset,
						char *					opt_error_buf
					));

extern jr_int		jr_LogGetResetOffset		PROTO ((
						jr_LogType *			log_info,
						jr_ULong				start_time,
						jr_ULong*				reset_offset_ref,
						char *					error_buf
					));

extern jr_int		jr_LogSetTruncateOffset		PROTO ((
						jr_LogType *			log_info,
						char *					error_buf
					));

extern jr_int		jr_LogTruncate				PROTO ((
						jr_LogType *			log_info,
						jr_ULong				truncate_offset,
						char *					opt_error_buf
					));

extern jr_int		jr_LogResetFromOffset		PROTO ((
						jr_LogType *			log_info,
						jr_ULong				new_start_time,
						jr_ULong				reset_offset,
						char *					error_buf
					));

#define				jr_LogReset(log_info, eb)	jr_LogResetFromOffset (log_info, LONG_MAX, eb)

/******** Log Entry ********/

#define jr_LOG_ENTRY_MAX_VALUES			256
		/*
		 * 256 because the log entry header has 1 char for the number of values
		 * The limits on the number of int and var values should be
		 * published in the documentation, and will not be checked.
		 */

typedef struct {
	jr_int			type_number;
	jr_ULong		entry_time;
	jr_ULong		entry_offset;

	jr_int			num_int_values;
	jr_int			num_ptr_values;

	jr_NetInt		int_values		[jr_LOG_ENTRY_MAX_VALUES];
	const void *	ptr_values		[jr_LOG_ENTRY_MAX_VALUES];
	jr_NetInt		data_lengths	[jr_LOG_ENTRY_MAX_VALUES];

	unsigned 		requires_sync		: 1;
	unsigned 		has_malloced_ptrs	: 1;
} jr_LogEntryType;


		/*
		 * Built-in entry type numbers.  User defined entry type
		 * number should be positive.
		 */
#define jr_LOG_HEADER_ENTRY					-1
#define jr_LOG_PAD_ENTRY					-2
#define jr_LOG_TRANSACTION_BEGIN_ENTRY		-10
#define jr_LOG_TRANSACTION_COMMIT_ENTRY		-11
#define jr_LOG_TRANSACTION_ABORT_ENTRY		-12


#define jr_LogEntryGetTime(entry_info, ulong_ref)	jr_ULongAsgn( ulong_ref, (entry_info)->entry_time)
#define jr_LogEntryTypeNumber(entry_info)			((entry_info)->type_number)
#define jr_LogEntryGetOffset(entry_info, ulong_ref)	jr_ULongAsgn( ulong_ref, (entry_info)->entry_offset)
#define jr_LogEntryRequiresSync(entry_info)			((entry_info)->requires_sync)

#define jr_LogEntryIntValue(entry_info, index)						\
		jr_NetIntToHost ((entry_info)->int_values[index])

#define jr_LogEntrySetTime(entry_info, v)		((entry_info)->entry_time = (v))
#define jr_LogEntrySetIntValue(entry_info, index, value)			\
		jr_NetIntAsgnInt ((entry_info)->int_values + (index), value)

#define jr_LogEntryPtrValue(entry_info, index)						\
		((entry_info)->ptr_values[index])

extern void		jr_LogEntrySetPtrValue			PROTO ((
					jr_LogEntryType *			entry_info,
					jr_int						index,
					const void *				ptr_value,
					jr_int						data_length
				));

#define jr_LogEntryDataLength(entry_info, index)					\
		jr_NetIntToHost ((entry_info)->data_lengths[index])


#define jr_LogEntryNumInts(entry_info)			((entry_info)->num_int_values)
#define jr_LogEntryNumPtrs(entry_info)			((entry_info)->num_ptr_values)


extern void		jr_LogEntryInit					PROTO ((
					jr_LogEntryType *			entry_info,
					jr_int						type_number,
					jr_int						num_int_values,
					jr_int						num_ptr_values,
					jr_ULong					entry_time,
					jr_int						requires_sync
				));

extern void		jr_LogEntryUndo					PROTO ((
					jr_LogEntryType *			entry_info
				));

extern jr_int	jr_LogEntrySavedLength			PROTO ((
					jr_LogEntryType *			entry_info
				));

extern jr_int	jr_LogEntryAddToFilePtr			PROTO ((
					jr_LogEntryType *			entry_info,
					jr_file_t *					file_info,
					jr_int						block_size,
					char *          			opt_error_buf
				));

extern jr_int	jr_LogAddEntry					PROTO((
					jr_LogType *				log_info,
					jr_LogEntryType *			entry_info,
					char *          			opt_error_buf
				));




/******** Functions For Testing ********/

/*
 * simulate a crash
 */

extern void		jr_LogCrash						PROTO ((
					jr_LogType *				log_info
				));

/*
 * to be used for testing purposes only.
 * changing the block size on a log with data will
 * effectively make the log unusable
 */
extern jr_int	jr_LogSetFileBlockSize			PROTO ((
					jr_LogType *				log_info,
					jr_int						block_size,
					char *						error_buf
				));

extern jr_int	jr_LogFlushIO(
					jr_LogType *				log_info,
					char *						error_buf);

#define jr_LogCurrOffset(log_info)				ftell ((log_info)->fp)

#endif
