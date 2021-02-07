#include "ezport.h"

extern "C" {
#include "esis_prj.h"
}
#include "jr/esis/io.hpp"


void jr_ESIS_InputInit (jr_ESIS_InputClass *esis_info)
{
	memset (esis_info, 0, sizeof (jr_ESIS_InputClass));

	jr_HTableInit			(	esis_info->keyword_table, sizeof (jr_ESIS_KeywordType),
								jr_ESIS_KeywordHash, jr_ESIS_KeywordCmp
							);

	jr_AttributeManagerInit	(esis_info->attr_manager);
	jr_AttributeListInit	(esis_info->attr_list, esis_info->attr_manager);
}

void jr_ESIS_InputUndo (jr_ESIS_InputClass *esis_info)
{
	jr_ESIS_KeywordType *			keyword_info;

	jr_AttributeManagerUndo (esis_info->attr_manager);
	jr_AttributeListUndo (esis_info->attr_list);

	jr_HTableForEachElementPtr (esis_info->keyword_table, keyword_info) {
		jr_ESIS_KeywordUndo (keyword_info);
	}
	jr_HTableUndo (esis_info->keyword_table);

	if (esis_info->file_name) {
		jr_free (esis_info->file_name);
	}
}

void *jr_ESIS_InputClass::operator new (
	size_t						size,
	jr_ESIS_InputClass *		esis_info)
{
	return esis_info;
}

void jr_ESIS_InputDefineKeyword (
	jr_ESIS_InputClass *		esis_info,
	char *						name,
	jr_int						keyword_number)
{
	jr_ESIS_KeywordType			keyword_info[1];
	jr_int						index;


	jr_ESIS_KeywordInit (keyword_info, name, keyword_number);

	index = jr_HTableSetNewElementIndex (esis_info->keyword_table, keyword_info);

	if (index < 0) {
		jr_ESIS_KeywordUndo (keyword_info);
	}
}

jr_int jr_ESIS_InputGetKeywordNumber (
	jr_ESIS_InputClass *		esis_info,
	const char *				name)
{
	jr_ESIS_KeywordType			tmp_keyword_info[1];
	jr_ESIS_KeywordType *		keyword_info;


	jr_ESIS_KeywordInit (tmp_keyword_info, name, 0);

	keyword_info	= (jr_ESIS_KeywordType *) jr_HTableFindElementPtr (
						esis_info->keyword_table, tmp_keyword_info
					);

	jr_ESIS_KeywordUndo (tmp_keyword_info);

	if (keyword_info) {
		return keyword_info->keyword_number;
	}
	return 0;
}


void jr_ESIS_InputClass::add_attribute (
	char *					name_str,
	char *					value_str)
{
}

void jr_ESIS_InputClass::set_line_number (jr_int line_number, char *opt_file_name)
{
	this->line_number		= line_number;

	if (opt_file_name) {
		this->file_name		= jr_strdup (opt_file_name);
	}
}


jr_int jr_ESIS_InputReadFromFileName (
	jr_ESIS_InputClass *		esis_info,
	char *						rfn,
	char *						error_buf)
{
	FILE *						rfp;
	jr_int						status;


	rfp		= fopen (rfn, "r");

	if (rfp == 0) {
		char					file_name_buf[256];

		sprintf (file_name_buf, "%.220s.esis", rfn);

		rfp	= fopen (file_name_buf, "r");

		if (rfp == 0) {
			jr_esprintf (error_buf, "couldn't open '%.50s' for reading: %s",
				rfn, strerror (errno)
			);
			return -1;
		}
	}

	status =  jr_ESIS_InputReadFromFilePtr (esis_info, rfp, rfn, error_buf);

	fclose (rfp);

	if (status != 0) {
		return -1;
	}
	return 0;
}

