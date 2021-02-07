#include "logfile.h"

jr_LogType *jr_LogCreate()
{
	jr_LogType *			log_info		= jr_malloc (sizeof (jr_LogType));

	jr_LogInit( log_info);

	return log_info;
}

void jr_LogInit(
	jr_LogType *			log_info)
{
	memset (log_info, 0, sizeof (*log_info));

	jr_file_init( log_info->file_info);
	jr_AListInit (log_info->transaction_stack, sizeof (jr_LogTransInfoType));
}


jr_int jr_LogOpen(
	jr_LogType *			log_info,
	const char *			file_name,
	const char *			how_string,
	jr_int					block_size,
	char *					error_buf)
{
	jr_int					open_flags			= jr_FILE_OPEN_BUFFERED | jr_FILE_OPEN_READ;
	jr_int					file_lock			= 0;

	jr_ULong				file_size;
	jr_ULong				tmp_ulong;
	jr_ULong				start_time;
	jr_int					is_synced;

	jr_int					status;

	
	switch (how_string[0]) {
		default			: 
		case 'r'		: {
			file_lock	= jr_FILE_LOCK_READ;
			break;
		}
		case 'w'		: {
			open_flags	|= jr_FILE_OPEN_WRITING;
			file_lock	= jr_FILE_LOCK_WRITE;
			break;
		}
		case 'a'		: {
			open_flags	|= jr_FILE_OPEN_APPENDING;
			file_lock	= jr_FILE_LOCK_WRITE;
			break;
		}
	}

	status = jr_file_open (log_info->file_info, file_name, open_flags, jr_PERM_RWX_OWNER, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't open file: %s", error_buf);
		goto return_status;
	}

	status = jr_file_lock (log_info->file_info, file_lock, 0, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't lock log file: %s", error_buf);
		goto return_status;
	}

	jr_ULongAsgnUInt( &tmp_ulong, 0);

	status		= jr_file_seek( log_info->file_info,  tmp_ulong, jr_FILE_SEEK_END, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't seek: %s", error_buf);
		goto return_status;
	}

	status	= jr_file_get_offset( log_info->file_info, &file_size, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get offset: %s", error_buf);
		goto return_status;
	}
		
	if (jr_ULongIsZero( file_size) &&  how_string[0] != 'r') {
		jr_LogEntryType			entry_info[1];
		jr_LogHeaderStruct		log_header [1];

		if (block_size  == 0) {
			jr_esprintf (error_buf, "block size can't be zero");
			status = jr_MISUSE_ERROR;
			goto return_status;
		}
		is_synced	= 1;
		jr_ULongAsgnUInt( &start_time, jr_time_seconds());

		jr_NetIntAsgnInt (&log_header->magic_number,	jr_LOG_MAGIC_NUMBER);
		jr_NetIntAsgnInt (&log_header->is_synced,		is_synced);
		jr_NetIntAsgnInt (&log_header->block_size,		block_size);

		jr_LogEntryInit (
			entry_info, jr_LOG_HEADER_ENTRY, 0, 1, start_time, 0
		);

		jr_LogEntrySetPtrValue (entry_info, 0, log_header, sizeof (log_header));

		status = jr_LogEntryAddToFilePtr (entry_info, log_info->file_info, block_size, error_buf);

		jr_LogEntryUndo (entry_info);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't add log header: %s", error_buf);
			goto return_status;
		}
	}
	else {
		jr_LogEntryType				entry_info[1];
		const jr_LogHeaderStruct *	log_header;
		jr_int						seek_whence;

		jr_ULongAsgnUInt( &tmp_ulong, 0);

		status		= jr_file_seek( log_info->file_info,  tmp_ulong, jr_FILE_SEEK_SET, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't seek: %s", error_buf);
			goto return_status;
		}

		if (block_size  == 0) {
			jr_esprintf (error_buf, "block size can't be zero");
			status = jr_MISUSE_ERROR;
			goto return_status;
		}
		if (block_size  &&  block_size  <  jr_LogHeaderEntryLength ()) {
			jr_esprintf (error_buf, "block size %d  <  minimum of %d",
				block_size, jr_LogHeaderEntryLength ()
			);
			status = jr_MISUSE_ERROR;
			goto return_status;
		}

		status	= jr_LogEntryInitFromFilePtr (
					entry_info, log_info->file_info, block_size, error_buf
				);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't read log header: %s", error_buf);
			goto return_status;
		}

		log_header = jr_LogEntryPtrValue (entry_info, 0);

		if (jr_NetIntToHost (log_header->magic_number)  !=  jr_LOG_MAGIC_NUMBER) {
			jr_esprintf (error_buf, "bad magic number");
			jr_LogEntryUndo (entry_info);
			status = jr_INTERNAL_ERROR;
			goto return_status;
		}

		if (block_size != jr_NetIntToHost (log_header->block_size)) {
			jr_esprintf (error_buf, "desired block size %d  !=  current block size %d",
				block_size, jr_NetIntToHost (log_header->block_size)
			);
			jr_LogEntryUndo (entry_info);
			status = jr_INTERNAL_ERROR;
			goto return_status;
		}

		block_size	= jr_NetIntToHost( log_header->block_size);
		is_synced	= jr_NetIntToHost( log_header->is_synced);

		jr_LogEntryGetTime( entry_info, &start_time);


		jr_LogEntryUndo (entry_info);

		if (how_string[0] == 'a') {
			seek_whence	= jr_FILE_SEEK_END;
		}
		else {
			seek_whence	= jr_FILE_SEEK_CUR;
			/*
			 * so that we may do fwrite()s after the fread() above
			 */
		}
		jr_ULongAsgnUInt( &tmp_ulong, 0);

		status		= jr_file_seek( log_info->file_info,  tmp_ulong, seek_whence, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't seek: %s", error_buf);
			goto return_status;
		}
	}

	log_info->block_size 			= block_size;
	jr_ULongAsgn( &log_info->start_time, start_time);
	log_info->file_name				= jr_strdup (file_name);
	log_info->opened_how			= how_string[0];

	log_info->is_file_synced		= is_synced;

	jr_ULongAsgn( &log_info->truncate_offset, file_size);

	log_info->in_transaction		= 0;

	jr_AListInit (log_info->transaction_stack, sizeof (jr_LogTransInfoType));

	return 0;

	return_status : {
		return status;
	}
}


