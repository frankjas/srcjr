#define _POSIX_SOURCE 1

#include "ezport.h"

#include <errno.h>
#include <signal.h>

#include "jr/syscalls.h"
#include "jr/malloc.h"
#include "jr/memfile.h"
#include "jr/io.h"
#include "jr/error.h"


#ifndef has_broken_sigio_on_pipes

#include <unistd.h>

extern void				jr_MemoryFileIO_Handler			PROTO ((
							jr_int						rfd,
							jr_MemoryFileType *			mem_file_ptr
						));



jr_MemoryFileType *jr_MemoryFileCreateForWriting (max_buffer_size, error_buf)
	jr_int						max_buffer_size;
	char *						error_buf;
{
	jr_MemoryFileType *			mem_file_ptr		= 0;

	jr_int						fd_array[2];

	jr_int						status;

	/*
	 * No Init() function, since we store the pointer to the struct
	 * in the async handler support routines.
	 */

	mem_file_ptr		= jr_malloc (sizeof (*mem_file_ptr));

	memset (mem_file_ptr, 0, sizeof (*mem_file_ptr));


	jr_AListInit (mem_file_ptr->char_buffer, sizeof (char));

	mem_file_ptr->max_buffer_size	= max_buffer_size;
	mem_file_ptr->rfd				= -1;
	mem_file_ptr->wfd				= -1;


	status	= pipe (fd_array);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't get 'pipe': %s", strerror (errno));
		status = -1;
		goto return_status;
	}

	mem_file_ptr->rfd		= fd_array [0];
	mem_file_ptr->wfd		= fd_array [1];

	mem_file_ptr->fp		= fdopen (mem_file_ptr->wfd, "w");

	if (mem_file_ptr->fp == NULL) {
		jr_esprintf (error_buf, "couldn't fdopen(%d): %s", mem_file_ptr->wfd, strerror (errno));
		status = -1;
		goto return_status;
	}

	setvbuf (mem_file_ptr->fp, NULL, _IOLBF, 0);


	status	= jr_FileDescInitNonBlockingIO (mem_file_ptr->rfd, error_buf);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't init. non-blocking I/O: %s", error_buf);
		status = -1;
		goto return_status;
	}

	status	= jr_FileDescInitReadAsyncIO (
				mem_file_ptr->rfd, jr_MemoryFileIO_Handler, mem_file_ptr, error_buf
			);

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't init. async I/O: %s", error_buf);
		status = -1;
		goto return_status;
	}


	status = 0;

	return_status : {
		if (status != 0) {
			if (mem_file_ptr->rfd != -1) {
				close (mem_file_ptr->rfd);
			}
			if (mem_file_ptr->wfd != -1) {
				close (mem_file_ptr->wfd);
			}
			if (mem_file_ptr->fp) {
				fclose (mem_file_ptr->fp);
			}
			jr_AListUndo (mem_file_ptr->char_buffer);
			jr_free (mem_file_ptr);

			return 0;
		}
	}

	return mem_file_ptr;
}


const char *jr_MemoryFileOpenString (mem_file_ptr)
	jr_MemoryFileType *			mem_file_ptr;
{
	if (mem_file_ptr->old_mask_ptr == 0) {
		mem_file_ptr->old_mask_ptr		= jr_malloc (sizeof (sigset_t));
		mem_file_ptr->num_string_opens	= 0;

		jr_BlockAsyncIO (mem_file_ptr->old_mask_ptr);
	}

	mem_file_ptr->num_string_opens ++;

	jr_AListNativeSetNewTail (mem_file_ptr->char_buffer, 0, char);
	jr_AListDeleteTail (mem_file_ptr->char_buffer);

	return jr_AListHeadPtr (mem_file_ptr->char_buffer);
}

void jr_MemoryFileCloseString (mem_file_ptr, string)
	jr_MemoryFileType *			mem_file_ptr;
	const char *				string;
{
	mem_file_ptr->num_string_opens --;

	if (mem_file_ptr->num_string_opens == 0  &&  mem_file_ptr->old_mask_ptr) {
		jr_UnBlockAsyncIO (mem_file_ptr->old_mask_ptr);
		jr_free (mem_file_ptr->old_mask_ptr);
		mem_file_ptr->old_mask_ptr	= 0;
	}
}

