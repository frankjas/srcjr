#ifndef __jr_syscalls_h___
#define __jr_syscalls_h___

#include "ezport.h"

#include <stdio.h>
#include <sys/types.h>

#include "jr/time.h"


/******** permissions ********/

#define					jr_PERM_S_OWNER			02000
#define					jr_PERM_S_GROUP			01000

#define					jr_PERM_R_OWNER			00400
#define					jr_PERM_W_OWNER			00200
#define					jr_PERM_X_OWNER			00100

#define					jr_PERM_R_GROUP			00040
#define					jr_PERM_W_GROUP			00020
#define					jr_PERM_X_GROUP			00010

#define					jr_PERM_R_OTHER			00004
#define					jr_PERM_W_OTHER			00002
#define					jr_PERM_X_OTHER			00001

#define					jr_PERM_RW_OWNER		(jr_PERM_R_OWNER | jr_PERM_W_OWNER)
#define					jr_PERM_RW_GROUP		(jr_PERM_R_GROUP | jr_PERM_W_GROUP)
#define					jr_PERM_RW_OTHER		(jr_PERM_R_OTHER | jr_PERM_W_OTHER)

#define					jr_PERM_RW_ALL			\
						(jr_PERM_RW_OWNER | jr_PERM_RW_GROUP | jr_PERM_RW_OTHER)

#define					jr_PERM_RWX_OWNER		(jr_PERM_R_OWNER | jr_PERM_W_OWNER | jr_PERM_X_OWNER)
#define					jr_PERM_RWX_GROUP		(jr_PERM_R_GROUP | jr_PERM_W_GROUP | jr_PERM_X_GROUP)
#define					jr_PERM_RWX_OTHER		(jr_PERM_R_OTHER | jr_PERM_W_OTHER | jr_PERM_X_OTHER)

#define					jr_PERM_RWX_ALL			\
						(jr_PERM_RWX_OWNER | jr_PERM_RWX_GROUP | jr_PERM_RWX_OTHER)


extern jr_int			jr_FileNameSetPerms (
							const char *		path,
							jr_int				mode,
							char *				error_buf);

extern jr_int			jr_FileNameGetPerms (
							const char *		path,
							char *				error_buf);

extern jr_int			jr_UnixModeToPerms (
							mode_t				unix_mode);

extern mode_t			jr_PermsToUnixMode (
							unsigned jr_int		perm_flags);

extern unsigned jr_int	jr_PermsFromString (
							const char *		perms_string);

extern jr_int			jr_PermsAddFromString (
							jr_int				curr_perms,
							const char *		perms_string);

#define					jr_PERMS_STRING_LENGTH	9

extern void				jr_PermsToString (
							char *				perm_string,
							unsigned jr_int		perm_flags);

/******** file ownership ********/

extern jr_int			jr_FileNameSetOwner (
							const char *		path,
							const char *		owner_name,
							const char *		group_name,
							char *				error_buf);

extern jr_int			jr_IsChownAllowed();

/******** directories ********/

extern jr_int			jr_chdir (
							const char *				dir_path,
							char *						error_buf);

extern jr_int			jr_getcwd (
							char *						path_buf,
							jr_int						path_buf_size,
							char *						opt_error_buf);

extern char *			jr_getcwd_malloc (
							char *						opt_error_buf);

extern jr_int			jr_MakeDirectoryPath			PROTO((
								const char *			pathname,
								jr_int					perm_flags,
								char *					errbuf
						)) ;



/******** File Descriptor Operations ********/

extern jr_int   		jr_FileDescHasPendingInput		PROTO((jr_int fd)) ;
extern time_t			jr_FilePtrModifiedTime			PROTO((FILE *fp)) ;
extern time_t			jr_FileDescModifiedTime			PROTO((jr_int fd)) ;
extern time_t			jr_FileNameModifiedTime			PROTO((const char *filename)) ;

extern jr_seconds_t		jr_FileNameSetModTime			PROTO((
							const char *				filename,
							jr_seconds_t				mod_time,
							char *						error_buf
						));
extern jr_seconds_t		jr_FileNameModTimeTouch			PROTO((
							const char *				filename,
							char *						error_buf
						));
extern size_t			jr_FilePtrDiskSize				PROTO((FILE *fp)) ;
extern size_t			jr_FileDescDiskSize				PROTO((jr_int fd)) ;
extern size_t			jr_FileNameDiskSize				PROTO((const char *fn)) ;
extern off_t			jr_FileDescBlockSize			PROTO((jr_int fd));




