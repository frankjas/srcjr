#define _POSIX_SOURCE 1

#include "ezport.h"

#include <errno.h>

#include "jr/mime.h"
#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/string.h"
#include "jr/vector.h"
#include "jr/misc.h"

void jr_MimeTableInit (mime_table_ptr)
	jr_MimeTableType *			mime_table_ptr;
{
	jr_HTableInit (
		mime_table_ptr->str_table,
		sizeof (jr_MimeInfoType), jr_MimeInfoHash, jr_MimeInfoCmp
	);
}

void jr_MimeTableUndo (mime_table_ptr)
	jr_MimeTableType *			mime_table_ptr;
{
	jr_MimeInfoType *			mime_info_ptr;


	jr_HTableForEachElementPtr (mime_table_ptr->str_table, mime_info_ptr) {
		jr_MimeInfoUndo (mime_info_ptr);
	}

	jr_HTableUndo (mime_table_ptr->str_table);
}


jr_int jr_MimeTableAddNewElement (
									mime_table_ptr, file_ext_str, mime_type_str,
									file_name, line_number, error_buf
								)
	jr_MimeTableType *			mime_table_ptr;
	const char *				file_ext_str;
	const char *				mime_type_str;
	const char *				file_name;
	jr_int						line_number;
	char *						error_buf;
{
	jr_MimeInfoType				tmp_mime_info [1];
	jr_MimeInfoType *			mime_info_ptr;

	jr_MimeInfoInit (tmp_mime_info, file_ext_str, mime_type_str, file_name, line_number);

	mime_info_ptr	= jr_HTableFindElementPtr (mime_table_ptr->str_table, tmp_mime_info);

	if (mime_info_ptr) {

		jr_MimeInfoUndo (tmp_mime_info);

		if (strcasecmp (jr_MimeInfoMimeTypeString (mime_info_ptr), mime_type_str)  ==  0) {
			return 0;
		}

		jr_esprintf (error_buf, "'%.64s' (on %.64s:%d)",
			jr_MimeInfoMimeTypeString (mime_info_ptr),
			jr_MimeInfoFileName (mime_info_ptr),
			jr_MimeInfoLineNumber (mime_info_ptr)
		);
		return -1;
	}

	jr_HTableSetNewElement (mime_table_ptr->str_table, tmp_mime_info);

	return 0;
}

const char *jr_MimeTableFindMimeType(
	jr_MimeTableType *			mime_table_ptr,
	const char *				file_ext_str)
{
	jr_MimeInfoType				tmp_mime_info [1];
	jr_MimeInfoType *			mime_info_ptr;

	jr_MimeInfoInit (tmp_mime_info, file_ext_str, 0, 0, 0);

	mime_info_ptr	= jr_HTableFindElementPtr (mime_table_ptr->str_table, tmp_mime_info);

	if (mime_info_ptr) {
		return jr_MimeInfoMimeTypeString( mime_info_ptr);
	}
	return 0;
}

jr_int jr_MimeTableLoadFromFileName (mime_table_ptr, rfn, mode, error_buf)
	jr_MimeTableType *			mime_table_ptr;
	char *						rfn;
	jr_int						mode;
	char *						error_buf;
{
	FILE *						rfp					= 0;
	jr_int						status;

	if (strcmp(rfn, "-") == 0) {
		status = jr_MimeTableLoadFromFilePtr (mime_table_ptr, stdin, "{stdin}", mode, error_buf) ;

		if (status != 0) {
			goto return_status;
		}
	}
	else {
		rfp = fopen(rfn, "r") ;

		if (rfp == NULL) {
			jr_esprintf (error_buf, "couldn't open for reading: %s",
				strerror (errno)
			);
			status = -1;
			goto return_status;
		}

		status = jr_MimeTableLoadFromFilePtr (mime_table_ptr, rfp, rfn, mode, error_buf) ;

		if (status != 0) {
			goto return_status;
		}

		fclose(rfp) ;
		rfp = 0;
	}

	status = 0;

	return_status : {
		if (rfp) {
			fclose (rfp);
		}
	}
	return status;
}