void jr_MemoryFileIO_Handler (rfd, mem_file_ptr)
	jr_int						rfd;
	jr_MemoryFileType *			mem_file_ptr;
{
	jr_int						read_size;			
	jr_int						status;


	while (1) {
		read_size				= 8192;


		if (	mem_file_ptr->max_buffer_size  == 0
			||	jr_AListSize (mem_file_ptr->char_buffer)  <  mem_file_ptr->max_buffer_size) {

			jr_AListIncreaseCapacity (
				mem_file_ptr->char_buffer, jr_AListSize (mem_file_ptr->char_buffer) + read_size
			);
		}
		else {
			jr_AListIncreaseCapacity (
				mem_file_ptr->char_buffer, mem_file_ptr->max_buffer_size + read_size
			);
		}

		status	= read (rfd, jr_AListTailPtr (mem_file_ptr->char_buffer) + 1, read_size);
	
		if (status == 0) {
			mem_file_ptr->saw_eof		= 1;
			break;
		}
		if (status < 0) {
			break ;
		}

		if (	mem_file_ptr->max_buffer_size  ==  0
			||	jr_AListSize (mem_file_ptr->char_buffer) + status  <  mem_file_ptr->max_buffer_size) {

			jr_AListSetSize (
				mem_file_ptr->char_buffer, jr_AListSize (mem_file_ptr->char_buffer) + status
			);
		}
		else {
			jr_AListSetSize (mem_file_ptr->char_buffer, mem_file_ptr->max_buffer_size);
		}
	}
}

#else
/*
 * No asynchronous I/O on pipes.  Use a temp file instead.
 */

jr_MemoryFileType *jr_MemoryFileCreateForWriting (max_buffer_size, error_buf)
	jr_int						max_buffer_size;
	char *						error_buf;
{
	jr_MemoryFileType *			mem_file_ptr		= 0;
	const char *				file_name;

	jr_int						status;

	/*
	 * No Init() function, since we store the pointer to the struct
	 * in the async handler support routines.
	 */

	mem_file_ptr		= jr_malloc (sizeof (*mem_file_ptr));

	memset (mem_file_ptr, 0, sizeof (*mem_file_ptr));


	jr_AListInit (mem_file_ptr->char_buffer, sizeof (char));

	mem_file_ptr->max_buffer_size	= max_buffer_size;
	mem_file_ptr->rfd				= -1;
	mem_file_ptr->wfd				= -1;


	file_name						= "/tmp/j1XXXXXX";

	if (strlen (file_name) + 1  >  jr_MEM_FILE_TMP_BUF_LENGTH) {
		jr_esprintf (error_buf, "internal error: buffer length %d  <  file name length %d",
			jr_MEM_FILE_TMP_BUF_LENGTH, (jr_int) strlen (file_name) + 1
		);
		status = -1;
		goto return_status;
	}
		
		
	mem_file_ptr->fp	= jr_TempFileOpen (0, error_buf);

	if (mem_file_ptr->fp == NULL) {
		status = -1;
		goto return_status;
	}

	status = 0;

	return_status : {
		if (status != 0) {
			if (mem_file_ptr->fp) {
				fclose (mem_file_ptr->fp);
			}
			jr_AListUndo (mem_file_ptr->char_buffer);
			jr_free (mem_file_ptr);

			mem_file_ptr	= 0;
		}
	}

	return mem_file_ptr;
}


const char *jr_MemoryFileOpenString (mem_file_ptr)
	jr_MemoryFileType *			mem_file_ptr;
{
	char *						string;
	unsigned jr_int				string_length;

	fflush (mem_file_ptr->fp);

		
	fseek (mem_file_ptr->fp, 0, SEEK_SET);

	string	= jr_FilePtrReadIntoNewMemory (mem_file_ptr->fp, &string_length);

	string[string_length] = 0;

	fseek (mem_file_ptr->fp, 0, SEEK_END);

	return string;
}

void jr_MemoryFileCloseString (mem_file_ptr, string)
	jr_MemoryFileType *			mem_file_ptr;
	const char *				string;
{
	if (string) {
		jr_free (string);
	}

}

#	endif



void jr_MemoryFileDestroy (mem_file_ptr)
	jr_MemoryFileType *			mem_file_ptr;
{
	jr_AListUndo (mem_file_ptr->char_buffer);

#ifndef has_broken_sigio_on_pipes
	if (mem_file_ptr->rfd >= 0) {
		jr_FileDescUndoNonBlockingIO (mem_file_ptr->rfd, 0);
		jr_FileDescUndoReadAsyncIO (mem_file_ptr->rfd, 0);

		close (mem_file_ptr->rfd);
	}

	if (mem_file_ptr->wfd >= 0) {
		close (mem_file_ptr->wfd);
	}
#endif

	if (mem_file_ptr->fp) {
		fclose (mem_file_ptr->fp);
	}

	if (mem_file_ptr->old_mask_ptr) {
		jr_free (mem_file_ptr->old_mask_ptr);
	}

	jr_free (mem_file_ptr);
}


