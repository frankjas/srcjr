#define _POSIX_SOURCE 1

#include "ezport.h"

#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/stat.h>

#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/tar.h"
#include "jr/syscalls.h"


jr_int jr_TarEntryInitFromRfp (tar_entry_ptr, rfp, error_buf)
	jr_TarEntryType *		tar_entry_ptr ;
	FILE *					rfp ;
	char *					error_buf ;
{
	jr_int					header_offset;
	jr_int					data_offset;

	jr_int					status;

	header_offset	= ftell (rfp) ;

	status	= jr_TarEntryInitFromFifoRfp (tar_entry_ptr, rfp, error_buf);

	data_offset = ftell(rfp) ;

	jr_TarEntrySetHeaderOffset	(tar_entry_ptr, header_offset);
	jr_TarEntrySetDataOffset	(tar_entry_ptr, data_offset);

	if (status > 0) {
		status	= 0;
	}

	return status;
}


jr_int jr_TarEntryInitFromFifoRfp (tar_entry_ptr, rfp, error_buf)
	jr_TarEntryType *		tar_entry_ptr ;
	FILE *					rfp ;
	char *					error_buf ;
{
	jr_int					magic_length;
	jr_int					num_items;
	long					tmp_value;
	jr_int					status;

	char					tmp_buf [16];		/* must hold mtime, size fields */

	jr_TarEntryInit (tar_entry_ptr);

	num_items		= fread (
						jr_TarEntryHeaderPtr (tar_entry_ptr),
						sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr)),
						1, rfp
					);

	if (num_items != 1) {
		if (feof (rfp)) {
			jr_esprintf (error_buf, "couldn't read: end-of-file");
			status		= jr_TAR_EOF_ERROR;
		}
		else {
			jr_esprintf (error_buf, "couldn't read: %s", strerror (errno));
			status		= jr_TAR_INTERNAL_ERROR;
		}
		goto return_status;
	}


	if (jr_TAR_BLOCK_SIZE  <  sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr))) {
		jr_esprintf (error_buf, "jr_TAR_BLOCK_SIZE (%d)  <  sizeof (jr_TarHeaderStruct) (%d)",
			jr_TAR_BLOCK_SIZE, (jr_int) sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr))
		);

		status		= jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	if (sizeof (tmp_buf)  <  jr_TAR_BLOCK_SIZE  -  sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr))) {
		jr_esprintf (error_buf, "couldn't align input: tmp_buf too small, should be at least %d bytes",
			jr_TAR_BLOCK_SIZE - (jr_int) sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr))
		);
		status		= jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}


	num_items		= fread (
						tmp_buf,
						jr_TAR_BLOCK_SIZE - sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr)),
						1, rfp
					);

	if (num_items != 1  &&  !feof (rfp)) {
		jr_esprintf (error_buf, "couldn't read: %s", strerror (errno));
		status		= jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}


	if (	jr_TarEntryHeaderName (tar_entry_ptr)[0]  ==  0
		&&	jr_TarEntryPrefix (tar_entry_ptr)[0]  ==  0) {
		/*
		 * This is the case which will be encountered
		 * in the first of the 2 NULL blocks at the end
		 * of a tar file.
		 */

		jr_esprintf (error_buf, "couldn't read: null tar block");
		status		= jr_TAR_EOF_ERROR;

		goto return_status;
	}

	magic_length	= strlen (TMAGIC);

	if (strncmp (jr_TarEntryHeaderMagic (tar_entry_ptr), TMAGIC, magic_length)  !=  0) {

		if (	jr_TarEntryHeaderMagic (tar_entry_ptr)[magic_length]  !=  0
			&&	jr_TarEntryHeaderMagic (tar_entry_ptr)[magic_length]  != ' ') {

			jr_esprintf (error_buf, "%.128s: bad magic field", jr_TarEntryHeaderName( tar_entry_ptr));
			status = jr_TAR_INTERNAL_ERROR;
			goto return_status;
		}
	}



	switch (jr_TarEntryHeaderTypeFlag (tar_entry_ptr)) {
		case LNKTYPE	:
		case SYMTYPE	:
		case DIRTYPE	:
		case BLKTYPE	:
		case CHRTYPE	:
		case FIFOTYPE	: {
			jr_TarEntrySetSizeInt	(tar_entry_ptr, 0);
			jr_TarEntrySetNumBlocks	(tar_entry_ptr, 0);
			break ;
		}

		default			:
		case REGTYPE	:
		case AREGTYPE	:
		case CONTTYPE	: {

			strncpy (
				tmp_buf, jr_TarEntryHeaderSize (tar_entry_ptr),
				sizeof (jr_TarEntryHeaderSize (tar_entry_ptr))
			);
			tmp_buf [sizeof (jr_TarEntryHeaderSize (tar_entry_ptr))] = 0;

			num_items	= sscanf (tmp_buf, " %lo", &tmp_value) ;

			if (num_items != 1) {
				jr_esprintf (error_buf, "couldn't parse 'size' field");
				status = jr_TAR_INTERNAL_ERROR;
				goto return_status;
			}

			jr_TarEntrySetSizeInt	(tar_entry_ptr, tmp_value);

			tmp_value				= (jr_TarEntrySize (tar_entry_ptr) + (jr_TAR_BLOCK_SIZE-1));
			tmp_value				/= jr_TAR_BLOCK_SIZE ;

			jr_TarEntrySetNumBlocks (tar_entry_ptr, tmp_value);

			break ;
		}
	}


	num_items = sscanf (jr_TarEntryHeaderMode (tar_entry_ptr),   " %lo",  &tmp_value) ;

	if (num_items != 1) {
		jr_esprintf (error_buf, "couldn't parse 'mode' field");
		status = jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	jr_TarEntrySetModeInt (tar_entry_ptr, tmp_value);


	num_items = sscanf (jr_TarEntryHeaderUID (tar_entry_ptr),    " %lo",  &tmp_value) ;

	if (num_items != 1) {
		jr_esprintf (error_buf, "couldn't parse 'uid' field");
		status = jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	jr_TarEntrySetUIDInt (tar_entry_ptr, tmp_value);


	num_items = sscanf (jr_TarEntryHeaderGID (tar_entry_ptr),    " %lo",  &tmp_value) ;

	if (num_items != 1) {
		jr_esprintf (error_buf, "couldn't parse 'gid' field");
		status = jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	jr_TarEntrySetGIDInt (tar_entry_ptr, tmp_value);


	strncpy (
		tmp_buf, jr_TarEntryHeaderMTime (tar_entry_ptr),
		sizeof (jr_TarEntryHeaderMTime (tar_entry_ptr))
	);
	tmp_buf [sizeof (jr_TarEntryHeaderMTime (tar_entry_ptr))] = 0;

	num_items = sscanf (tmp_buf,  " %lo",  &tmp_value) ;

	if (num_items != 1) {
		jr_esprintf (error_buf, "couldn't parse 'mtime' field");
		status = jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	jr_TarEntrySetMTimeInt (tar_entry_ptr, tmp_value);


	num_items = sscanf (jr_TarEntryHeaderDevMajor (tar_entry_ptr),   " %lo",  &tmp_value) ;

	if (num_items != 1) {
		if (jr_TarEntryHeaderDevMajor (tar_entry_ptr) [0]  !=  0) {
			jr_esprintf (error_buf, "couldn't parse 'devmajor' field");
			status = jr_TAR_INTERNAL_ERROR;
			goto return_status;
		}
	}
	else {
		jr_TarEntrySetDevMajorInt (tar_entry_ptr, tmp_value);
	}



	num_items = sscanf (jr_TarEntryHeaderDevMinor (tar_entry_ptr),   " %lo",  &tmp_value) ;

	if (num_items != 1) {
		if (jr_TarEntryHeaderDevMinor (tar_entry_ptr) [0]  !=  0) {
			jr_esprintf (error_buf, "couldn't parse 'devminor' field");
			status = jr_TAR_INTERNAL_ERROR;
			goto return_status;
		}
	}
	else {
		jr_TarEntrySetDevMinorInt (tar_entry_ptr, tmp_value);
	}


	num_items = sscanf (jr_TarEntryHeaderChkSum (tar_entry_ptr), " %lo",  &tmp_value) ;

	if (num_items != 1) {
		jr_esprintf (error_buf, "couldn't parse 'chksum' field");
		status = jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	jr_TarEntrySetChkSumInt (tar_entry_ptr, tmp_value);


	status	= jr_TAR_BLOCK_SIZE;

	return_status : {
		if (status != 0) {
			jr_TarEntryUndo (tar_entry_ptr);
		}
	}

	return status;
}


jr_int jr_TarEntryReadData (tar_entry_ptr, rfp, opt_data_ref, error_buf)
	jr_TarEntryType *		tar_entry_ptr ;
	FILE *					rfp ;
	void **					opt_data_ref;
	char *					error_buf ;
{
	struct stat				stat_info[1];

	void *					data_ptr					= 0;
	jr_int					num_items;
	jr_int					is_fifo_rfp					= 0;
	jr_int					align_bytes					= 0;
	unsigned jr_int			k;

	jr_int					status;



	if (opt_data_ref) {
		data_ptr = jr_malloc (jr_TarEntrySize (tar_entry_ptr) + 1) ;
		/*
		 * add 1 since we want to make a string out of it
		 * added 3/1/94
		 */

		if (data_ptr == 0) {
			jr_esprintf (error_buf, "couldn't allocate %ld bytes for data (header offset %ld)",
				(long) jr_TarEntrySize (tar_entry_ptr),
				(long) jr_TarEntryHeaderOffset (tar_entry_ptr)
			) ;

			status = jr_TAR_INTERNAL_ERROR;
			goto return_status;
		}
	}


	if (jr_TarEntrySize (tar_entry_ptr) > 0) {

		if (data_ptr) {
			num_items = fread (data_ptr, jr_TarEntrySize (tar_entry_ptr), 1, rfp) ;

			if (num_items != 1) {
				jr_esprintf (error_buf, "couldn't read %ld bytes for (data header block %ld) : %s",
					(long) jr_TarEntrySize (tar_entry_ptr),
					(long) jr_TarEntryHeaderOffset (tar_entry_ptr),
					strerror(errno)
				) ;

				status = jr_TAR_INTERNAL_ERROR;
				goto return_status;
			}
		}
		else {
			
			status	= fstat (fileno (rfp), stat_info);

			if (status != 0) {
				jr_esprintf (error_buf, "couldn't fstat() input: %s",
					strerror(errno)
				) ;

				status = jr_TAR_INTERNAL_ERROR;
				goto return_status;
			}

			is_fifo_rfp	= S_ISFIFO (stat_info->st_mode);

			if (is_fifo_rfp) {
				
				for (k=0;  k < jr_TarEntrySize (tar_entry_ptr);  k++) {
					if (getc (rfp)  ==  EOF) {
						break;
					}
				}
			}
			else {
				fseek (rfp, jr_TarEntrySize (tar_entry_ptr), SEEK_CUR) ;
			}
		}

		align_bytes		= jr_TarFilePtrAlignFifoRead (rfp, jr_TarEntrySize (tar_entry_ptr));
	}

	if (opt_data_ref) {
		((char *) data_ptr) [jr_TarEntrySize (tar_entry_ptr)] = 0;

		*opt_data_ref	= data_ptr;
		data_ptr		= 0;
	}

	status			= jr_TarEntrySize (tar_entry_ptr) + align_bytes;

	return_status : {
		if (data_ptr) {
			jr_free (data_ptr);
		}
	}
	return status;
}


jr_int jr_TarEntryCopyData (tar_entry_ptr, rfp, wfp, error_buf)
	jr_TarEntryType *		tar_entry_ptr ;
	FILE *					rfp ;
	FILE *					wfp ;
	char *					error_buf ;
{
	unsigned jr_int			k;
	jr_int					c;
	jr_int					align_bytes			= 0;

	jr_int					status;



	if (jr_TarEntrySize (tar_entry_ptr) > 0) {

		
		for (k=0;  k < jr_TarEntrySize (tar_entry_ptr);  k++) {
			if ((c = getc (rfp))  ==  EOF) {
				jr_esprintf (error_buf, "unexpected EOF after byte %d", k);
				status	= jr_TAR_EOF_ERROR;
				goto return_status;
			}
			putc (c, wfp);
		}

		align_bytes	= jr_TarFilePtrAlignFifoWrite (wfp, jr_TarEntrySize (tar_entry_ptr));
		align_bytes	= jr_TarFilePtrAlignFifoRead (rfp, jr_TarEntrySize (tar_entry_ptr));
	}


	status			= jr_TarEntrySize (tar_entry_ptr) + align_bytes;

	return_status : {
	}
	return status;
}


jr_int jr_TarEntryWriteSegment (	
	jr_TarEntryType *		tar_entry_ptr ,
	unsigned jr_int			total_data_size,
	const void *			opt_data_ptr,
	FILE *					opt_data_rfp,
	unsigned jr_int			curr_data_size,
	FILE *					wfp,
	char *					error_buf)
{
	char					tmp_buf [16];		/* must hold mtime, size fields */
	jr_int					num_items;
	jr_int					status;


	/*
	 * This function writes out the 512 byte tar header
	 * followed by the data and seeks to the next 512 block boundary
	 * after the data has been written.
	 * The tar header data will be adjusted to match the new size
	 * argument passed in, and a new checksum will be calculated.
	 */

	switch (jr_TarEntryTypeFlag (tar_entry_ptr)) {
		case DIRTYPE	:
		case LNKTYPE	:
		case SYMTYPE	: {
			total_data_size	= 0;
			break;
		}
	}


	jr_TarEntrySetSize (tar_entry_ptr, total_data_size);

	jr_TarEntryMakePosixConformant (tar_entry_ptr);
	
	num_items	= fwrite (
					jr_TarEntryHeaderPtr (tar_entry_ptr),
					sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr)),
					1, wfp
				);

	if (num_items != 1) {
		jr_esprintf (error_buf, "couldn't write tar header: %s", strerror (errno));

		status		= jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}



	if (sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr))  >  jr_TAR_BLOCK_SIZE) {
		jr_esprintf (error_buf, "sizeof (jr_TarHeaderStruct) (%d)  >  jr_TAR_BLOCK_SIZE (%d)",
			(jr_int) sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr)), jr_TAR_BLOCK_SIZE
		);

		status		= jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	if (jr_TAR_BLOCK_SIZE  -  sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr))  >  sizeof (tmp_buf)) {
		jr_esprintf (error_buf, "couldn't align input: tmp_buf too small, should be at least %d bytes",
			jr_TAR_BLOCK_SIZE - (jr_int) sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr))
		);
		status		= jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}

	memset (tmp_buf, 0, sizeof (tmp_buf));

	num_items		= fwrite (
						tmp_buf,
						jr_TAR_BLOCK_SIZE - sizeof (*jr_TarEntryHeaderPtr (tar_entry_ptr)),
						1, wfp
					);

	if (num_items != 1) {
		jr_esprintf (error_buf, "couldn't write tar header alignment: %s", strerror (errno));
		status		= jr_TAR_INTERNAL_ERROR;
		goto return_status;
	}


	if (curr_data_size > 0) {
		if (opt_data_ptr) {
			num_items	= fwrite (opt_data_ptr, curr_data_size, 1, wfp);

			if (num_items != 1) {
				jr_esprintf (error_buf, "couldn't write data: %s", strerror (errno));

				status		= jr_TAR_INTERNAL_ERROR;
				goto return_status;
			}
		}
		else if (opt_data_rfp) {
			/*
			 * Fill with null if no data or not enough data.
			 */
			jr_int			c				= 0;
			unsigned jr_int	k;

			for (k=0;  k < curr_data_size; k++) {
				if (opt_data_rfp) {
					c = getc (opt_data_rfp);

					if (c == EOF) {
						c = 0;
					}
				}
				putc (c, wfp);
			}
		}

		if (opt_data_ptr || opt_data_rfp) {
			if (curr_data_size  ==  total_data_size) {
				jr_TarFilePtrAlignFifoWrite (wfp, curr_data_size);
			}
		}
	}


	status	= 0;

	return_status : {
	}

	return status;
}