/*
 * These functions added by FJ 4/30/94
 */

extern jr_int			jr_FileDescReadLock				PROTO ((
							jr_int						file_descriptor,
							jr_int						time_out,
							char *						error_buf
						));

extern jr_int			jr_FileDescWriteLock			PROTO ((
							jr_int						file_descriptor,
							jr_int						time_out,
							char *						error_buf
						));

extern void				jr_FileDescUnLock				PROTO ((
							jr_int						file_descriptor
						));

#define					jr_FilePtrReadLock(fp, time_out, error_buf)					\
						jr_FileDescReadLock (fileno (fp), time_out, error_buf)

#define					jr_FilePtrWriteLock(fp, time_out, error_buf)				\
						jr_FileDescWriteLock (fileno (fp), time_out, error_buf)

#define					jr_FilePtrUnLock(fp)										\
						jr_FileDescUnLock (fileno (fp))



extern jr_int			jr_fcntl_flock					PROTO ((
							jr_int						fd,
							jr_int						operation,
							jr_int						is_blocking
						));

					/*
					 * jr_fcntl_flock() lock types are: F_UNLCK, F_RDLCK, F_WRLCK
					 */

#ifdef __jr_compiling_timelock
	extern jr_int		jr_FileDescTimedFlock				PROTO((
								jr_int			fd,
								const char *	how,
								jr_int			time_seconds
						)) ;
#else
	extern jr_int		jr_FileDescTimedFlock				PROTO((
								jr_int			fd,
								const char *	how,
								...
						)) ;
#endif

extern jr_int			jr_FileDescCopy			PROTO((
								jr_int			wfd,
								jr_int			rfd,
								void *			buf,
								jr_int			buffer_length
						)) ;


/******** Non-Blocking I/O ********/

extern jr_int			jr_FileDescInitNonBlockingIO	PROTO ((
							jr_int						fd,
							char *						opt_error_buf
						));

extern jr_int			jr_FileDescUndoNonBlockingIO	PROTO ((
							jr_int						fd,
							char *						opt_error_buf
						));


#define					jr_FilePtrInitNonBlockingIO(fp, opt_error_buf)		\
						jr_FileDescInitNonBlockingIO (fileno (fp), opt_error_buf)

#define					jr_FilePtrUndoNonBlockingIO(fp, opt_error_buf)		\
						jr_FileDescUndoNonBlockingIO (fileno (fp), opt_error_buf)


/******** Asynchronous I/O ********/

extern void				jr_BlockAsyncIO				PROTO ((
							void *					old_mask_ptr
						));

extern void				jr_UnBlockAsyncIO			PROTO ((
							void *					old_mask_ptr
						));

extern jr_int			jr_InitAsyncIO					PROTO ((
							jr_int						fd,
							FILE *						fp,
							void						(*handler_fn) (),
							void *						handler_param,
							jr_int						is_for_reading,
							char *						opt_error_buf
						));

extern jr_int			jr_UndoAsyncIO					PROTO ((
							jr_int						fd,
							jr_int						is_for_reading,
							char *						opt_error_buf
						));

#define					jr_FileDescInitReadAsyncIO(fd, handler_fn, handler_param, opt_error_buf)	\
						jr_InitAsyncIO (fd, 0, handler_fn, handler_param, 1, opt_error_buf)

#define					jr_FileDescUndoReadAsyncIO(fd, opt_eb)	\
						jr_UndoAsyncIO (fd, 1, opt_eb)

#define					jr_FileDescInitWriteAsyncIO(fd, handler_fn, handler_param, opt_error_buf)	\
						jr_InitAsyncIO (fd, 0, handler_fn, handler_param, 0, opt_error_buf)

#define					jr_FileDescUndoWriteAsyncIO(fd, opt_eb)	\
						jr_UndoAsyncIO (fd, 0, opt_eb)


#define					jr_FilePtrInitReadAsyncIO(fp, handler_fn, handler_param, opt_error_buf)	\
						jr_InitAsyncIO (fileno (fp), fp, handler_fn, handler_param, 1, opt_error_buf)

#define					jr_FilePtrUndoReadAsyncIO(fp, opt_eb)	\
						jr_UndoAsyncIO (fileno (fp), 1, opt_eb)

#define					jr_FilePtrInitWriteAsyncIO(fp, handler_fn, handler_param, opt_error_buf)	\
						jr_InitAsyncIO (fileno (fp), fp, handler_fn, handler_param, 0, opt_error_buf)

