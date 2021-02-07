#include "logfile.h"

void jr_LogCrash (log_info)
	jr_LogType *	log_info;
{
	jr_file_undo( log_info->file_info);
	jr_file_init( log_info->file_info);
}

jr_int jr_LogSize(
	jr_LogType *	log_info,
	jr_ULong *		file_size_ref,
	char *			error_buf)
{
	jr_int			status;
	jr_ULong		file_size;

	jr_ULongAsgnUInt( &file_size, 0);

	status		= jr_file_seek( log_info->file_info, file_size, jr_FILE_SEEK_END, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't seek: %s", error_buf);
		return -1;
	}

	status		= jr_file_get_offset( log_info->file_info, &file_size, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get offset: %s", error_buf);
		return -1;
	}

	jr_ULongAsgn( file_size_ref, file_size);

	return 0;
}

jr_int jr_LogGetOffset(
	jr_LogType *	log_info,
	jr_ULong *		curr_offset_ref,
	char *			error_buf)
{
	jr_int			status;
	jr_ULong		curr_offset;

	status		= jr_file_get_offset( log_info->file_info, &curr_offset, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get offset: %s", error_buf);
		return -1;
	}

	jr_ULongAsgn( curr_offset_ref, curr_offset);

	return 0;
}

jr_int jr_LogFlushIO(
	jr_LogType *	log_info,
	char *			error_buf)
{
	jr_int			status;

	status		= jr_file_flush( log_info->file_info, error_buf);

	if (status != 0) {
		return -1;
	}
	return 0;
}

jr_int jr_LogGetStartTimeFromFileName(
	char *						log_file_name,
	jr_ULong *					start_time_ref,
    char *          			error_buf)
{
	jr_LogEntryType				entry_info[1];
	jr_file_t					file_info[1];

	jr_int						status;


	jr_file_init( file_info);

	status	= jr_file_open( file_info, log_file_name, jr_FILE_OPEN_READ, 0, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't file for reading: %s", error_buf);
		goto return_status;
	}

	status	= jr_LogEntryInitFromFilePtr (
				entry_info, file_info, jr_LogHeaderEntryLength (), error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't read log header: %s", error_buf);
		goto return_status;
	}

	jr_LogEntryGetTime (entry_info, start_time_ref);

	jr_LogEntryUndo (entry_info);

	status	= 0;

	return_status : {
		jr_file_undo( file_info);
	}
	return status;
}


jr_int jr_LogSetFileSyncBit (log_info, synced_bit, opt_error_buf)
	jr_LogType *			log_info;
	jr_int					synced_bit;
	char *					opt_error_buf;
{
	jr_LogEntryType			entry_info[1];
	jr_LogHeaderStruct *	log_header;

	jr_ULong				old_offset;
	jr_ULong				tmp_ulong;

	jr_int					status;


	if (jr_LogOpenedReadOnly (log_info)) {
		jr_esprintf (opt_error_buf, "not opened for writing");
		return -1;
	}

	status	= jr_file_get_offset( log_info->file_info, &old_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf( opt_error_buf, "couldn't get offset: %s", opt_error_buf);
		return -1;
	}

	jr_ULongAsgnUInt( &tmp_ulong, 0);

	status = jr_file_seek( log_info->file_info, tmp_ulong, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}

	status = jr_LogEntryInitFromLog (entry_info, log_info, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't read log header: %s", opt_error_buf);
		return -1;
	}

	log_header = (void *) jr_LogEntryPtrValue (entry_info, 0);

	jr_NetIntAsgnInt (&log_header->is_synced, synced_bit);

	jr_ULongAsgnUInt( &tmp_ulong, 0);

	status = jr_file_seek( log_info->file_info, tmp_ulong, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}


	status	= jr_LogEntryAddToFilePtr (
				entry_info, log_info->file_info, log_info->block_size, opt_error_buf
			);

	jr_LogEntryUndo (entry_info);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't write log header: %s", opt_error_buf);
		return -1;
	}

	log_info->is_file_synced = synced_bit;

	status = jr_file_seek( log_info->file_info, old_offset, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}


	return 0;
}