void jr_TarEntrySetIsDir (tar_entry_ptr)
	jr_TarEntryType *		tar_entry_ptr;
{
	jr_TarEntrySetTypeFlag (tar_entry_ptr, DIRTYPE);
}

void jr_TarEntrySetIsFile (tar_entry_ptr)
	jr_TarEntryType *		tar_entry_ptr;
{
	jr_TarEntrySetTypeFlag (tar_entry_ptr, REGTYPE);
}

void jr_TarEntrySetCustomType (tar_entry_ptr, type_value)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					type_value;
{
	jr_TarEntrySetTypeFlag (tar_entry_ptr, type_value);
}

void jr_TarEntrySetIsSymLink (tar_entry_ptr, link_name)
	jr_TarEntryType *		tar_entry_ptr;
	const char *			link_name;
{
	jr_TarEntrySetTypeFlag (tar_entry_ptr, SYMTYPE);

	strncpy (
		jr_TarEntryHeaderLinkName (tar_entry_ptr), 
		link_name,
		sizeof (jr_TarEntryHeaderLinkName (tar_entry_ptr)) - 1
	);
}


void jr_TarEntrySetUserInfo (tar_entry_ptr, uid, opt_uname, gid, opt_gname)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					uid;
	const char *			opt_uname;
	jr_int					gid;
	const char *			opt_gname;
{

	jr_TarEntrySetUID (tar_entry_ptr, uid);
	jr_TarEntrySetGID (tar_entry_ptr, gid);

	if (opt_uname) {
		strncpy (
			jr_TarEntryHeaderUName (tar_entry_ptr), 
			opt_uname,
			sizeof (jr_TarEntryHeaderUName (tar_entry_ptr)) - 1
		);
	}
	if (opt_gname) {
		strncpy (
			jr_TarEntryHeaderGName (tar_entry_ptr), 
			opt_gname,
			sizeof (jr_TarEntryHeaderGName (tar_entry_ptr)) - 1
		);
	}
}