#define					jr_FilePtrUndoWriteAsyncIO(fp, opt_eb)	\
						jr_UndoAsyncIO (fileno (fp), 0, opt_eb)


						/*
						 * These functions should return a 'pid_t'
						 * but that would require _POSIX_SOURCE to include
						 * this header file
						 */

/******** Terminal Control *******/

extern jr_int			jr_SetTerminalMode				PROTO ((
							jr_int						fd,
							jr_int						mode_constant,
							jr_int						turn_on_mode,
							char *						error_buf
						));

#define					jr_CBREAK_MODE					1

#define					jr_SetCbreakMode(fd, turn_on_mode, error_buf)			\
						jr_SetTerminalMode (fd, jr_CBREAK_MODE, turn_on_mode, error_buf)


extern jr_int			jr_SimulateTerminalInputFromString		PROTO((
								const char *	string_arg
						)) ;


/******** Path Manipulations ********/

extern jr_int			jr_FileNameToAbsolutePath		PROTO((
								char *					result_buf,
								jr_int					result_buf_length,
								const char *			potentially_relative_filename
						)) ;

extern void				jr_FileNameToRelativePath		PROTO((
								char *					target_path_buf,
								jr_int					buflen,
								const char *			source_path_arg,
								const char *			relative_to_here_arg
						)) ;

extern char *			jr_PathSearch					PROTO ((
							char *						file_name,
							char **						dir_vector
						));

extern jr_int			jr_FileNameFindInPathString		PROTO((
								const char *			search_for_file_name,
								const char *			path_string,
								char *					result_buf,
								jr_int					result_buf_length
						)) ;

extern jr_int			jr_FileNameFindInPathVector		PROTO((
								const char *			search_for_file_name,
								char **					path_vector,
								char *					result_buf,
								jr_int					result_buf_length
						)) ;


extern jr_int			jr_PathStringCopyWithDollarAndTildeExpansion	PROTO((
							char *						target_str,
							const char *				source_str
						)) ;

extern char *			jr_NewStringFromDollarAndTildeExpansion			PROTO((
							const char *				word
						)) ;


/******** Canonical Paths ********/

extern jr_int			jr_GetCanonicalPath (
							const char *				user_path,
							char *						path_buf,
							jr_int						path_buf_size,
							char *						error_buf);

extern const char *		jr_MallocCanonicalPath (
							const char *				user_path,
							char *						error_buf);



/******** Temporary Files ********/


extern void				jr_MkTemp				PROTO ((
							char *				tmp_name_template
						));

extern const char *		jr_MkTempInTempDir (
							const char *		tmp_name_template);

extern FILE *			jr_TempFileOpen			PROTO ((
							char *				opt_template,
							char *				error_buf
						));

extern jr_int			jr_GetTempDir			PROTO ((
							char *				tmp_dir_buf,
							jr_int				tmp_dir_buf_length,
							char *				error_buf
						));


/******** File Types ********/

extern jr_int			jr_PathIsDirectory (
							const char *				pathname,
							char *						error_buf) ;

extern jr_int			jr_PathIsLink (
							const char *				pathname,
							char *						error_buf);

extern jr_int			jr_PathIsFile(
							const char *				pathname,
							char *						opt_error_buf);


/******** Moving/Copying/Removing Files ********/

extern jr_int			jr_FileNameSafeCopy		PROTO ((
							const char *		replaced_file,
							const char *		new_data_file,
							FILE *				opt_new_data_file_ptr,
							char *				error_buf
						));

extern jr_int			jr_MoveFileName							PROTO ((
								const char *	old_file_path,
								const char *	new_file_path,
								char *			error_buf
						));
						/*
						** 11/26/05: Copies the file if the rename fails,
						** i.e. if the files are on different devices.
						*/


extern jr_int			jr_MovePath (
							const char *			old_path_name,
							const char *			new_path_name,
							char *					error_buf);
						/*
						** 11/26/05: no copy, so works for directories as well.
						*/

extern jr_int			jr_RemovePath (
							const char *			path_name,
							char *					error_buf);

extern jr_int			jr_Unlink (
							const char *			file_path,
							char *					opt_error_buf);

extern jr_int			jr_UnlinkNoApr (
							const char *			file_path,
							char *					opt_error_buf);

extern jr_int			jr_SymLink (
							const char *			link_target_path,
							const char *			new_link_path,
							char *					error_buf);

extern jr_int			jr_ReadLink (
							const char *			link_path,
							char *					link_target_buf,
							jr_int					buf_length,
							char *					error_buf);