void jr_LogDestroy (log_info)
	jr_LogType *	log_info;
{
	jr_LogUndo (log_info);

	jr_free (log_info);
}


void jr_LogUndo (log_info)
	jr_LogType *			log_info;
{
	jr_LogTransInfoType *	trans_info;


	jr_file_undo( log_info->file_info);

	if (log_info->file_name) {
		jr_free (log_info->file_name);
	}

	jr_AListForEachElementPtr (log_info->transaction_stack, trans_info) {
		jr_LogTransInfoUndo (trans_info);
	}
	jr_AListUndo (log_info->transaction_stack);
}

void jr_LogEntryInit (
	jr_LogEntryType *			entry_info,
	jr_int						type_number,
	jr_int						num_int_values,
	jr_int						num_ptr_values,
	jr_ULong					entry_time,
	jr_int						requires_sync)
{
	entry_info->type_number				= type_number;
	entry_info->num_int_values			= num_int_values;
	entry_info->num_ptr_values			= num_ptr_values;

	jr_ULongAsgn( &entry_info->entry_time, entry_time);

	entry_info->requires_sync			= requires_sync;
	entry_info->has_malloced_ptrs		= 0;
}

void jr_LogEntryUndo (entry_info)
	jr_LogEntryType *		entry_info;
{
	jr_int		i;

	if (entry_info->has_malloced_ptrs) {
		for (i=0;  i < entry_info->num_ptr_values;  i++) {
			jr_free (entry_info->ptr_values[i]);
		}
	}
}

void jr_LogEntrySetPtrValue (entry_info, index, ptr_value, data_length)
	jr_LogEntryType *		entry_info;
	jr_int					index;
	const void *			ptr_value;
	jr_int					data_length;
{
	entry_info->ptr_values[index]	= ptr_value;

	jr_NetIntAsgnInt (entry_info->data_lengths + index, data_length);
}

jr_int jr_LogEntrySavedLength (entry_info)
	jr_LogEntryType *		entry_info;
{
	jr_int						total_entry_length;
	jr_int						var_data_length;
	jr_int						i;

	for (var_data_length = 0, i = 0;  i < entry_info->num_ptr_values;  i++) {
		var_data_length += jr_NetIntToHost (entry_info->data_lengths [i]);
	}

	total_entry_length = 0;
	total_entry_length += sizeof (jr_LogEntryHeaderStruct);
	total_entry_length += entry_info->num_int_values * sizeof (jr_NetInt);
	total_entry_length += entry_info->num_ptr_values * sizeof (jr_NetInt);	/* for data lengths */
	total_entry_length += var_data_length;

	return total_entry_length;
}

jr_int jr_LogHeaderEntryLength ()
{
	jr_LogEntryType			entry_info[1];
	jr_LogHeaderStruct		log_header [1];
	jr_ULong				tmp_ulong;

	jr_ULongAsgnUInt( &tmp_ulong, 0);

	jr_LogEntryInit (
		entry_info, jr_LOG_HEADER_ENTRY, 0, 1, tmp_ulong, 0
	);

	jr_LogEntrySetPtrValue (entry_info, 0, log_header, sizeof (log_header));

	return jr_LogEntrySavedLength (entry_info);
}


void jr_LogEntryHeaderInit(
	jr_LogEntryHeaderStruct *	entry_header,
	jr_int						type_number,
	jr_int						num_int_values,
	jr_int						num_ptr_values,
	jr_ULong					entry_time)
{
	jr_NetIntAsgnInt		(&entry_header->magic_number,	jr_LOG_MAGIC_NUMBER);
    jr_NetShortAsgnShort	(&entry_header->type_number,	type_number);
	jr_NetCharAsgnInt		(&entry_header->num_int_values, num_int_values);
	jr_NetCharAsgnInt		(&entry_header->num_ptr_values, num_ptr_values);

	jr_NetULongAsgnULong	(&entry_header->entry_time,		entry_time);
}