void jr_TarEntrySetPathInfo (tar_entry_ptr, pathname, is_dir, mode, mtime)
	jr_TarEntryType *		tar_entry_ptr ;
	const char *			pathname ;
	jr_int					is_dir;
	jr_int					mode;
	jr_int					mtime;
{
	jr_int					pathname_length			= strlen (pathname);
	jr_int					tar_pathname_length		= pathname_length;
	jr_int					has_trailing_slash		= 0;


	if (is_dir  &&  pathname_length > 0) {

		if (pathname [pathname_length - 1]  !=  '/') {
			has_trailing_slash	= 0;
			tar_pathname_length ++;
		}
	}

	if (	tar_pathname_length
		>	(jr_int) (sizeof(jr_TarEntryHeaderName (tar_entry_ptr))
			+ sizeof(jr_TarEntryHeaderPrefix (tar_entry_ptr)))) {

		/*
		 * Path name too long.  Is caller's responsibility to check.
		 */
		return;
	}

	if (is_dir) {
		jr_TarEntrySetIsDir (tar_entry_ptr);
	}
	else {
		jr_TarEntrySetIsFile (tar_entry_ptr);
	}

	jr_TarEntrySetMode	(tar_entry_ptr, mode);
	jr_TarEntrySetMTime	(tar_entry_ptr, mtime);


	if (tar_pathname_length  >  (jr_int) sizeof(jr_TarEntryHeaderName (tar_entry_ptr))) {
		const char *		cp ;
		jr_int				prefix_length			= tar_pathname_length;


		for (cp = pathname + pathname_length;  cp >= pathname;  cp--) {

			if (*cp == '/') {
				prefix_length	= cp - pathname + 1;

				if (prefix_length <=  (jr_int) sizeof(jr_TarEntryHeaderPrefix (tar_entry_ptr))) {
					break ;
				}
			}
		}

		if (	(*cp == '/')  &&  (cp != pathname)
			&&		tar_pathname_length - prefix_length
				<=  (jr_int) sizeof (jr_TarEntryHeaderName (tar_entry_ptr))) {
			
			strncpy (
				jr_TarEntryHeaderPrefix (tar_entry_ptr),
				pathname,
				prefix_length
			);

			if (prefix_length  <  (jr_int) sizeof(jr_TarEntryHeaderPrefix (tar_entry_ptr))) {
				/*
				** 9/11/06: if it has no null, use trailing '/' instead, otherwise no null?
				** 12/31/06: use "null required if null fits" rule?
				** ToDo: verify with standard.
				*/
				jr_TarEntryHeaderPrefix (tar_entry_ptr)[prefix_length] = 0;
			}

			strncpy (
				jr_TarEntryHeaderName (tar_entry_ptr),
				cp + 1,
				sizeof (jr_TarEntryHeaderName (tar_entry_ptr))
			);

			if (is_dir  &&  !has_trailing_slash) {
				jr_TarEntryHeaderName (tar_entry_ptr)[pathname_length - prefix_length] = '/';
			}

			/*
			 * Use tar_pathname_length since we've added the trailing slash if it's a dir.
			 */

			if (	tar_pathname_length - prefix_length
				<  (jr_int) sizeof(jr_TarEntryHeaderName (tar_entry_ptr))) {

				jr_TarEntryHeaderName (tar_entry_ptr)[tar_pathname_length - prefix_length] = 0;
				/*
				 * Null required if the null fits.
				 */
			}
		}
		else {
			/*
			 * Path name too long.  Is caller's responsibility to check.
			 */
			jr_TarEntryHeaderName (tar_entry_ptr)[0] = 0;
		}
	}
	else {
		strncpy (
			jr_TarEntryHeaderName (tar_entry_ptr),
			pathname,
			sizeof (jr_TarEntryHeaderName (tar_entry_ptr))
		);
		memset (
			jr_TarEntryHeaderPrefix (tar_entry_ptr),
			0,
			sizeof (jr_TarEntryHeaderPrefix (tar_entry_ptr))
		);

		if (is_dir  &&  !has_trailing_slash) {
			jr_TarEntryHeaderName (tar_entry_ptr)[pathname_length] = '/';
		}

		/*
		 * Use tar_pathname_length since we've added the trailing slash if it's a dir.
		 */

		if (tar_pathname_length <  (jr_int) sizeof(jr_TarEntryHeaderName (tar_entry_ptr))) {
			jr_TarEntryHeaderName (tar_entry_ptr)[tar_pathname_length] = 0;
		}
	}
}