jr_int jr_MimeTableLoadFromFilePtr (mime_table_ptr, rfp, rfn, mode, error_buf)
	jr_MimeTableType *			mime_table_ptr;
	FILE *						rfp;
	char *						rfn;
	jr_int						mode;
	char *						error_buf;
{
	jr_int						line_number;
	char						line_buffer [1024];

	char *						start_ptr;
	const char **				word_vector					= 0;

	const char *				mime_type_str;
	const char *				file_ext_str;

	jr_int						k;
	
	jr_int						status;


	for (	line_number = 1;
			fgets (line_buffer, sizeof (line_buffer), rfp) != NULL;
			line_number ++) {

		start_ptr	= jr_FirstNonWhiteSpacePtr (line_buffer);

		if (*start_ptr == jr_MIME_FILE_LINE_COMMENT_CHAR) {
			continue;
		}

		word_vector	= jr_VectorCreateWithWords (start_ptr, jr_MIME_FILE_WORD_DELIMITERS);

		if (jr_VectorLength (word_vector)  >  1) {

			mime_type_str	= word_vector [0];

			for (k=1;  word_vector[k];  k++) {
				file_ext_str	= word_vector[k];

				status	= jr_MimeTableAddNewElement (
							mime_table_ptr,
							file_ext_str, mime_type_str,
							rfn, line_number,
							error_buf
						);

				if (status != 0) {
					if (mode & jr_MIME_TABLE_FLAG_REDEFINITIONS) {

						jr_esprintf (error_buf,
							"%.64s:%d: file ext. (%.16s, %.32s): %.100s", 
							rfn, line_number, file_ext_str, mime_type_str, error_buf
						);

						if (mode & jr_MIME_TABLE_EXIT_ON_ERROR) {
							goto return_status;
						}
						else {
							fprintf (stderr, "Warning: %18s:%-4d: file ext. (%s, %s) == %s\n",
								rfn, line_number, file_ext_str, mime_type_str, error_buf
							);
						}
					}
				}
			}
		}

		jr_VectorDestroy (word_vector);
		word_vector = 0;
	}

	status	= 0;

	return_status : {
		if (word_vector) {
			jr_VectorDestroy (word_vector);
		}
	}
	return status;
}


void jr_MimeTablePrintForPhp (mime_table_ptr, var_name, indent_str, wfp)
	jr_MimeTableType *			mime_table_ptr;
	const char *				var_name;
	const char *				indent_str;
	FILE *						wfp;
{
	jr_MimeInfoType *			mime_info_ptr;
	jr_int						printed_entry			= 0;

	fprintf (wfp, "%s$jr_mime_types = array (\n", indent_str);

	jr_MimeTableForEachMimeInfoPtr (mime_table_ptr, mime_info_ptr) {
		if (printed_entry) {
			fprintf (wfp, ",\n");
		}
		fprintf (wfp, "%s\t'%s' => '%s'",
			indent_str,
			jr_MimeInfoFileExtString (mime_info_ptr),
			jr_MimeInfoMimeTypeString (mime_info_ptr)
		);
		printed_entry = 1;
	}
	if (printed_entry) {
		fprintf (wfp, "\n");
	}
	fprintf (wfp, "%s)\n", indent_str);
}

void jr_MimeInfoInit (mime_info_ptr, file_ext_str, opt_mime_type_str, opt_file_name, opt_line_number)
	jr_MimeInfoType *			mime_info_ptr;
	const char *				file_ext_str;
	const char *				opt_mime_type_str;
	const char *				opt_file_name;
	jr_int						opt_line_number;
{
	memset (mime_info_ptr, 0, sizeof (*mime_info_ptr));

	mime_info_ptr->file_ext_str			= jr_strdup (file_ext_str);

	if (opt_mime_type_str) {
		mime_info_ptr->mime_type_str	= jr_strdup (opt_mime_type_str);
	}
	if (opt_file_name) {
		mime_info_ptr->file_name		= jr_strdup (opt_file_name);
	}
	mime_info_ptr->line_number			= opt_line_number;
}


void jr_MimeInfoUndo (mime_info_ptr)
	jr_MimeInfoType *			mime_info_ptr;
{
	if (mime_info_ptr->file_ext_str) {
		jr_free (mime_info_ptr->file_ext_str);
	}
	if (mime_info_ptr->mime_type_str) {
		jr_free (mime_info_ptr->mime_type_str);
	}
	if (mime_info_ptr->file_name) {
		jr_free (mime_info_ptr->file_name);
	}
}

jr_int jr_MimeInfoCmp (void_ptr_1, void_ptr_2)
	const void *				void_ptr_1;
	const void *				void_ptr_2;
{
	const jr_MimeInfoType *		mime_info_ptr_1			= void_ptr_1;
	const jr_MimeInfoType *		mime_info_ptr_2			= void_ptr_2;

	jr_int						diff;

	diff	= strcasecmp (mime_info_ptr_1->file_ext_str, mime_info_ptr_2->file_ext_str);

	if (diff != 0) {
		return diff;
	}

	return 0;
}

jr_int jr_MimeInfoHash (void_ptr_1)
	const void *				void_ptr_1;
{
	const jr_MimeInfoType *		mime_info_ptr_1			= void_ptr_1;

	jr_int						hash_value				= 0;

	hash_value	+= jr_strhash_pjw (mime_info_ptr_1->file_ext_str);

	return hash_value;
}