jr_int jr_LogAddEntry (log_info, entry_info, opt_error_buf)
	jr_LogType *				log_info;
	jr_LogEntryType *			entry_info;
    char *          			opt_error_buf;
{
	jr_int						status;


	/*
	 * can't fseek() to the end each time because fseek()
	 * calls fflush()
	 */

	if (jr_LogOpenedReadOnly (log_info)) {
		jr_esprintf (opt_error_buf, "not opened for writing");
		return -1;
	}

	if (jr_LogFileIsSynced (log_info)) {
		status = jr_LogSetFileSyncBit (log_info, 0, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't clear synced bit: %s", opt_error_buf);
			return -1;
		}
	}

	if (log_info->is_executing) {
		jr_esprintf (opt_error_buf, "can't add entries while log is executing");
		return -1;
	}

	status	= jr_LogEntryAddToFilePtr(
				entry_info, log_info->file_info, log_info->block_size, opt_error_buf
			);

	if (status != 0) {
		return -1;
	}

	if (entry_info->requires_sync) {
		if (jr_LogIsInTransaction (log_info)) {
			jr_LogTransInfoType *		trans_info;

			trans_info = jr_AListTailPtr (log_info->transaction_stack);

			jr_LogTransInfoSetSyncBit (trans_info, 1);
		}
		else {
			status = jr_file_flush( log_info->file_info, opt_error_buf);

			if (status != 0) {
				jr_esprintf (opt_error_buf, "couldn't sync entry: flush failed: %s", opt_error_buf);
				return status;
			}

			status = jr_file_sync( log_info->file_info, opt_error_buf);

			if (status != 0) {
				jr_esprintf (opt_error_buf, "couldn't sync entry: %s", opt_error_buf);
				return -1;
			}
		}
	}

    return 0;
}

jr_int jr_LogEntryAddToFilePtr(
	jr_LogEntryType *			entry_info,
	jr_file_t *					file_info,
	jr_int						block_size,
    char *          			error_buf)
{
    jr_int						num_bytes;
    jr_int						w_bytes;
	jr_int						total_entry_length;

	jr_LogEntryHeaderStruct		entry_header[1];

	jr_int						i;
	jr_int						data_length;
	jr_int						status;


	jr_LogEntryHeaderInit (entry_header,
		entry_info->type_number,
		entry_info->num_int_values,
		entry_info->num_ptr_values,
		entry_info->entry_time
	);

	total_entry_length	= jr_LogEntrySavedLength (entry_info);

	if (block_size) {
		/*
		 * if the entry doesn't fit on the block, pad to the block boundary
		 */
		status = jr_AvoidLogBlockBoundary (file_info, block_size, total_entry_length, 0, error_buf);

		if (status < 0) {
			return -1;
		}
	}

	num_bytes		= 0;
	w_bytes	= sizeof( jr_LogEntryHeaderStruct);

	status		= jr_file_write( file_info, entry_header, w_bytes, error_buf);

	if (status != w_bytes) {
		if (status < 0) {
			jr_esprintf( error_buf, "write error: %s", error_buf);
		}
		else {
			jr_esprintf( error_buf, "only wrote %d of %d bytes", status, w_bytes);
			status	= jr_IO_ERROR;
		}
		return status;
	}
	num_bytes	+= status;

	w_bytes	= entry_info->num_int_values * sizeof (jr_NetInt);

	status		= jr_file_write( file_info, entry_info->int_values, w_bytes, error_buf);

	if (status != w_bytes) {
		if (status < 0) {
			jr_esprintf( error_buf, "write error: %s", error_buf);
		}
		else {
			jr_esprintf( error_buf, "only wrote %d of %d bytes", status, w_bytes);
			status	= jr_IO_ERROR;
		}
		return status;
	}
	num_bytes	+= status;


	w_bytes	= entry_info->num_ptr_values * sizeof (jr_NetInt);
	status		= jr_file_write( file_info, entry_info->data_lengths, w_bytes, error_buf);

	if (status != w_bytes) {
		if (status < 0) {
			jr_esprintf( error_buf, "write error: %s", error_buf);
		}
		else {
			jr_esprintf( error_buf, "only wrote %d of %d bytes", status, w_bytes);
			status	= jr_IO_ERROR;
		}
		return status;
	}

	num_bytes	+= status;

	for (i=0;  i < entry_info->num_ptr_values;  i++) {
		data_length	= jr_NetIntToHost (entry_info->data_lengths[i]);

		if (entry_info->ptr_values[i]) {
			w_bytes	= data_length;
			status	= jr_file_write( file_info, entry_info->ptr_values[i], w_bytes, error_buf);

			if (status != w_bytes) {
				if (status < 0) {
					jr_esprintf( error_buf, "write error: %s", error_buf);
				}
				else {
					jr_esprintf( error_buf, "only wrote %d of %d bytes", status, w_bytes);
					status	= jr_IO_ERROR;
				}
				return status;
			}
		}
		else {
			jr_ULong		tmp_ulong;

			jr_ULongAsgnUInt( &tmp_ulong, data_length);

			status		= jr_file_seek( file_info,  tmp_ulong, jr_FILE_SEEK_CUR, error_buf);

			if (status != 0) {
				jr_esprintf (error_buf, "couldn't seek: %s", error_buf);
				return status;
			}
		}
		num_bytes	+= data_length;
	}

    if (num_bytes != total_entry_length) {
        jr_esprintf (error_buf, "only wrote %d of %d bytes", num_bytes, total_entry_length);
        return -1;
    }
	
	return 0;
}


