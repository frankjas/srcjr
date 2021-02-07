#ifndef __TAR_H__
#define __TAR_H__

#include "ezport.h"

#include <stdio.h>

/*
 * 6/16/2004: existing tar formats include (from GNU tar info files, "% info tar")
 * v7 - Unix V7 format
 * gnu
 * posix 'ustar' (1988)
 * posix (2001)
 */

#define jr_TAR_INTERNAL_ERROR					-1
#define jr_TAR_EOF_ERROR						-2


#define jr_TAR_BLOCK_SIZE						512
#define jr_TAR_BLOCK_SIZE_NUM_BITS				9

#define jr_TAR_PATH_MAX							255
#define jr_TAR_SYM_LINK_MAX						100

#define jr_TAR_UNAME_MAX						31
#define jr_TAR_GNAME_MAX						31

#define jr_TAR_UID_MAX							jr_TAR_MAX_7_BYTE_ASCII_OCTAL
#define jr_TAR_GID_MAX							jr_TAR_MAX_7_BYTE_ASCII_OCTAL

#define jr_TAR_MAX_7_BYTE_ASCII_OCTAL			07777777		/* 2079151 */
#define jr_TAR_MAX_12_BYTE_ASCII_OCTAL			jr_UINT_MAX



typedef struct {
	char name		[100] ;					/* NUL-terminated if NUL fits */
	char mode		[8] ;					/* NUL-terminated digit string (MKS documentation) */
	char uid		[8] ;					/* NUL-terminated digit string (MKS documentation) */
	char gid		[8] ;					/* NUL-terminated digit string (MKS documentation) */
	char size		[12] ;
	char mtime		[12] ;
	char chksum		[8] ;					/* NUL-terminated digit string (MKS documentation) */
	char typeflag	[1] ;					/* see below */
	char linkname	[100] ;					/* NUL-terminated if NUL fits */
	char magic		[6] ;					/* must be TMAGIC (NUL term.) */
	char version	[2] ;					/* must be TVERSION */
	char uname		[32] ;					/* NUL-terminated */
	char gname		[32] ;					/* NUL-terminated */
	char devmajor	[8] ;					/* NUL-terminated digit string (MKS documentation) */
	char devminor	[8] ;					/* NUL-terminated digit string (MKS documentation) */
	char prefix		[155] ;					/* NUL-terminated if NUL fits */
} jr_TarHeaderStruct ;


#ifdef missing_tar_h
	#define TMAGIC			"ustar"	/* ustar and a null */
	#define TMAGLEN			6		  
	#define TVERSION		"00"	/* 00 and no null */
	#define TVERSLEN		2

	/* Values used in typeflag field */
	#define REGTYPE			'0'		/* regular file */
	#define AREGTYPE		'\0'	/* regular file */
	#define LNKTYPE			'1'		/* hard link    */
	#define SYMTYPE			'2'		/* sym link     */
	#define CHRTYPE			'3'		/* character special */
	#define BLKTYPE 		'4'		/* block special */
	#define DIRTYPE			'5'		/* directory */
	#define FIFOTYPE		'6'		/* FIFO special */
	#define CONTTYPE		'7'		/* reserved */

	/* Bits used in the mode field - values in octal */
	#define TSUID			04000	/* set UID on execution */
	#define TSGID			02000	/* set GID on execution */
	#define TSVTX			01000	/* reserved 	   	*/
	#define TUREAD			00400	/* read by owner	*/
	#define TUWRITE			00200	/* write by owner	*/
	#define TUEXEC			00100	/* execute/search by owner */
	#define TGREAD			00040	/* read by group 	*/
	#define TGWRITE			00020	/* write by group 	*/
	#define TGEXEC			00010	/* execute/search by group */
	#define TOREAD			00004	/* read by other 	*/
	#define TOWRITE			00002	/* write by other 	*/
	#define	TOEXEC			00001	/* execute/search by other */
#else
#	include <tar.h>
#endif

#define jr_MAJOR_DEV(x)			((jr_int)(((unsigned)(x)>>8)&0377))
#define jr_MINOR_DEV(x)			((jr_int)((x)&0377))
/*
 * For extracting major and minor numbers from a short.
 */


/*
 * 6/15/2004: from <tar.h>
 *		The values for "typeflag": Values 'A'-'Z' are reserved for custom implementations.
 *		All other values are reserved for future POSIX.1 revisions.
 *
 * 9/18/2002 These GNU type values are from the GNU/Linux 'tar' source header file tar.h
 */