jr_int jr_LogSetStartTime (log_info, start_time, opt_error_buf)
	jr_LogType *			log_info;
	jr_ULong				start_time;
	char *					opt_error_buf;
{
	jr_LogEntryType			entry_info[1];
	jr_ULong				old_offset;
	jr_ULong				tmp_ulong;

	jr_int					status;


	if (jr_LogOpenedReadOnly (log_info)) {
		jr_esprintf (opt_error_buf, "not opened for writing");
		return -1;
	}

	status	= jr_file_get_offset( log_info->file_info, &old_offset, opt_error_buf);

	if (status != 0) {
		jr_esprintf( opt_error_buf, "couldn't get offset: %s", opt_error_buf);
		return -1;
	}

	jr_ULongAsgnUInt( &tmp_ulong, 0);

	status = jr_file_seek( log_info->file_info, tmp_ulong, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}

	status = jr_LogEntryInitFromLog (entry_info, log_info, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't read log header: %s", opt_error_buf);
		return -1;
	}

	jr_LogEntrySetTime (entry_info, start_time);

	status = jr_file_seek( log_info->file_info, tmp_ulong, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}


	status	= jr_LogEntryAddToFilePtr (
				entry_info, log_info->file_info, log_info->block_size, opt_error_buf
			);

	jr_LogEntryUndo (entry_info);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't write log header: %s", opt_error_buf);
		return -1;
	}

	log_info->start_time = start_time;

	status = jr_file_seek( log_info->file_info, old_offset, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf (opt_error_buf, "couldn't seek: %s", opt_error_buf);
		return -1;
	}

	return 0;
}


jr_int jr_LogGetResetOffset (log_info, start_time, reset_offset_ref, error_buf)
	jr_LogType *			log_info;
	jr_ULong				start_time;
	jr_ULong*				reset_offset_ref;
	char *					error_buf;
{
	jr_LogEntryType			entry_info[1];
	jr_LogHeaderStruct		log_header [1];
	jr_ULong				reset_offset;
	jr_int					status;


	status = jr_LogPadToBlockBoundary (log_info, &reset_offset, error_buf);

	if (status != 0) {
		return -1;
	}

	jr_NetIntAsgnInt (&log_header->magic_number,	jr_LOG_MAGIC_NUMBER);
	jr_NetIntAsgnInt (&log_header->is_synced,		0);
	jr_NetIntAsgnInt (&log_header->block_size,		log_info->block_size);

	jr_LogEntryInit (
		entry_info, jr_LOG_HEADER_ENTRY, 0, 1, start_time, 0
	);

	jr_LogEntrySetPtrValue (entry_info, 0, log_header, sizeof (log_header));

	status	= jr_LogEntryAddToFilePtr (
				entry_info, log_info->file_info, log_info->block_size, error_buf
			);

	jr_LogEntryUndo (entry_info);

	if (status != 0) {
		return -1;
	}

	jr_ULongAsgn( reset_offset_ref, reset_offset);

	return 0;
}




jr_int jr_LogResetFromOffset (log_info, new_start_time, reset_offset, error_buf)
	jr_LogType *			log_info;
	jr_ULong				new_start_time;
	jr_ULong				reset_offset;
	char *					error_buf;
{
	jr_ULong				curr_start_time;
	jr_ULong				tmp_ulong;

	jr_int					status;


	jr_LogGetStartTime (log_info, &curr_start_time);
	
	/*
	 * Reset the start time first, so if it fails, the log is still usable
	 */
	status 	= jr_LogSetStartTime (log_info, new_start_time, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't reset start time: %s", error_buf);
		return jr_LOG_GENERIC_ERROR;
	}

	jr_ULongAsgnUInt( &tmp_ulong, 0);

	status	= jr_LogDeleteRange (log_info, tmp_ulong, reset_offset, error_buf);

	if (status != 0) {
		if (status == jr_LOG_CORRUPTED_ERROR) {
			return status;;
		}

		/*
		 * try to restore the start time, so that the log is still usable
		 */
		status 	= jr_LogSetStartTime (log_info, curr_start_time, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't reset start time: %s", error_buf);
			return jr_LOG_CORRUPTED_ERROR;
		}
		return jr_LOG_GENERIC_ERROR;
	}

	return 0;
}

jr_int jr_LogTruncate (log_info, truncate_offset, opt_error_buf)
	jr_LogType *			log_info;
	jr_ULong				truncate_offset;
	char *					opt_error_buf;
{
	jr_int					status;

	status	= jr_file_truncate( log_info->file_info, truncate_offset, opt_error_buf);

	if (status != 0) {
		return status;
	}

	status	= jr_file_seek( log_info->file_info, truncate_offset, jr_FILE_SEEK_SET, opt_error_buf);

	if (status != 0) {
		jr_esprintf( opt_error_buf, "couldn't seek: %s", opt_error_buf);
	}

	return 0;
}