jr_int jr_LogEntryInitFromFilePtr(
	jr_LogEntryType *			entry_info,
	jr_file_t *					file_info,
	jr_int						block_size,
	char *						opt_error_buf)
{
	jr_LogEntryHeaderStruct		entry_header [1];
	jr_int						r_bytes;

	jr_ULong					curr_offset;
	jr_int						i;

	jr_ULong					tmp_ulong;

	jr_int						read_size;
	jr_int						status;


	while (1) {
		/*
		 * skip to next block boundary if there isn't enough room
		 * for a whole entry
		 */
		status	= jr_file_get_offset( file_info, &curr_offset, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't get offset: %s", opt_error_buf);
			return status;
		}
			

		if (block_size) {
			jr_int						space_in_block;

			space_in_block	= block_size - jr_ULongModUInt( curr_offset, block_size);

			if (space_in_block < (jr_int) sizeof (jr_LogEntryHeaderStruct)) {
				/*
				** Using SEEK_CUR on an odd offset, incrementing by an odd value
				** sometimes causes fseek() to return -1, errno is 0, under Next 3.3.
				*/
				jr_ULongAddUInt( &curr_offset, curr_offset, space_in_block);

				status = jr_file_seek (file_info, curr_offset, jr_FILE_SEEK_SET, opt_error_buf);

				if (status != 0) {
					jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
					return status;
				}
			}
		}

		read_size	= sizeof (jr_LogEntryHeaderStruct);
		status = jr_file_read( file_info, entry_header, read_size, opt_error_buf);

		if (status != read_size) {
			if (status < 0) {
				jr_esprintf (opt_error_buf, "error reading log entry: %s", opt_error_buf);
			}
			else if (status == 0) {
				jr_esprintf (opt_error_buf, "end of file");
				status = jr_EOF_ERROR;
			}
			else {
				jr_esprintf (opt_error_buf, "read %d bytes of %d requested", status, read_size);
				status	= jr_IO_ERROR;
			}
			return status;
		}

		if (jr_NetShortToHost (entry_header->type_number) ==  jr_LOG_PAD_ENTRY) {
			/*
			 * this is padding, time field tells how much padding, should put
			 * us at the next block boundary.
			 */
			jr_ULong		tmp_ulong;
			jr_ULong		padding_size;

			jr_ULongAsgnNetULong (&tmp_ulong, entry_header->entry_time);

			jr_ULongSubtractUInt( &padding_size, tmp_ulong, sizeof(jr_LogEntryHeaderStruct));

			status = jr_file_seek( file_info, padding_size, jr_FILE_SEEK_CUR, opt_error_buf);

			if (status != 0) {
				jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
				return status;
			}
			continue;
		}
		break;
	}

	if (jr_NetIntToHost (entry_header->magic_number)  !=  jr_LOG_MAGIC_NUMBER) {
		jr_esprintf (opt_error_buf, "bad magic number");
		return jr_INTERNAL_ERROR;
	}

	jr_ULongAsgnNetULong (&tmp_ulong, entry_header->entry_time);

	jr_LogEntryInit (
		entry_info,
		jr_NetShortToHost (entry_header->type_number),
		(unsigned) jr_NetCharToHost (entry_header->num_int_values),
		(unsigned) jr_NetCharToHost (entry_header->num_ptr_values),
		tmp_ulong,
		0 /* requires_sync */
	);

	jr_LogEntrySetOffset (entry_info, curr_offset);

	r_bytes	= entry_info->num_int_values * sizeof (jr_NetInt),

	status	= jr_file_read( file_info, entry_info->int_values, r_bytes, opt_error_buf);

	if (status < r_bytes) {
		if (status < 0) {
			jr_esprintf (opt_error_buf, "error reading log entry: %s", opt_error_buf);
		}
		else if (status == 0) {
			jr_esprintf (opt_error_buf, "premature end of file");
			status = jr_INTERNAL_ERROR;
		}
		else {
			jr_esprintf (opt_error_buf, "read %d bytes of %d requested", status, r_bytes);
			status	= jr_IO_ERROR;
		}
		return status;
	}

	r_bytes 	= entry_info->num_ptr_values * sizeof (jr_NetInt),
	status		= jr_file_read( file_info, entry_info->data_lengths, r_bytes, opt_error_buf);

	if (status < r_bytes) {
		if (status < 0) {
			jr_esprintf (opt_error_buf, "error reading log entry: %s", opt_error_buf);
		}
		else if (status == 0) {
			jr_esprintf (opt_error_buf, "premature end of file");
			status = jr_INTERNAL_ERROR;
		}
		else {
			jr_esprintf (opt_error_buf, "read %d bytes of %d requested", status, r_bytes);
			status	= jr_IO_ERROR;
		}
		return status;
	}


	for (i=0;  i < entry_info->num_ptr_values;  i++) {
		r_bytes					= jr_NetIntToHost (entry_info->data_lengths[i]);
		entry_info->ptr_values[i]		= jr_malloc (r_bytes);
		entry_info->has_malloced_ptrs	= 1;

		status	= jr_file_read( file_info, (void *) entry_info->ptr_values[i], r_bytes, opt_error_buf);

		if (status < r_bytes) {
			entry_info->num_ptr_values = i+1;
			jr_LogEntryUndo (entry_info);

			if (status < 0) {
				jr_esprintf (opt_error_buf, "error reading log entry: %s", opt_error_buf);
			}
			else if (status == 0) {
				jr_esprintf (opt_error_buf, "premature end of file");
				status = jr_INTERNAL_ERROR;
			}
			else {
				jr_esprintf (opt_error_buf, "read %d bytes of %d requested", status, r_bytes);
				status	= jr_IO_ERROR;
			}
			return status;
		}
	}

	return 0;
}