#define jr_TAR_GNUTYPE_DUMPDIR		'D'		/* This is a dir entry that contains the names */
											/* of files that were in the dir at the time the */
											/* dump was made.  */

#define jr_TAR_GNUTYPE_LONGLINK		'K'		/* Identifies the *next* file on the tape as */
											/* having a long linkname.  */

#define jr_TAR_GNUTYPE_LONGNAME		'L'		/* Identifies the *next* file on the tape as */
											/* having a long name.  */

#define jr_TAR_GNUTYPE_MULTIVOL		'M'		/* This is the continuation of a file that began */
											/* on another volume.  */

#define jr_TAR_GNUTYPE_NAMES		'N'		/* For storing filenames that do not fit into */
											/* the main header.  */

#define jr_TAR_GNUTYPE_SPARSE		'S'		/* This is for sparse files.  */

#define jr_TAR_GNUTYPE_VOLHDR		'V'		/* This file is a tape/volume header.  */
											/* Ignore it on extraction.  */


typedef struct {
	jr_TarHeaderStruct	tar_header[1];

	unsigned jr_int		mode_int ;
	unsigned jr_int		uid_int ;
	unsigned jr_int		gid_int ;
	unsigned jr_int		size_int ;
	unsigned jr_int		mtime_int ;
	unsigned jr_int		chksum_int ;
	unsigned jr_int		dev_major_int ;
	unsigned jr_int		dev_minor_int ;

	unsigned jr_int		header_offset ;
	unsigned jr_int		data_offset ;
	unsigned jr_int		num_blocks ;
} jr_TarEntryType ;


extern jr_int			jr_TarEntryInitFromRfp		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							FILE *					rfp,
							char *					error_buf
						));

extern jr_int			jr_TarEntryInitFromFifoRfp	PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							FILE *					rfp,
							char *					error_buf
						));

extern jr_int			jr_TarEntryReadData			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							FILE *					rfp,
							void **					opt_data_ref,
							char *					error_buf
						));

extern jr_int			jr_TarEntryCopyData			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							FILE *					rfp,
							FILE *					wfp,
							char *					error_buf
						));


extern jr_int			jr_TarEntryWriteSegment		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							unsigned jr_int			total_data_size,
							const void *			opt_data_ptr,
							FILE *					opt_data_rfp,
							unsigned jr_int			curr_data_size,
							FILE *					wfp,
							char *					error_buf
						));

#define					jr_TarEntryWrite(													\
							tar_entry_ptr,													\
							total_data_size, opt_data_ptr, opt_data_rfp,					\
							wfp, error_buf													\
						)																	\
						jr_TarEntryWriteSegment (											\
							tar_entry_ptr,													\
							total_data_size, opt_data_ptr, opt_data_rfp, total_data_size,	\
							wfp, error_buf													\
						)


extern jr_int			jr_TarFileWriteEOF			PROTO ((
							FILE *					wfp,
							char *					error_buf
						));

extern void				jr_TarEntrySetIsDir			PROTO ((
							jr_TarEntryType *		tar_entry_ptr
						));

extern void				jr_TarEntrySetIsFile		PROTO ((
							jr_TarEntryType *		tar_entry_ptr
						));

extern void				jr_TarEntrySetCustomType	PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					type_value
						));

extern void				jr_TarEntrySetIsSymLink		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							const char *			link_name
						));

extern void				jr_TarEntrySetUserInfo		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					uid,
							const char *			opt_uname,
							jr_int					gid,
							const char *			opt_gname
						));

extern void				jr_TarEntrySetPathInfo		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							const char *			pathname,
							jr_int					is_dir,
							jr_int					mode,
							jr_int					mtime
						));

extern void				jr_TarEntryInit				PROTO ((
							jr_TarEntryType *		tar_entry_ptr
						));

extern void				jr_TarEntryUndo				PROTO ((
							jr_TarEntryType *		tar_entry_ptr
						));


extern void				jr_TarEntrySetMode			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					mode_int
						));

extern void				jr_TarEntrySetUID			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					uid_int
						));

extern void				jr_TarEntrySetGID			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					gid_int
						));

extern void				jr_TarEntrySetMTime			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					mtime_int
						));

extern void				jr_TarEntrySetSize			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							unsigned jr_int			size_int
						));

extern void				jr_TarEntrySetDevMajor		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					dev_major
						));

extern void				jr_TarEntrySetDevMinor		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							jr_int					dev_minor
						));