extern jr_int			jr_FileExists(
							const char *			file_path);

extern jr_int			jr_FileNameSetMtime (
							const char *			file_path,
							jr_seconds_t			time_sec,
							char *					error_buf);

extern jr_int			jr_FileNameGetMtime (
							const char *			file_path,
							jr_seconds_t *			time_sec_ref,
							char *					error_buf);

/******** User Names and attributes ********/

extern jr_int			jr_CheckUnixPassword	PROTO((
							char *				username,
							char *				unencrypted_password_text,
							void *				pw_entry
						)) ;

extern jr_int			jr_GetUserName			PROTO ((
								char *			namebuf,
								jr_int			buf_length,
								char *			error_buf
						));

extern jr_int			jr_GetGroupName			PROTO ((
								char *			namebuf,
								jr_int			buf_length,
								char *			error_buf
						));

extern jr_int			jr_GetUserHomeDir		PROTO ((
							char *				path_buf,
							jr_int				path_buf_length,
							char *				error_buf
						));

extern jr_int			jr_IsRootUser			PROTO ((void));


/******** Signals ********/

extern void				jr_SigChildHandler		PROTO ((jr_int signum));
extern char *			jr_SignalName			PROTO ((unsigned jr_int signum));


/******** Asynchronous Function Calls ********/

extern jr_int			jr_AddAsyncFunction (
							void						(*handler_fn) (),
							void *						handler_param,
							jr_int						num_seconds,
							jr_int						do_repeat,
							char *						error_buf);

extern void				jr_RemoveAsyncFunction (
							void						(*handler_fn) (),
							void *						handler_param);

extern jr_int			jr_GetAsyncFunctionNumSeconds (
							void						(*handler_fn) (),
							void *						handler_param);

extern jr_int			jr_GetAsyncFunctionCallTime (
							void						(*handler_fn) (),
							void *						handler_param);


/******** executing commands ********/

#define					jr_EXEC_ERROR					-1
#define					jr_EXEC_SPAWN_ERROR				-2
#define					jr_EXEC_RUN_ERROR				-3
#define					jr_EXEC_INTERNAL_ERROR			-4

extern jr_int			jr_exec_cmd (
							const char *				command_str,
							char *						error_buf);

extern FILE *			jr_popen (
							const char *				command_str,
							const char *				mode_str,
							char *						error_buf);

extern jr_int			jr_pclose (
							FILE *						fp,
							char *						error_buf);


extern char *			jr_NewStringFilteredFromCommandVector	PROTO((
							char **			argv,
							const char *	input_str,
							char *error_buf
						));

extern jr_int			jr_Invoke1WayCommandVector			PROTO ((
							char **							argv,
							FILE **							rfp_ptr,
							char *							error_buf
						));

extern jr_int			jr_Invoke1WayCommandVectorWithStderr	PROTO ((
							char **							argv,
							FILE **							rfp_ptr,
							char *							error_buf
						));

extern jr_int			jr_InvokeCommandVector				PROTO ((
							char **							argv,
							FILE *							fp_array[],
							jr_int							num_fps,
							char *							error_buf
						));

#define					jr_Invoke2WayCommandVector(argv, fps, eb)		\
						jr_InvokeCommandVector (argv, fps, 2, eb)

#define					jr_Invoke2WayCommandVectorWithStderr(argv, fps, eb)		\
						jr_InvokeCommandVector (argv, fps, 3, eb)


/******** Miscellaneous ********/

extern void				jr_critical_section				PROTO ((
							char *						how,
							void *						opt_old_mask_ptr
						));

extern char *			jr_ExitStatusToString			PROTO ((
							char *						status_buf,
							jr_int						status
						));

extern jr_int			jr_uninterrupted_sleep			PROTO ((
							jr_int						sleep_sec,
							jr_int						sleep_usec
						));

extern void				jr_sleep						PROTO ((
							jr_int						sleep_sec,
							jr_int						sleep_usec
						));

extern jr_int			jr_getpid						(void);

extern jr_int			jr_IsTapeFilePtr				PROTO ((
							FILE *						fp,
							char *						error_buf
						));

extern jr_int			jr_IsInteractiveFilePtr			PROTO ((
							FILE *						fp,
							char *						error_buf
						));

extern jr_int			jr_gethostname(
							char *						host_name,
							jr_int						buf_length,
							char *						error_buf);

#define					jr_MAXHOSTNAME					128

#endif
