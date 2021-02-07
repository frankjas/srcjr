#include "ezport.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "jr/apr.h"
#include "jr/syscalls.h"
#include "jr/io.h"
#include "jr/alist.h"
#include "jr/malloc.h"
#include "jr/error.h"

jr_int jr_PathDbMakePath (
	const char *				base_path,
	jr_int						numeric_value,
	jr_int						value_base,
	jr_int						path_flags,
	const char **				balanced_path_ref,
	char *						error_buf)
{
	FILE *						conf_fp				= 0;
	jr_int						conf_is_locked		= 0;
	char *						conf_path			= 0;
	jr_int						max_value			= -1;

	char *						balanced_path		= 0;
	char *						old_path			= 0;
	char *						new_path			= 0;
	char *						tmp_dir				= 0;
	char *						old_mod_path		= 0;
	char *						new_mod_path		= 0;

	jr_int						initialize_conf		= 0;

	jr_dir_t					dir_info[1];

	jr_int						curr_digit;
	jr_AList					path_buf[1];
	char						digit_buf[32];
	jr_int						num_digits;
	jr_int						max_num_digits;

	jr_int						k;
	jr_int						nitems;
	jr_int						status;


	jr_AListInit (path_buf, sizeof (char));
	jr_dir_init (dir_info);

	/*
	** 11/26/06: Copy the base path to the char list (no null)
	*/

	for (k=0;  base_path[k];  k++) {
		jr_AListNativeSetNewTail (path_buf, base_path[k], char);
	}

	if (jr_AListNativeTail (path_buf, char)  ==  '/') {
		jr_AListDeleteTail (path_buf);
	}


	/*
	** 11/26/05: Open and lock the conf file, reading the current maximum value
	*/

	conf_path	= jr_malloc (strlen (base_path) + 1 + strlen (jr_PATH_DB_CONF_FILE) + 1);
	sprintf (conf_path, "%s/%s", base_path, jr_PATH_DB_CONF_FILE);

	conf_fp		= fopen (conf_path, "r+");

	if (conf_fp == 0  &&  errno == ENOENT) {
		conf_fp		= fopen (conf_path, "w+");
	}

	if (conf_fp == 0) {
		jr_esprintf (error_buf, "couldn't open '%.64s': %s", conf_path, strerror (errno));
		status = jr_ConvertErrno (errno);
		goto return_status;
	}

	if (path_flags & jr_PATH_DB_CREATE_FLAG) {
		status		= jr_FilePtrWriteLock (conf_fp, 5, error_buf);
	}
	else {
		status		= jr_FilePtrReadLock (conf_fp, 5, error_buf);
	}

	if (status != 0) {
		jr_esprintf (error_buf, "couldn't lock '%.64s': %s", conf_path, strerror (errno));
		goto return_status;
	}

	conf_is_locked	= 1;

	max_value		= -1;

	{
		char			line_buffer[256];

		fseek (conf_fp, 0, SEEK_SET);

		while (fgets (line_buffer, sizeof (line_buffer), conf_fp)  !=  NULL) {
			nitems	= sscanf (line_buffer, "max. value: %d", &max_value);

			if (nitems != 1) {
				jr_esprintf (error_buf, "badly formatted conf. file '%.64s'", conf_path);
				status	= jr_INTERNAL_ERROR;
				goto return_status;
			}
		}
	}

	if (max_value < 0) {
		max_value	= 0;
		initialize_conf	= 1;
	}


	if (value_base > 36) {
		jr_esprintf (error_buf, "value base (%d) is too large, max. is 36", value_base);
		status = jr_INTERNAL_ERROR;
		goto return_status;
	}

	/*
	** 11/26/05: figure out the digits in reverse order.
	*/
	{
		jr_int			max_quotient;
		jr_int			curr_quotient;

		max_quotient	= max_value;
		curr_quotient	= numeric_value;
		num_digits		= 0;
		max_num_digits	= 0;

		for (; max_quotient > 0  ||  curr_quotient > 0;  num_digits++) {

			if (curr_quotient) {
				curr_digit	= curr_quotient % value_base;
			}
			else {
				curr_digit	= 0;
			}

			if (value_base <= 10) {
				digit_buf[num_digits] = '0' + curr_digit;
			}
			else {
				digit_buf[num_digits] = 'a' + curr_digit;
			}

			if (curr_quotient) {
				curr_quotient	= curr_quotient / value_base;
			}
			if (max_quotient) {
				max_quotient	= max_quotient / value_base;
				max_num_digits ++;
			}
		}
		if (max_value == 0  &&  numeric_value == 0) {
			digit_buf[num_digits] = '0';
			num_digits++;
		}
	}

	/*
	** 11/26/05: Add the digits to the char list, creating the desired path.
	*/

	for (k=num_digits - 1;  k >= 0;  k--) {
		curr_digit		= digit_buf[k];

		jr_AListNativeSetNewTail (path_buf, '/', char);
		jr_AListNativeSetNewTail (path_buf, curr_digit, char);
	}
	jr_AListNativeSetNewTail (path_buf, 0, char);


	balanced_path	= jr_strdup (jr_AListHeadPtr (path_buf));

	if (path_flags & jr_PATH_DB_CREATE_FLAG) {
		char *			cptr;

		if (num_digits > max_num_digits  &&  num_digits > 1) {
			jr_int			num_new_levels	= num_digits - max_num_digits;
			jr_int			k;
			jr_int			q;

			/*
			** 1-18-2011: move the top-level directory/files down one level
			** i.e. under a new top-level "0/" directory.  Then keep adding
			** levels until the right number of "0" levels are prepended.
			*/
			tmp_dir			= jr_malloc (strlen (base_path) + 1 + 7 + 1);

			sprintf (tmp_dir, "%s/nXXXXXX", base_path);

			jr_MkTemp (tmp_dir);


			status	= jr_MakeDirectoryPath (tmp_dir, jr_PERM_RWX_ALL, error_buf);

			if (status != 0) {
				jr_esprintf (error_buf, "couldn't make temp. dir '%.64s': %s", tmp_dir, error_buf);
				goto return_status;
			}

			old_path	= jr_malloc (strlen (base_path) + 1 + 1 + 1);
			new_path	= jr_malloc (strlen (tmp_dir) + 1 + 1 + 1);

			for (k=0;  k < num_new_levels;  k++) {

				for (q=0; q < value_base; q++) {
					sprintf (old_path, "%s/%c", base_path, '0' + q);
					sprintf (new_path, "%s/%c", tmp_dir, '0' + q);

					status	= jr_MovePath (old_path, new_path, error_buf);

					if (status != 0  &&  status != jr_NOT_FOUND_ERROR) {
						jr_esprintf (error_buf, "couldn't move '%.64s' to '%.64s': %s",
							old_path, new_path, error_buf
						);
						goto return_status;
					}
				}
				if (max_num_digits == 1  &&  k == 0) {
					/*
					** 1-18-2011: going from one digit to many, i.e. adding the first layer
					** of directories.  Move all the files in the current directory
					** starting with old_path to the new '0' directory.
					** Rationale: PathDb returns the old path and the caller may have
					** appended other chars onto it.
					*/
					const char *			entry_path;
					jr_int					nitems;
					char					first_char;

					status	= jr_dir_open (dir_info, base_path, error_buf);

					if (status != 0) {
						jr_esprintf (error_buf, "couldn't open '%.64s': %s", base_path, error_buf);
						goto return_status;
					}

					while (1) {
						status	= jr_dir_read (dir_info, &entry_path, 0, error_buf);

						if (status != 0) {
							if (status == jr_NOT_FOUND_ERROR) {
								break;
							}
							goto return_status;
						}

						nitems	= sscanf( entry_path, "%c", &first_char);

						if (nitems != 1) {
							continue;
						}

						if (first_char >= '0'  &&  first_char <= '0' + value_base - 1) {
							if (old_mod_path) {
								jr_free( old_mod_path);
							}
							if (new_mod_path) {
								jr_free( new_mod_path);
							}

							old_mod_path	= jr_malloc( strlen(base_path) + 1 + strlen(entry_path) + 1);
							new_mod_path	= jr_malloc( strlen(tmp_dir) + 1 + strlen(entry_path) + 1);

							sprintf( old_mod_path, "%s/%s", base_path, entry_path);
							sprintf( new_mod_path, "%s/%s", tmp_dir, entry_path);

							status	= jr_MovePath (old_mod_path, new_mod_path, error_buf);

							if (status != 0) {
								jr_esprintf (error_buf, "couldn't move '%.64s' to '%.64s': %s",
									old_mod_path, new_mod_path, error_buf
								);
								goto return_status;
							}
						}
					}

				}
				sprintf (new_path, "%s/0", base_path);

				status		= jr_MovePath (tmp_dir, new_path, error_buf);

				if (status != 0  &&  status != jr_NOT_FOUND_ERROR) {
					jr_esprintf (error_buf, "couldn't move '%.64s' to '%.64s': %s",
						old_path, new_path, error_buf
					);
					goto return_status;
				}
			}
		}
		if (path_flags & jr_PATH_DB_FILE_FLAG) {
			cptr		= strrchr (balanced_path, '/');

			if (cptr) {
				*cptr	= 0;
			}
		}

		status	= jr_MakeDirectoryPath (balanced_path, jr_PERM_RWX_ALL, error_buf);

		if (status != 0) {
			jr_esprintf (error_buf, "couldn't make new path '%.64s': %s", tmp_dir, error_buf);
			goto return_status;
		}

		if (path_flags & jr_PATH_DB_FILE_FLAG) {
			if (cptr) {
				*cptr	= '/';
			}
		}

		if (numeric_value > max_value  || initialize_conf) {
			fseek (conf_fp, 0, SEEK_SET);

			fprintf (conf_fp, "max. value: %d\n", numeric_value);
		}
	}

	status				= 0;

	*balanced_path_ref	= balanced_path;
	balanced_path		= 0;

	return_status : {
		jr_AListUndo (path_buf);
		jr_dir_undo (dir_info);

		if (conf_path) {
			jr_free (conf_path);
		}
		if (conf_fp) {
			if (conf_is_locked) {
				jr_FilePtrUnLock (conf_fp);
			}
			fclose (conf_fp);
		}
		if (balanced_path) {
			jr_free (balanced_path);
		}
		if (tmp_dir) {
			jr_free (tmp_dir);
		}
		if (old_path) {
			jr_free (old_path);
		}
		if (new_path) {
			jr_free (new_path);
		}
		if (old_mod_path) {
			jr_free (old_mod_path);
		}
		if (new_mod_path) {
			jr_free (new_mod_path);
		}
	}

	return status;
}