void jr_TarEntryInit (tar_entry_ptr)
	jr_TarEntryType *		tar_entry_ptr;
{
	memset (tar_entry_ptr, 0, sizeof (*tar_entry_ptr));
}


void jr_TarEntryUndo (tar_entry_ptr)
	jr_TarEntryType *		tar_entry_ptr;
{
}

void jr_TarEntrySetMode (tar_entry_ptr, mode_int)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					mode_int;
{
	if (mode_int < 0) {
		mode_int	= 0;
	}
	jr_TarEntrySetModeInt (tar_entry_ptr, mode_int);

	if ((unsigned) mode_int < jr_TAR_MAX_7_BYTE_ASCII_OCTAL) {
		sprintf (jr_TarEntryHeaderMode (tar_entry_ptr) ,"%07o", (unsigned) mode_int) ;
	}
}

void jr_TarEntrySetUID (tar_entry_ptr, uid_int)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					uid_int;
{
	if (uid_int < 0) {
		uid_int	= jr_USHRT_MAX;			/* 1/5/07: Unix id for "nobody"? */
	}
	jr_TarEntrySetUIDInt (tar_entry_ptr, uid_int);

	if ((unsigned) uid_int < jr_TAR_MAX_7_BYTE_ASCII_OCTAL) {
		sprintf (jr_TarEntryHeaderUID (tar_entry_ptr) ,"%07o", (unsigned) uid_int) ;
	}
}