jr_int jr_LogExecuteEntries(
	jr_LogType *			log_info,
	jr_int					execution_style,
	jr_ULong				start_value,
	jr_ULong				stop_value,
	jr_int					(*entry_fn) (),
	void *					entry_fn_arg,
	jr_ULong *				opt_offset_ref,
    char *          		opt_error_buf)
{
	jr_ULong				start_offset;
	jr_ULong				stop_offset;
	jr_ULong				start_time;
	jr_ULong				stop_time;

	jr_int					num_executed_entries		= 0;
	jr_int					stop_if_not_in_transaction	= 0;

	jr_int					execute_by_time;
	jr_int					truncate_incomplete_transaction;
	jr_int					execute_transaction;
	jr_int					execute_next_entry;

	jr_LogEntryType			entry_info [1];

	jr_ULong				last_valid_offset;
	jr_int					in_transaction;

	jr_ULong				curr_offset;
	jr_ULong				old_offset;

	jr_ULong				curr_time;

	jr_int					status;


	jr_ULongAsgn( &start_offset, start_value);
	jr_ULongAsgn( &stop_offset, stop_value);

	jr_ULongAsgn( &start_time, start_value);
	jr_ULongAsgn( &stop_time, stop_value);

	jr_ULongAsgnUInt( &curr_time, 0);

	execute_by_time						= execution_style & jr_LOG_EXECUTE_BY_TIME;
	truncate_incomplete_transaction		= execution_style & jr_LOG_TRUNCATE_INCOMPLETE_TRANSACTION;
	execute_transaction					= execution_style & jr_LOG_EXECUTE_TO_TRANSACTION_END;
	execute_next_entry					= execution_style & jr_LOG_EXECUTE_NEXT_ENTRY;

	status	= jr_file_get_offset( log_info->file_info, &old_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't get offset: %s", opt_error_buf);
		goto return_bad;
	}

	if (execute_by_time) {
		jr_ULongAsgnUInt( &start_offset, 0);
		jr_ULongAsgnMaxValue( &stop_offset);
	}
	if (execute_next_entry) {
		jr_ULongAsgnMaxValue( &stop_offset);
	}

	status = jr_file_seek( log_info->file_info, start_offset, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}

	status	= jr_file_get_offset( log_info->file_info, &curr_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't get offset: %s", opt_error_buf);
		goto return_bad;
	}

	jr_ULongAsgn( &last_valid_offset, curr_offset);

	in_transaction		= 0;

	log_info->is_executing	= 1;

	while (jr_ULongCmp( curr_offset, stop_offset) < 0  ||  execute_transaction && in_transaction) {

		if (	truncate_incomplete_transaction
			&&  jr_ULongCmp( curr_offset, log_info->truncate_offset) >= 0) {

			break;
		}
		if (	execute_next_entry
			&&  num_executed_entries > 0 
			&&	!(execute_transaction && in_transaction)) {

			break;
		}

		status = jr_LogEntryInitFromLog (entry_info, log_info, opt_error_buf);

		if (status == jr_EOF_ERROR) {
			break;
		}
		else if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't read log entry: %s", opt_error_buf);

			status = -1;
			goto return_bad;
		}

		status	= jr_file_get_offset( log_info->file_info, &curr_offset, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't get offset: %s", opt_error_buf);
			goto return_bad;
		}

		jr_ULongAsgn( &curr_time, entry_info->entry_time);

		if (	execute_by_time
			&&	!jr_ULongIsZero( entry_info->entry_time)
			&&  jr_ULongCmp( entry_info->entry_time, start_time) < 0) {

			jr_LogEntryUndo (entry_info);
			continue;
		}
		if (execute_by_time
				&&   !jr_ULongIsZero( stop_time)   &&  jr_ULongCmp( curr_time, stop_time) >= 0
				&&   !(execute_transaction && in_transaction)
			) {
			jr_LogEntryUndo (entry_info);
			break;
		}

		switch (entry_info->type_number) {
			case jr_LOG_TRANSACTION_BEGIN_ENTRY		: in_transaction++; break;
			case jr_LOG_TRANSACTION_COMMIT_ENTRY	: in_transaction--; break;
			case jr_LOG_HEADER_ENTRY				: {
				jr_LogEntryUndo (entry_info);
				continue;
			}
		}

		if (entry_fn) {
			status = (*entry_fn) (entry_fn_arg, entry_info, opt_error_buf);

			if (status < 0) {
				jr_LogEntryUndo (entry_info);
				goto return_bad;
			}
			if (status  ==  jr_LOG_STOP_IF_NOT_IN_TRANSACTION) {
				stop_if_not_in_transaction = 1;
			}
			else {
				stop_if_not_in_transaction = 0;
			}
		}
		jr_LogEntryUndo (entry_info);

		num_executed_entries ++;

		if (! in_transaction) {
			/*
			 * if we're in a transaction, wait until we see the end
			 * to decide if the transaction should be kept
			 */
			last_valid_offset	= curr_offset;

			if (stop_if_not_in_transaction) {
				break;
			}
		}
    }

	if (in_transaction  &&  truncate_incomplete_transaction) {
		if (entry_fn) {
			jr_ULong		tmp_ulong;

			jr_ULongAsgnUInt( &tmp_ulong, 0);

			/*
			 * If we stop in the middle of a transaction,
			 * abort the transaction we're in.
			 */
			jr_LogEntryInit (entry_info, jr_LOG_TRANSACTION_ABORT_ENTRY, 0, 0, tmp_ulong, 0);
			jr_LogEntrySetOffset (entry_info, curr_offset);

			status = (*entry_fn) (entry_fn_arg, entry_info, opt_error_buf);

			jr_LogEntryUndo (entry_info);

			if (status < 0) {
				goto return_bad;
			}
		}

		status	= jr_LogDeleteRange (							/*@ delete_partial @*/
					log_info, last_valid_offset, log_info->truncate_offset, opt_error_buf
				);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't remove incomplete transaction: %s",
				opt_error_buf
			);
			status = -1;
			goto return_bad;
		}
		curr_offset = last_valid_offset;
		/*
		 * Delete range leaves the fp at the new end of the log
		 */
	}
	else {
		status = jr_file_seek( log_info->file_info, old_offset, jr_FILE_SEEK_SET, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
			goto return_bad;
		}
	}

	log_info->is_executing = 0;

	if (opt_offset_ref) {
		jr_ULongAsgn( opt_offset_ref, curr_offset);
	}

	status = 0;

	return_bad : {
		if (status != 0) {
			jr_file_seek( log_info->file_info, old_offset, jr_FILE_SEEK_SET, 0);
			log_info->is_executing = 0;
		}
	}
	return status;
}