extern void				jr_TarEntrySetMagic			PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							const char *			magic_string
						));

extern void				jr_TarEntrySetVersion		PROTO ((
							jr_TarEntryType *		tar_entry_ptr,
							const char *			magic_string
						));

extern void				jr_TarEntryInitCheckSum		PROTO ((
							jr_TarEntryType *		tar_entry_ptr
						));

extern void				jr_TarEntryMakePosixConformant	PROTO ((
							jr_TarEntryType *		tar_entry_ptr
						));

extern const char *		jr_TarEntryMakeFullPath		PROTO ((
							jr_TarEntryType *		tar_entry_ptr
						));


extern void				jr_TarFilePtrAlign			PROTO ((
							FILE *					fp
						));

extern jr_int			jr_TarFilePtrAlignFifoRead	PROTO ((
							FILE *					rfp,
							unsigned jr_int			num_bytes_moved
						));

extern jr_int			jr_TarFilePtrAlignFifoWrite	PROTO ((
							FILE *					wfp,
							unsigned jr_int			num_bytes_moved
						));


#define jr_TarEntryHeaderPtr(tar_entry_ptr)				((tar_entry_ptr)->tar_header)

#define jr_TarEntryMode(tar_entry_ptr)					((tar_entry_ptr)->mode_int)
#define jr_TarEntrySetModeInt(tar_entry_ptr, v)			((tar_entry_ptr)->mode_int = (v))

#define jr_TarEntryUID(tar_entry_ptr)					((tar_entry_ptr)->uid_int)
#define jr_TarEntrySetUIDInt(tar_entry_ptr, v)			((tar_entry_ptr)->uid_int = (v))

#define jr_TarEntryGID(tar_entry_ptr)					((tar_entry_ptr)->gid_int)
#define jr_TarEntrySetGIDInt(tar_entry_ptr, v)			((tar_entry_ptr)->gid_int = (v))

#define jr_TarEntrySize(tar_entry_ptr)					((tar_entry_ptr)->size_int)
#define jr_TarEntrySetSizeInt(tar_entry_ptr, v)			((tar_entry_ptr)->size_int = (v))

#define jr_TarEntryMTime(tar_entry_ptr)					((tar_entry_ptr)->mtime_int)
#define jr_TarEntrySetMTimeInt(tar_entry_ptr, v)		((tar_entry_ptr)->mtime_int = (v))

#define jr_TarEntryChkSum(tar_entry_ptr)				((tar_entry_ptr)->chksum_int)
#define jr_TarEntrySetChkSumInt(tar_entry_ptr, v)		((tar_entry_ptr)->chksum_int = (v))

#define jr_TarEntryHeaderOffset(tar_entry_ptr)			((tar_entry_ptr)->header_offset)
#define jr_TarEntrySetHeaderOffset(tar_entry_ptr, v)	((tar_entry_ptr)->header_offset = (v))

#define jr_TarEntryDataOffset(tar_entry_ptr)			((tar_entry_ptr)->data_offset)
#define jr_TarEntrySetDataOffset(tar_entry_ptr, v)		((tar_entry_ptr)->data_offset = (v))

#define jr_TarEntryNumBlocks(tar_entry_ptr)				((tar_entry_ptr)->num_blocks)
#define jr_TarEntrySetNumBlocks(tar_entry_ptr, v)		((tar_entry_ptr)->num_blocks = (v))

#define jr_TarEntryDevMajor(tar_entry_ptr)				((tar_entry_ptr)->dev_major_int)
#define jr_TarEntrySetDevMajorInt(tar_entry_ptr, v)		((tar_entry_ptr)->dev_major_int = (v))

#define jr_TarEntryDevMinor(tar_entry_ptr)				((tar_entry_ptr)->dev_minor_int)
#define jr_TarEntrySetDevMinorInt(tar_entry_ptr, v)		((tar_entry_ptr)->dev_minor_int = (v))

#define jr_TarEntryName(tar_entry_ptr)					\
		jr_TarHeaderName (jr_TarEntryHeaderPtr (tar_entry_ptr))

#define jr_TarEntryTypeFlag(tar_entry_ptr)				\
		jr_TarHeaderTypeFlag (jr_TarEntryHeaderPtr (tar_entry_ptr))

#define jr_TarEntrySetTypeFlag(tar_entry_ptr, v)		\
		jr_TarHeaderSetTypeFlag (jr_TarEntryHeaderPtr (tar_entry_ptr), v)