void jr_TarEntrySetGID (tar_entry_ptr, gid_int)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					gid_int;
{
	if (gid_int < 0) {
		gid_int	= jr_USHRT_MAX;			/* 1/5/07: Unix id for "nobody"? */
	}
	jr_TarEntrySetGIDInt (tar_entry_ptr, gid_int);

	if ((unsigned) gid_int < jr_TAR_MAX_7_BYTE_ASCII_OCTAL) {
		sprintf (jr_TarEntryHeaderGID (tar_entry_ptr) ,"%07o", (unsigned) gid_int) ;
	}
}

void jr_TarEntrySetSize (tar_entry_ptr, size_int)
	jr_TarEntryType *		tar_entry_ptr;
	unsigned jr_int			size_int;
{
	jr_TarEntrySetSizeInt (tar_entry_ptr, size_int);

	if (size_int < jr_TAR_MAX_12_BYTE_ASCII_OCTAL) {
		sprintf (jr_TarEntryHeaderSize (tar_entry_ptr) ,"%011o", size_int) ;
	}
	else {
		sprintf (jr_TarEntryHeaderSize (tar_entry_ptr) ,"%011o", jr_TAR_MAX_12_BYTE_ASCII_OCTAL) ;
	}
}

void jr_TarEntrySetMTime (tar_entry_ptr, mtime_int)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					mtime_int;
{
	if (mtime_int < 0) {
		mtime_int	= 0;
	}
	jr_TarEntrySetMTimeInt (tar_entry_ptr, mtime_int);

	if ((unsigned) mtime_int < jr_TAR_MAX_12_BYTE_ASCII_OCTAL) {
		sprintf (jr_TarEntryHeaderMTime (tar_entry_ptr) ,"%011o", (unsigned) mtime_int) ;
	}
}