jr_int jr_LogSetTruncateOffset (log_info, error_buf)
	jr_LogType *			log_info;
	char *					error_buf;
{
	jr_ULong				truncate_offset;
	jr_int					status;

	status = jr_LogPadToBlockBoundary (log_info, &truncate_offset, error_buf);

	if (status < 0) {
		return status;
	}

	jr_ULongAsgn( &log_info->truncate_offset, truncate_offset);

	return 0;
}


jr_int jr_AvoidLogBlockBoundary(
	jr_file_t *				file_info,
	jr_int					block_size,
	jr_int					log_entry_length,
	jr_ULong *				opt_end_offset_ref,
	char *					opt_error_buf)
{
	jr_LogEntryHeaderStruct	entry_header [1];

	jr_ULong				curr_end_offset;
	jr_ULong				padding_size;

	jr_int					space_in_block;
	jr_int					w_bytes;
	jr_int					status;


	status = jr_file_get_offset( file_info, &curr_end_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf( opt_error_buf, "can't get file size: %s", opt_error_buf);
		return status;
	}

	if (block_size == 0  ||  jr_ULongModUInt( curr_end_offset, block_size)  ==  0) {
		if (opt_end_offset_ref) {
			jr_ULongAsgn( opt_end_offset_ref, curr_end_offset);
		}
		return 0;
	}

	space_in_block	= block_size - jr_ULongModUInt( curr_end_offset, block_size);

	if (space_in_block < log_entry_length) {
		/*
		 * no room on this block for the next entry
		 * create a pad entry which contains the number of bytes to the
		 * next entry in the entry_time field.
		 * If there is no room for the pad entry, then skip to the next block
		 * boundary.
		 */
		w_bytes = sizeof (jr_LogEntryHeaderStruct);

		if (w_bytes <= space_in_block) {
			jr_ULongAsgnUInt( &padding_size, space_in_block);
			jr_LogEntryHeaderInit (entry_header, jr_LOG_PAD_ENTRY, 0, 0, padding_size);

			status = jr_file_write( file_info, entry_header, w_bytes, opt_error_buf);

			if (status != w_bytes) {
				if (status < 0) {
					jr_esprintf( opt_error_buf, "write error: %s", opt_error_buf);
				}
				else {
					jr_esprintf( opt_error_buf, "only wrote %d of %d bytes", status, w_bytes);
					status	= jr_IO_ERROR;
				}
				return status;
			}
		}

		jr_ULongAddUInt( &curr_end_offset, curr_end_offset, space_in_block);

		status = jr_file_seek( file_info, curr_end_offset, jr_FILE_SEEK_SET, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
			return -1;
		}
	}
	if (opt_end_offset_ref) {
		jr_ULongAsgn( opt_end_offset_ref, curr_end_offset);
	}
	return 0;
}