jr_int jr_ESIS_InputReadFromFilePtr (
	jr_ESIS_InputClass *		esis_info,
	FILE *						rfp,
	char *						rfn,
	char *						error_buf)
{
	jr_int						c;
	jr_AList					char_list[1];

	jr_int						line_number				= 1;

	jr_int						num_errors				= 0;
	jr_int						return_status			= 0;

	jr_int						status;


	jr_AListInit (char_list, sizeof (char));


	for (line_number=1; (c = getc (rfp))  !=  EOF; line_number++) {
		switch (c) {
			case '('		: {			/* start element */
				char *		element_name;
				jr_int		keyword_number;

				element_name	= jr_ESIS_ReadWord (rfp, char_list);
				keyword_number	= jr_ESIS_InputGetKeywordNumber (esis_info, element_name);
				
				status			= esis_info->element_begin (
									element_name, keyword_number, error_buf
								);

				jr_AttributeListEmpty (esis_info->attr_list);

				if (status != 0) {
					goto bad_status;
				}
				break;
			}

			case ')'		: {			/* end element */
				char *			element_name;
				jr_int			keyword_number;


				element_name	= jr_ESIS_ReadWord (rfp, char_list);
				keyword_number	= jr_ESIS_InputGetKeywordNumber (esis_info, element_name);

				status			= esis_info->element_end (
									element_name, keyword_number, error_buf
								);

				if (status != 0) {
					goto bad_status;
				}

				break;
			}

			case '-'		: {			/* text data */
				char *		element_data;

				element_data	= jr_ESIS_ReadData (rfp, char_list);

				status			= esis_info->element_data (element_data, error_buf);

				if (status != 0) {
					goto bad_status;
				}

				break;
			}

			case '&'		: {			/* external entity reference */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case '?'		: {			/* processing instruction */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'A'		: {			/* attribute definition */
				char *		attr_name;
				char *		attr_type;
				char *		attr_value;
				jr_int		type_initial_1;
				jr_int		type_initial_2;

				jr_int		add_attr				= 0;


				attr_name		= jr_ESIS_SaveWord (rfp, char_list);
				attr_type		= jr_ESIS_ReadWord (rfp, char_list);
				type_initial_1	= toupper (attr_type[0]);
				type_initial_2	= toupper (attr_type[1]);

				if (type_initial_1 == 'I'  &&  type_initial_2 == 'M') {
					attr_value		= 0;
				}
				else {
					attr_value		= jr_ESIS_ReadData (rfp, char_list);
				}

				switch (type_initial_1) {
					case 'I'		: {
						if (type_initial_2  ==  'M') {
							add_attr = 1;
						}
						else {
						}
						break;
					}
					case 'C'		: {
						add_attr = 1;
						break;
					}
					case 'N'		: {
						break;
					}
					case 'E'		: {
						break;
					}
					case 'T'		: {
						add_attr = 1;
						break;
					}
				}

				if (add_attr) {
					if (attr_value) {
						jr_AttributeListAddAttribute (esis_info->attr_list, attr_name, attr_value);
					}
					esis_info->add_attribute (attr_name, attr_value);
				}


				jr_free (attr_name);
				break;
			}

			case 'D'		: {			/* data attribute for external entity */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'a'		: {			/* link attribute */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'N'		: {			/* notation definition */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'E'		: {			/* external data entity */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'I'		: {			/* internal data entity */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'S'		: {			/* sub-document entity */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'T'		: {			/* external SGML text entity */
				jr_esprintf (error_buf, "%s:%d: ESIS command '%c' not implemented",
					rfn, line_number, c
				);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 's'		: {			/* system identifier */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'p'		: {			/* public identifier */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 't'		: {			/* generated system identifier */
				jr_esprintf (error_buf, "%s:%d: ESIS command '%c' not implemented",
					rfn, line_number, c
				);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case '{'		: {			/* sub-document start */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case '}'		: {			/* sub-document end */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'L'		: {			/* line and file */
				char *		line_number_str;
				jr_int		is_last;
				jr_int		curr_line_number;
				char *		file_name			= 0;

				line_number_str		= jr_ESIS_ReadWordWithIsLast (rfp, char_list, &is_last);

				curr_line_number		= atoi (line_number_str);

				if (!is_last) {
					file_name	= jr_ESIS_ReadData (rfp, char_list);
				}

				esis_info->set_line_number (curr_line_number, file_name);
					
				break;
			}

			case '#'		: {			/* APPINFO parameter */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'C'		: {			/* was conforming document */
				jr_ESIS_ReadData (rfp, char_list);
				esis_info->is_conforming = 1;

				esis_info->set_is_conforming (1);

				break;
			}

			case 'i'		: {			/* next element is included sub-element */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}

			case 'e'		: {			/* next element has EMPTY content */
				jr_esprintf (error_buf, "ESIS command '%c' not implemented", c);
				status = jr_ESIS_NOT_IMPLEMENTED_ERROR;
				goto bad_status;
				break;
			}


			default			: {
				jr_esprintf (error_buf, "illegal leading character '%c' (ASCII %d)",
					c, c
				);
				status = jr_ESIS_ILLEGAL_INPUT_ERROR;
				goto bad_status;
			}
		}

		if (0) bad_status : {
			num_errors++;

			if (return_status  == 0) {
				return_status	= status;
			}

			if (esis_info->max_num_errors  >  1) {
				char *		error_file_name		= rfn;
				jr_int		error_line_number	= line_number;

				if (jr_ESIS_InputFileName (esis_info)) {
					error_file_name				= jr_ESIS_InputFileName (esis_info);
					error_line_number			= jr_ESIS_InputLineNumber (esis_info);
				}
				jr_esprintf (error_buf, "%.40s:%d: %s",
					error_file_name, error_line_number, error_buf
				);
			}

			if (num_errors  >= esis_info->max_num_errors) {
				goto return_status;
			}
		}
	}

	if (! jr_ESIS_InputIsConforming (esis_info)) {

		esis_info->set_is_conforming (0);

		jr_esprintf (error_buf, "not conforming");
		status = jr_ESIS_NOT_CONFORMING_ERROR;
		goto return_status;
	}

	return_status : {
		jr_AListUndo (char_list);
	}

	if (return_status != 0   &&  esis_info->max_num_errors > 1) {
		jr_esprintf (error_buf, "processing errors encountered");
	}
	else {
		char *		error_file_name		= rfn;
		jr_int		error_line_number	= line_number;

		if (jr_ESIS_InputFileName (esis_info)) {
			error_file_name				= jr_ESIS_InputFileName (esis_info);
			error_line_number			= jr_ESIS_InputLineNumber (esis_info);
		}
		jr_esprintf(error_buf, "%.40s:%d: %s",
			error_file_name, error_line_number, error_buf
		);
	}

	return return_status;
}


void jr_ESIS_KeywordInit (
	jr_ESIS_KeywordType *		keyword_info,
	const char *				name_str,
	jr_int						keyword_number)
{
	memset (keyword_info, 0, sizeof (jr_ESIS_KeywordType));

	keyword_info->name_str			= jr_strdup (name_str);
	keyword_info->keyword_number	= keyword_number;
}

void jr_ESIS_KeywordUndo (
	jr_ESIS_KeywordType *		keyword_info)
{
	jr_free (keyword_info->name_str);
}

jr_ESIS_KeywordType *jr_ESIS_KeywordCreate (
	const char *				name_str,
	jr_int						keyword_number)
{
	jr_ESIS_KeywordType *		keyword_info;

	keyword_info		= (jr_ESIS_KeywordType *) jr_malloc (sizeof (jr_ESIS_KeywordType));

	jr_ESIS_KeywordInit (keyword_info, name_str, keyword_number);

	return keyword_info;
}

void jr_ESIS_KeywordDestroy (
	jr_ESIS_KeywordType *		keyword_info)
{
	jr_ESIS_KeywordUndo (keyword_info);
	jr_free (keyword_info);
}

jr_int jr_ESIS_KeywordHash (const void *	void_arg_1)
{
	const jr_ESIS_KeywordType *	keyword_info		= (jr_ESIS_KeywordType *) void_arg_1;
	jr_int						hash_value			= 0;

	hash_value += jr_strhash_pjw (keyword_info->name_str);

	return hash_value;
}


jr_int jr_ESIS_KeywordCmp (const void * void_arg_1, const void *void_arg_2)
{
	const jr_ESIS_KeywordType *	keyword_info_1	= (jr_ESIS_KeywordType *) void_arg_1;
	const jr_ESIS_KeywordType *	keyword_info_2	= (jr_ESIS_KeywordType *) void_arg_2;
	jr_int						diff;

	diff = strcmp (keyword_info_1->name_str, keyword_info_2->name_str);

	if (diff != 0) {
		return diff;
	}

	return 0;
}