void jr_TarEntrySetDevMajor (tar_entry_ptr, dev_major)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					dev_major;
{
	if (dev_major < 0) {
		dev_major	= 0;
	}
	jr_TarEntrySetDevMajorInt (tar_entry_ptr, dev_major);

	if ((unsigned) dev_major < jr_TAR_MAX_7_BYTE_ASCII_OCTAL) {
		sprintf (jr_TarEntryHeaderDevMajor (tar_entry_ptr) ,"%07o", (unsigned) dev_major) ;
	}
}

void jr_TarEntrySetDevMinor (tar_entry_ptr, dev_minor)
	jr_TarEntryType *		tar_entry_ptr;
	jr_int					dev_minor;
{
	if (dev_minor < 0) {
		dev_minor	= 0;
	}
	jr_TarEntrySetDevMinorInt (tar_entry_ptr, dev_minor);

	if ((unsigned) dev_minor < jr_TAR_MAX_7_BYTE_ASCII_OCTAL) {
		sprintf (jr_TarEntryHeaderDevMinor (tar_entry_ptr) ,"%07o", (unsigned) dev_minor) ;
	}
}

void jr_TarEntrySetMagic (tar_entry_ptr, magic_string)
	jr_TarEntryType *		tar_entry_ptr;
	const char *			magic_string;
{
	sprintf (jr_TarEntryHeaderMagic (tar_entry_ptr), "%.*s",
		(jr_int) sizeof(jr_TarEntryHeaderMagic (tar_entry_ptr))-1,
		TMAGIC
	) ;
}
 