off_t jr_NextLogEntryOffset (curr_end_offset, log_entry_length, block_size)
	off_t					curr_end_offset;
	jr_int					log_entry_length;
	jr_int					block_size;
{
	jr_int					space_in_block;


	if (block_size == 0  ||  curr_end_offset % block_size  ==  0) {
		return curr_end_offset;
	}

	space_in_block	= block_size - curr_end_offset % block_size;

	if (space_in_block < log_entry_length) {
		curr_end_offset += space_in_block;
	}
	return curr_end_offset;
}


jr_int jr_LogTransactionBegin (log_info, opt_error_buf)
	jr_LogType *			log_info;
	char *					opt_error_buf;
{
	jr_LogEntryType			entry_info [1];
	jr_LogTransInfoType *	trans_info;
	jr_ULong				curr_offset;
	jr_ULong				tmp_ulong;

	jr_int					status;


	status = jr_file_get_offset( log_info->file_info, &curr_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf( opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}

	jr_ULongAsgnUInt( &tmp_ulong, 0);
	jr_LogEntryInit (entry_info, jr_LOG_TRANSACTION_BEGIN_ENTRY, 0, 0, tmp_ulong, 0);

	status = jr_LogAddEntry (log_info, entry_info, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't log begin transaction request: %s", opt_error_buf);
		return -1;
	}

	log_info->in_transaction ++;

	trans_info = jr_AListNewTailPtr (log_info->transaction_stack);

	jr_LogTransInfoInit (trans_info,  curr_offset);

	return 0;
}


jr_int jr_LogTransactionCommit (log_info, opt_error_buf)
	jr_LogType *			log_info;
	char *					opt_error_buf;
{
	jr_LogEntryType			entry_info [1];
	jr_LogTransInfoType *	trans_info;

	jr_ULong				tmp_ulong;
	jr_int					status;


	if (!jr_LogIsInTransaction (log_info)) {
		jr_esprintf (opt_error_buf, "not in transaction");
		return -1;
	}

	log_info->in_transaction --;
	trans_info	= jr_AListDeletedTailPtr (log_info->transaction_stack);
	/*
	 * setup a new tail before the AddEntry() so that this commit
	 * is conceptually part of the outer transaction
	 */

	jr_ULongAsgnUInt( &tmp_ulong, 0);

	jr_LogEntryInit (
		entry_info, jr_LOG_TRANSACTION_COMMIT_ENTRY, 0, 0, tmp_ulong, trans_info->requires_sync
	);

	status = jr_LogAddEntry (log_info, entry_info, opt_error_buf);

	if (status != 0) {
		char			other_error_buf [jr_ERROR_LEN];

		status = jr_LogTruncate (log_info, trans_info->begin_offset, other_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't add log entry (%s), then couldn't truncate(): %s",
				opt_error_buf, other_error_buf
			);
			return -1;
		}

		jr_esprintf (opt_error_buf, "couldn't log transaction commit: %s", opt_error_buf);
		return -1;
	}

	return 0;
}


jr_int jr_LogTransactionAbort (log_info, opt_error_buf)
	jr_LogType *			log_info;
	char *					opt_error_buf;
{
	jr_LogTransInfoType *	trans_info;
	jr_int					status;


	if (!jr_LogIsInTransaction (log_info)) {
		jr_esprintf (opt_error_buf, "not in transaction");
		return -1;
	}

	log_info->in_transaction --;
	trans_info			= jr_AListDeletedTailPtr (log_info->transaction_stack);

	status = jr_LogTruncate (log_info, trans_info->begin_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't truncate log: %s", opt_error_buf);
		return -1;
	}

	return 0;
}

void jr_LogTransInfoInit(
	jr_LogTransInfoType *		trans_info,
	jr_ULong					begin_offset)
{
	jr_ULongAsgn( &trans_info->begin_offset, begin_offset);
	trans_info->requires_sync		= 0;
}

void jr_LogTransInfoUndo (trans_info)
	jr_LogTransInfoType *		trans_info;
{
}