#ifdef __garbage__
	for (k=num_digits - 1;  k >= 0;  k--) {
		curr_digit		= digit_buf[k];

		jr_AListNativeSetNewTail (path_buf, '/', char);
		jr_AListNativeSetNewTail (path_buf, curr_digit, char);
		jr_AListNativeSetNewTail (path_buf, 0, char);

		status	= jr_stat (jr_AListHeadPtr (path_buf), stat_info, error_buf);

		if (status == jr_NOT_FOUND_ERROR) {
			if (jr_path_flags & jr_PATH_DB_CREATE) {
				if (k != 0) {
					status	= jr_MakeDirectoryPath (jr_AListHeadPtr (path_buf), error_buf);

					if (status != 0) {
						jr_esprintf (error_buf, "couldn't create dir '%.64s': %s",
							jr_AListHeadPtr (path_buf), error_buf
						);
						goto return_status;
					}
				}
			}
			else {
				jr_esprintf (error_buf, "not found");
				goto return_status;
			}
		}
		else if (status < 0) {
			jr_esprintf (error_buf, "couldn't access '%.64s': %s",
				jr_AListHeadPtr (path_buf), error_buf
			);
			goto return_status;
		}
		else if (jr_stat_is_directory (stat_info)) {
			jr_AListDeleteTail (path_buf);
			/*
			** 11/25/05: get rid of the null
			*/

			if (k == 0) {
				jr_AListNativeSetNewTail (path_buf, '/', char);
				jr_AListNativeSetNewTail (path_buf, 0, char);
				break;
			}
			/*
			** 11/25/05: Traverse downwards
			*/
		}
		else {
			/*
			** 11/26/05: The current path is a file
			*/

			if (k == 0) {
				if (path_flags & jr_PATH_DB_FILE_FLAG) {
					break;
				}
				jr_esprintf (error_buf, "desired path is not a directory (%.64s)",
					jr_AListHeadPtr (path_buf)
				);
				status = -1;
				goto return_status;
			}
			if (path_flags  &  jr_PATH_DB_CREATE_FLAG) {
				/*
				** 11/25/05: it's a file and this value has more digits,
				** -> this value is greater than anything stored before
				*/
				if (open_flags & jr_PATH_DB_DIRECTORY_FLAG) {
					status	= jr_MakeDirectoryPath (
								jr_AListHeadPtr (path_buf), jr_PERMS_RWX_ALL, error_buf
							);
				}
				continue;
			}
		}
	}
#endif