void jr_TarEntrySetVersion (tar_entry_ptr, magic_string)
	jr_TarEntryType *		tar_entry_ptr;
	const char *			magic_string;
{
	jr_TarHeaderStruct *	tar_header_ptr			= jr_TarEntryHeaderPtr (tar_entry_ptr);

	tar_header_ptr->version [0]		= '0';
	tar_header_ptr->version [1]		= '0';

	/*
	 * No null in this field.
	 */
}


static unsigned long SumByteString(string, nbytes)
	unsigned char *			string ;
	jr_int					nbytes ;
{
	unsigned long			sum					= 0L;
	jr_int					i;

	for (i=0; i < nbytes; i++) {
		sum += string[i] ;
	}
	
	return(sum) ;
}


void jr_TarEntryInitCheckSum (tar_entry_ptr)
	jr_TarEntryType *		tar_entry_ptr;
{
	jr_TarHeaderStruct *	thp						= jr_TarEntryHeaderPtr (tar_entry_ptr);
	jr_int					chksum_val				= 0;

	chksum_val += SumByteString (jr_TarHeaderName (thp),		sizeof(jr_TarHeaderName (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderMode (thp),		sizeof(jr_TarHeaderMode (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderUID (thp),			sizeof(jr_TarHeaderUID (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderGID (thp),			sizeof(jr_TarHeaderGID (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderSize (thp),		sizeof(jr_TarHeaderSize (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderMTime (thp),		sizeof(jr_TarHeaderMTime (thp))) ;
	chksum_val += SumByteString ("              ",				sizeof(jr_TarHeaderChkSum (thp))) ;
	chksum_val += SumByteString (&jr_TarHeaderTypeFlag (thp),	sizeof(thp->typeflag)) ;
	chksum_val += SumByteString (jr_TarHeaderLinkName (thp),	sizeof(jr_TarHeaderLinkName (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderMagic (thp),		sizeof(jr_TarHeaderMagic (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderVersion (thp),		sizeof(jr_TarHeaderVersion (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderUName (thp),		sizeof(jr_TarHeaderUName (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderGName (thp),		sizeof(jr_TarHeaderGName (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderDevMajor (thp),	sizeof(jr_TarHeaderDevMajor (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderDevMinor (thp),	sizeof(jr_TarHeaderDevMinor (thp))) ;
	chksum_val += SumByteString (jr_TarHeaderPrefix (thp),		sizeof(jr_TarHeaderPrefix (thp))) ;


	jr_TarEntrySetChkSumInt (tar_entry_ptr, chksum_val);

	if ((unsigned) chksum_val < jr_TAR_MAX_7_BYTE_ASCII_OCTAL) {
		sprintf(jr_TarHeaderChkSum (thp), "%07o", (unsigned) chksum_val) ;
	}
	else {
		/* 
		 * max. check sum : 512 * 256 == 131072 < 2079151 == MAX_7_BYTE_ASCII_OCTAL
		 */
	}
}

void jr_TarEntryMakePosixConformant (tar_entry_ptr)
	jr_TarEntryType *		tar_entry_ptr;
{
	jr_TarEntrySetMagic		(tar_entry_ptr, TMAGIC);
	jr_TarEntrySetVersion	(tar_entry_ptr, TVERSION);
	jr_TarEntryInitCheckSum	(tar_entry_ptr);
}


const char *jr_TarEntryMakeFullPath (tar_entry_ptr)
	jr_TarEntryType *		tar_entry_ptr;
{
	jr_int					prefix_length		= strlen (jr_TarEntryPrefix (tar_entry_ptr));
	jr_int					name_length			= strlen (jr_TarEntryName (tar_entry_ptr));
	jr_int					path_length			= prefix_length + 1 + name_length;
	char *					path_string;

	path_string		= jr_malloc (path_length + 1);

	if (jr_TarEntryPrefix (tar_entry_ptr)[0]) {

		if (jr_TarEntryPrefix(tar_entry_ptr)[prefix_length - 1] == '/') {
			sprintf (path_string, "%s%s",
				jr_TarEntryPrefix (tar_entry_ptr), jr_TarEntryName (tar_entry_ptr)
			);
		}
		else {
			sprintf (path_string, "%s/%s",
				jr_TarEntryPrefix (tar_entry_ptr), jr_TarEntryName (tar_entry_ptr)
			);
		}
	}
	else {
		sprintf (path_string, "%s", jr_TarEntryName (tar_entry_ptr));
	}

	return path_string;
}


void jr_TarFilePtrAlign (fp)
	FILE *					fp;
{
	unsigned jr_int			absolute_offset ;
	unsigned jr_int			offset_within_block ;

	absolute_offset = ftell(fp) ;

	offset_within_block = (absolute_offset % jr_TAR_BLOCK_SIZE) ;

	if (offset_within_block != 0) {
		absolute_offset += jr_TAR_BLOCK_SIZE - offset_within_block ;
		fseek(fp, absolute_offset, SEEK_SET) ;
	}
}


jr_int jr_TarFilePtrAlignFifoRead (rfp, num_bytes_moved)
	FILE *					rfp;
	unsigned jr_int			num_bytes_moved;
{
	unsigned jr_int			offset_within_block ;
	unsigned jr_int			bytes_to_block_end 		= 0;

	unsigned jr_int			k;


	offset_within_block = (num_bytes_moved % jr_TAR_BLOCK_SIZE) ;

	if (offset_within_block != 0) {
		bytes_to_block_end	= jr_TAR_BLOCK_SIZE - offset_within_block;

		for (k=0;  k < bytes_to_block_end;  k++) {
			if (getc (rfp) == EOF) {
				break;
			}
		}
	}

	return bytes_to_block_end;
}


jr_int jr_TarFilePtrAlignFifoWrite (wfp, num_bytes_moved)
	FILE *					wfp;
	unsigned jr_int			num_bytes_moved;
{
	unsigned jr_int			offset_within_block ;
	unsigned jr_int			bytes_to_block_end			= 0;

	unsigned jr_int			k;


	offset_within_block = (num_bytes_moved % jr_TAR_BLOCK_SIZE) ;

	if (offset_within_block != 0) {
		bytes_to_block_end	= jr_TAR_BLOCK_SIZE - offset_within_block;

		for (k=0;  k < bytes_to_block_end; k++) {
			putc (0, wfp);
		}
	}

	return bytes_to_block_end;
}


jr_int jr_TarFileWriteEOF (wfp, error_buf)
	FILE *					wfp;
	char *					error_buf;
{
	jr_int					num_items;
	jr_int					k;
	char					null_buf [jr_TAR_BLOCK_SIZE];

	/*
	 * 6/15/2004: two null blocks signify the end of a tar file
	 * according to /usr/include/tar.h on Linux.
	 */

	memset (null_buf, 0, jr_TAR_BLOCK_SIZE);

	for (k=0;  k < 2;  k++) {
		num_items	= fwrite (null_buf, jr_TAR_BLOCK_SIZE, 1, wfp);

		if (num_items != 1) {
			jr_esprintf (error_buf, "couldn't write null tar header: %s", strerror (errno));

			return jr_TAR_INTERNAL_ERROR;
		}
	}

	return 0;
}

const char * jr_TarTypeFlagToString(type_flag)
	jr_int					type_flag;
{
	char *  type_desc ;

	switch (type_flag) {
		case  LNKTYPE : type_desc = "hard-link" ; break ;
		case  SYMTYPE : type_desc = "symb-link" ; break ;
		case  DIRTYPE : type_desc = "directory" ; break ;
		case  BLKTYPE : type_desc = "blck-spec" ; break ;
		case  CHRTYPE : type_desc = "char-spec" ; break ;
		case FIFOTYPE : type_desc = "fifo-spec" ; break ;
		case  REGTYPE : type_desc = "file" ; break;
		case AREGTYPE : type_desc = "file" ; break;
		case CONTTYPE : type_desc = "cont-spec" ; break ;
		default       : type_desc = "UNKNOWN" ; break ;
	}

	return(type_desc) ;
}

unsigned jr_int jr_TarModeToPerms (
	jr_int				tar_mode)
{
	unsigned jr_int		perm_flags			= 0;

	if (tar_mode & TSUID) {
		perm_flags		|= jr_PERM_S_OWNER;
	}
	if (tar_mode & TSGID) {
		perm_flags		|= jr_PERM_S_GROUP;
	}

	if (tar_mode & TUREAD) {
		perm_flags		|= jr_PERM_R_OWNER;
	}
	if (tar_mode & TUWRITE) {
		perm_flags		|= jr_PERM_W_OWNER;
	}
	if (tar_mode & TUEXEC) {
		perm_flags		|= jr_PERM_X_OWNER;
	}

	if (tar_mode & TGREAD) {
		perm_flags		|= jr_PERM_R_GROUP;
	}
	if (tar_mode & TGWRITE) {
		perm_flags		|= jr_PERM_W_GROUP;
	}
	if (tar_mode & TGEXEC) {
		perm_flags		|= jr_PERM_X_GROUP;
	}

	if (tar_mode & TOREAD) {
		perm_flags		|= jr_PERM_R_OTHER;
	}
	if (tar_mode & TOWRITE) {
		perm_flags		|= jr_PERM_W_OTHER;
	}
	if (tar_mode & TOEXEC) {
		perm_flags		|= jr_PERM_X_OTHER;
	}

	return perm_flags;
}