#define jr_TarEntryMagic(tar_entry_ptr)					\
		jr_TarHeaderMagic (jr_TarEntryHeaderPtr (tar_entry_ptr))

#define jr_TarEntryLinkName(tar_entry_ptr)					\
		jr_TarHeaderLinkName (jr_TarEntryHeaderPtr (tar_entry_ptr))

#define jr_TarEntryUName(tar_entry_ptr)					\
		jr_TarHeaderUName (jr_TarEntryHeaderPtr (tar_entry_ptr))

#define jr_TarEntryGName(tar_entry_ptr)					\
		jr_TarHeaderGName (jr_TarEntryHeaderPtr (tar_entry_ptr))

#define jr_TarEntryPrefix(tar_entry_ptr)				\
		jr_TarHeaderPrefix (jr_TarEntryHeaderPtr (tar_entry_ptr))

#define jr_TarEntryIsDir(tar_entry_ptr)				\
		jr_TarHeaderIsDir (jr_TarEntryHeaderPtr (tar_entry_ptr))


#define jr_TarEntryHeaderName(tar_entry_ptr)		jr_TarHeaderName ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderMode(tar_entry_ptr)		jr_TarHeaderMode ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderUID(tar_entry_ptr)			jr_TarHeaderUID ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderGID(tar_entry_ptr)			jr_TarHeaderGID ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderSize(tar_entry_ptr)		jr_TarHeaderSize ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderMTime(tar_entry_ptr)		jr_TarHeaderMTime ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderChkSum(tar_entry_ptr)		jr_TarHeaderChkSum ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderTypeFlag(tar_entry_ptr)	jr_TarHeaderTypeFlag ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderLinkName(tar_entry_ptr)	jr_TarHeaderLinkName ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderMagic(tar_entry_ptr)		jr_TarHeaderMagic ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderVersion(tar_entry_ptr)		jr_TarHeaderVersion ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderUName(tar_entry_ptr)		jr_TarHeaderUName ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderGName(tar_entry_ptr)		jr_TarHeaderGName ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderDevMajor(tar_entry_ptr)	jr_TarHeaderDevMajor ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderDevMinor(tar_entry_ptr)	jr_TarHeaderDevMinor ((tar_entry_ptr)->tar_header)
#define jr_TarEntryHeaderPrefix(tar_entry_ptr)		jr_TarHeaderPrefix ((tar_entry_ptr)->tar_header)


#define jr_TarHeaderName(tar_header_ptr)			((tar_header_ptr)->name)
#define jr_TarHeaderMode(tar_header_ptr)			((tar_header_ptr)->mode)
#define jr_TarHeaderUID(tar_header_ptr)				((tar_header_ptr)->uid)
#define jr_TarHeaderGID(tar_header_ptr)				((tar_header_ptr)->gid)
#define jr_TarHeaderSize(tar_header_ptr)			((tar_header_ptr)->size)
#define jr_TarHeaderMTime(tar_header_ptr)			((tar_header_ptr)->mtime)
#define jr_TarHeaderChkSum(tar_header_ptr)			((tar_header_ptr)->chksum)
#define jr_TarHeaderTypeFlag(tar_header_ptr)		((tar_header_ptr)->typeflag[0])
#define jr_TarHeaderLinkName(tar_header_ptr)		((tar_header_ptr)->linkname)
#define jr_TarHeaderMagic(tar_header_ptr)			((tar_header_ptr)->magic)
#define jr_TarHeaderVersion(tar_header_ptr)			((tar_header_ptr)->version)
#define jr_TarHeaderUName(tar_header_ptr)			((tar_header_ptr)->uname)
#define jr_TarHeaderGName(tar_header_ptr)			((tar_header_ptr)->gname)
#define jr_TarHeaderDevMajor(tar_header_ptr)		((tar_header_ptr)->devmajor)
#define jr_TarHeaderDevMinor(tar_header_ptr)		((tar_header_ptr)->devminor)
#define jr_TarHeaderPrefix(tar_header_ptr)			((tar_header_ptr)->prefix)

#define jr_TarHeaderIsDir(tar_header_ptr)			\
		(jr_TarHeaderTypeFlag (tar_header_ptr) == DIRTYPE)

#define jr_TarHeaderSetTypeFlag(tar_header_ptr, v)	((tar_header_ptr)->typeflag[0] = (v))


extern const char *		jr_TarTypeFlagToString		PROTO ((
							jr_int					type_flag
						));


extern unsigned			jr_int jr_TarModeToPerms (
							jr_int					tar_mode);

#endif