jr_int jr_LogDeleteRange(
	jr_LogType *				log_info,
	jr_ULong					start_offset,
	jr_ULong					end_offset,
	char *						opt_error_buf)
{
	jr_ULong					eof_offset;
	jr_ULong					num_bytes;
	jr_int						read_size;
	void *						read_buffer;
	jr_int						wrote_bytes				= 1;

	jr_int						status;


	if (jr_ULongCmp( end_offset, start_offset) < 0) {
		jr_esprintf (opt_error_buf, "invalid range [%ld - %ld]",
			(long) jr_ULongToHost( start_offset), (long) jr_ULongToHost( end_offset)
		);
		return jr_LOG_GENERIC_ERROR;
	}
	if (jr_ULongCmp( start_offset, end_offset) == 0) {
		return 0;
	}

	if (	log_info->block_size
		&&		jr_ULongModUInt( start_offset, log_info->block_size)
			!=  jr_ULongModUInt( end_offset, log_info->block_size)
		) {
		/*
		** Relative block offsets are not equal. In this case
		** the end offset must either be a block boundary or the end of the file.
		*/
		jr_ULongAsgnUInt( &eof_offset, 0);

		status = jr_file_seek( log_info->file_info, eof_offset, jr_FILE_SEEK_END, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
			return jr_LOG_GENERIC_ERROR;
		}

		status	= jr_file_get_offset( log_info->file_info, &eof_offset, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't get offset: %s", opt_error_buf);
			return jr_LOG_GENERIC_ERROR;
		}

		if (jr_ULongCmp( end_offset, eof_offset) >= 0) {
			jr_ULongAsgn( &end_offset, eof_offset);
		}
		else {
			if (jr_ULongModUInt( end_offset, log_info->block_size)) {
				jr_esprintf (opt_error_buf, "end offset %u bytes past block boundary",
					jr_ULongModUInt( end_offset, log_info->block_size)
				);
				return jr_LOG_GENERIC_ERROR;
			}
			if (jr_ULongModUInt( start_offset, log_info->block_size)) {

				status = jr_file_seek(
							log_info->file_info, start_offset, jr_FILE_SEEK_SET, opt_error_buf
						);

				if (status != 0) {
					jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
					return jr_LOG_GENERIC_ERROR;
				}

				status = jr_LogPadToBlockBoundary (log_info, &start_offset, opt_error_buf);

				if (status < 0) {
					jr_esprintf (opt_error_buf, "couldn't pad %u bytes of start block: %s",
						jr_ULongModUInt( start_offset, log_info->block_size),
						opt_error_buf
					);
					return jr_LOG_GENERIC_ERROR;
				}
				wrote_bytes = 1;
			}
		}
	}

	read_size	= log_info->block_size;

	read_buffer	= jr_malloc (read_size);

	jr_ULongAsgnUInt( &eof_offset, 0);

	status = jr_file_seek( log_info->file_info, eof_offset, jr_FILE_SEEK_END, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		goto return_bad;
	}

	status	= jr_file_get_offset( log_info->file_info, &eof_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't get offset: %s", opt_error_buf);
		goto return_bad;
	}

	/*
	** 3-12-2013: copy from the end of the deleted section to the end of the file
	** over the start of the deleted section.
	*/
	jr_ULongSubtract( &num_bytes, eof_offset, end_offset);
	
	while (jr_ULongIsNonZero( num_bytes)) {
		status = jr_file_seek( log_info->file_info, end_offset, jr_FILE_SEEK_SET, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
			goto return_bad;
		}

		if (jr_ULongCmpUInt( num_bytes, read_size) < 0) {
			read_size = jr_ULongToHost( num_bytes);
		}

		status	= jr_file_read( log_info->file_info, read_buffer, read_size, opt_error_buf);

		if (status != read_size) {
			if (status < 0) {
				jr_esprintf (opt_error_buf, "error reading log entry: %s", opt_error_buf);
			}
			else {
				jr_esprintf (opt_error_buf, "read %d bytes of %d requested", status, read_size);
				status	= jr_IO_ERROR;
			}
			goto return_bad;
		}

		status = jr_file_seek( log_info->file_info, start_offset, jr_FILE_SEEK_SET, opt_error_buf);

		if (status != 0) {
			jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
			goto return_bad;
		}

		status = jr_file_write( log_info->file_info, read_buffer, read_size, opt_error_buf);

		if (status != read_size) {
			if (status < 0) {
				jr_esprintf( opt_error_buf, "write error: %s", opt_error_buf);
			}
			else {
				jr_esprintf( opt_error_buf, "only wrote %d of %d bytes", status, read_size);
				status	= jr_IO_ERROR;
			}
			goto return_bad;
		}
		wrote_bytes		= 1;

		jr_ULongSubtractUInt( &num_bytes, num_bytes, read_size);
		jr_ULongAddUInt( &start_offset, start_offset, read_size);
		jr_ULongAddUInt( &end_offset, end_offset, read_size);
	}

	status = jr_LogTruncate (log_info, start_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't truncate log: %s", opt_error_buf);
		goto return_bad;
	}


	if (0) return_bad : {
		if (wrote_bytes) {
			status = jr_LOG_CORRUPTED_ERROR;
		}
		else {
			status = jr_LOG_GENERIC_ERROR;
		}
	}
	else {
		status = 0;
	}

	jr_free (read_buffer);

	return status;
}

jr_int jr_LogSync (log_info, opt_error_buf)
	jr_LogType *			log_info;
	char *					opt_error_buf;
{
	jr_int					status;

	status = jr_LogSetFileSyncBit (log_info, 1, opt_error_buf);

	if (status != 0) {
		return -1;
	}

	status = jr_file_flush( log_info->file_info, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't flush: %s", opt_error_buf);
		return -1;
	}

	return 0;
}

jr_int jr_LogSyncAllEntries (log_info, opt_error_buf)
	jr_LogType *			log_info;
	char *					opt_error_buf;
{
	jr_int					status;

	status = jr_file_flush( log_info->file_info, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't flush: %s", opt_error_buf);
		return -1;
	}

	return 0;
}
