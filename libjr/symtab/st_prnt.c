#include "project.h"

jr_int jr_ST_PrintDeclaration (declarator_type, opt_declarator_name, prefix, wfp)
	jr_ST_TypeStruct *		declarator_type;
	char *					opt_declarator_name;
	char *					prefix;
	FILE *					wfp;
{
	char					dest_buf [1024];
	jr_int					status;


	status	= jr_ST_DeclarationToString (
				declarator_type,
				0,	/* opt_base_type_sym */
				opt_declarator_name,
				0,	/* nesting_level */
				dest_buf, sizeof (dest_buf)
			);

	if (prefix == 0) {
		prefix = "";
	}
	if (status == 0) {
		fprintf (wfp, "%s%s", prefix, dest_buf);
	}
	else {
		fprintf (wfp, "%s%s...(decl. too long)", prefix, dest_buf);
	}

	return (status);
}


jr_int jr_ST_DeclarationToString (
							declarator_type, opt_base_type_sym, opt_declarator_name,
							nesting_level, dest_buf, buf_length
						)
	jr_ST_TypeStruct *	declarator_type;
	jr_ST_Symbol *		opt_base_type_sym;
	char *				opt_declarator_name;
	jr_int				nesting_level;
	char *				dest_buf;
	jr_int				buf_length;
{
	char *				end_ptr;
	jr_ST_Symbol *		curr_base_type_sym;
	jr_ST_Symbol *		base_type_sym;
	jr_int				i;
	jr_int *			ip;
	jr_AList			stars_list[1];
	jr_int				status;

	end_ptr		= dest_buf + buf_length - 1;
	*end_ptr	= 0;
	/*
	 * null out the string so that after an array bounds error
	 * they still get a valid string
	 */


	jr_AListInit (stars_list, sizeof (jr_int));

	/*
	 * for each type in the list of abstract declarators
	 * record the number of stars
	 */

	jr_AListNativeSetNewTail (stars_list, jr_ST_TypeNumStars (declarator_type), jr_int);

	/*
	 * count the number of stars per nesting level in declaration
	 */
	curr_base_type_sym = jr_ST_TypeBaseSymbol (declarator_type);

	while (	jr_ST_SymbolIsVariable (curr_base_type_sym)
		||	jr_ST_SymbolIsFunction (curr_base_type_sym)) {
		/*
		 * bottom out when we hit the first typedef, builtin, struct
		 */

		jr_AListNativeSetNewTail (
			stars_list, jr_ST_TypeNumStars (jr_ST_SymbolType (curr_base_type_sym)), int
		);

		curr_base_type_sym = jr_ST_TypeBaseSymbol (jr_ST_SymbolType (curr_base_type_sym));
	}

	/*
	 * print the passed in base type for this symbol, not the actual base type
	 * (to allow a caller to change the base type)
	 */
	if (opt_base_type_sym) {
		base_type_sym = opt_base_type_sym;
	}
	else {
		base_type_sym = jr_ST_FindBaseTypeSymbol (declarator_type);
	}

	buf_length	= end_ptr - dest_buf;

	status		= jr_ST_TypeSymbolToString (
					base_type_sym, nesting_level + 1, dest_buf, buf_length + 1
				);

	if (status != 0) goto return_bad;
	dest_buf	= strchr (dest_buf, 0);


	*dest_buf	= ' ';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;

	
	/*
	 * print the stars in reverse order, with open paren around each
	 * new level, except for the first
	 */
	jr_AListForEachElementPtrRev (stars_list, ip) {
		for (i=0; i < *ip; i++) {
			/*
			 * add a star to the buffer
			 */
			*dest_buf = '*';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
		}

		if (ip != jr_AListHeadPtr (stars_list)) {
			*dest_buf	= '(';  dest_buf ++;  if (dest_buf == end_ptr) goto return_bad;
		}
	}

	/* 
	 * put the name of the variable
	 */
	if (opt_declarator_name) {
		jr_int				str_length			= (jr_int) strlen (opt_declarator_name);

		buf_length	= end_ptr - dest_buf;

		if (str_length > buf_length) goto return_bad;

		strcpy (dest_buf, opt_declarator_name);
		dest_buf += str_length;
	}
	
	/*
	 * put the array or parameter parts of the declaration(s)
	 * followed by the close parens
	 */
	buf_length	= end_ptr - dest_buf;
	status		= jr_ST_TypeListToString (declarator_type, dest_buf, buf_length + 1);

	if (status != 0) goto return_bad;
	dest_buf	= strchr (dest_buf, 0);

	/*
	 * loop through each nesting level to print the parameter list or
	 * array dimensions (using TypeListToString).
	 * Also print close paren.
	 */
	curr_base_type_sym = jr_ST_TypeBaseSymbol (declarator_type);
	while (	jr_ST_SymbolIsVariable (curr_base_type_sym)
		||	jr_ST_SymbolIsFunction (curr_base_type_sym)) {
		/*
		 * bottom out when we hit the first typedef, builtin, struct
		 */

		*dest_buf	= ')'; dest_buf ++; if (dest_buf == end_ptr) goto return_bad;

		buf_length	= end_ptr - dest_buf;
		status	= jr_ST_TypeListToString (
					jr_ST_SymbolType (curr_base_type_sym), dest_buf, buf_length + 1
				);

		if (status != 0) goto return_bad;
		dest_buf	= strchr (dest_buf, 0);
		
		curr_base_type_sym = jr_ST_TypeBaseSymbol (jr_ST_SymbolType (curr_base_type_sym));
	}


	if (1) {
		status = 0;
	}
	else {
		return_bad :
		status = -1;
	}

	jr_AListUndo (stars_list);

	return (status);
}


jr_int jr_ST_TypeSymbolToString (base_type_sym, nesting_level, dest_buf, buf_length)
	jr_ST_Symbol *		base_type_sym;
	jr_int				nesting_level;
	char *				dest_buf;
	jr_int				buf_length;
{
	/*
	 * this function can only be called on typedefs, builtins
	 * structs or unions
	 */
	char *			end_ptr;
	jr_int			status, i;


	end_ptr		= dest_buf + buf_length - 1;
	*end_ptr	= 0;
	/*
	 * null out the string so that after an array bounds error
	 * they still get a valid string
	 */

	switch (jr_ST_SymbolKind (base_type_sym)) {
			/*
			 * a string will act like a built in
			 */
		case jr_ST_STRING :
		case jr_ST_BUILTIN :
		case jr_ST_TYPEDEF : {
			char *string;
			jr_int str_length;

			string		= jr_ST_SymbolStr (base_type_sym);
			str_length	= (jr_int) strlen (string);

			buf_length	= end_ptr - dest_buf;

			if (str_length > buf_length) goto return_bad;

			strcpy (dest_buf, string);
			dest_buf += str_length;
			break;
		}

		case jr_ST_STRUCT :
		case jr_ST_UNION :
		case jr_ST_ENUM :		{
			jr_ST_FieldList *		field_list;
			jr_ST_FieldType *			field;
			char *				string;

			if (jr_ST_SymbolStr (base_type_sym)) {
				string		= jr_ST_SymbolStr (base_type_sym);
			}
			else if (jr_ST_SymbolKind(base_type_sym) == jr_ST_STRUCT) {
				string = "struct {\n";
			}
			else if (jr_ST_SymbolKind(base_type_sym) == jr_ST_UNION) {
				string = "union {\n";
			}
			else {
				string = "enum {\n";
			}

			{
				jr_int	str_length = (jr_int) strlen (string);

				buf_length	= end_ptr - dest_buf;

				if (str_length > buf_length) goto return_bad;

				strcpy (dest_buf, string);
				dest_buf += str_length;
			}
			
			if (jr_ST_SymbolStr (base_type_sym)) {
				break;
			}

			if (jr_ST_SymbolKind (base_type_sym) == jr_ST_ENUM) {
				break;
			}

			/*
			 * its an un-named struct, print the fields in the declaration
			 */
			field_list = jr_ST_TypeFieldList (jr_ST_SymbolType (base_type_sym));


			jr_ST_FieldListForEachElementPtr (field_list, field) {
				for (i=0; i < nesting_level; i++) {
					/*
					 * add a tab to the buffer
					 */
					*dest_buf = '\t';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
				}

				buf_length	= end_ptr - dest_buf;
				status	= jr_ST_DeclarationToString (
							jr_ST_FieldTypePtr (field),
							jr_ST_FindBaseTypeSymbol (jr_ST_FieldTypePtr (field)),
							jr_ST_FieldStr (field),
							nesting_level,
							dest_buf, buf_length + 1
						);

				if (status != 0) goto return_bad;
				dest_buf = strchr (dest_buf, 0);

				*dest_buf = ';';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
				*dest_buf = '\n';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
			}
			for (i=0; i < nesting_level-1; i++) {
				*dest_buf = '\t';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
			}
			*dest_buf = '}';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
			*dest_buf = ' ';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;

			*dest_buf = 0;
			break;
		}
	}

	if (1) {
		status = 0;
	}
	else {
		return_bad: status = -1;
	}

	return (status);
}

jr_int jr_ST_TypeListToString (type, dest_buf, buf_length)
	jr_ST_TypeStruct *	type;
	char *				dest_buf;
	jr_int				buf_length;
{
	/*
	 * only should be called for variables or functions
	 */
	char *				end_ptr;
	jr_int				status			= 0;


	end_ptr		= dest_buf + buf_length - 1;
	*end_ptr	= 0;

	switch (jr_ST_TypeKind (type)) {
		case jr_ST_VARIABLE : {
			jr_ST_ArrayList *	list = jr_ST_TypeArrayList (type);
			jr_int				i, str_length;
			char *				string;
			char				digit_buf [64];

			jr_ST_ArrayForEachDimension (list, i) {
				*dest_buf = '[';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;

				/*
				 * first use the length number, then the length str,
				 * then the length expr
				 */
				if (	jr_ST_ArrayDimensionLengthNum (list, i) > 0
					||	jr_ST_ArrayDimensionLengthStr (list, i)) {

					if (jr_ST_ArrayDimensionLengthNum (list, i) > 0) {
						sprintf (digit_buf, "%d", jr_ST_ArrayDimensionLengthNum (list, i));
						string = digit_buf;
					}
					else {
						string = jr_ST_ArrayDimensionLengthStr (list, i);
					}

					str_length	= (jr_int) strlen (string);
					buf_length	= end_ptr - dest_buf;

					if (str_length > buf_length) goto return_bad;

					strcpy (dest_buf, string);
					dest_buf += str_length;
				}
				else if (jr_ST_ArrayDimensionLengthExpr (list, i)) {
					char error_buf [512];

					buf_length	= end_ptr - dest_buf;
					status = jr_C_ExprNodeToString (
						dest_buf, buf_length + 1, jr_ST_ArrayDimensionLengthExpr (list, i), error_buf
					);
					if (status != 0) goto return_bad;
					dest_buf = strchr (dest_buf, 0);
				}

				*dest_buf = ']';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
			}
			*dest_buf = 0;
			break;
		}
		case jr_ST_FUNCTION : {
			jr_ST_ParamList *list = jr_ST_TypeParamList (type);
			jr_ST_ParamType *param;
			jr_int first_time = 1;

			*dest_buf = ' ';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
			*dest_buf = '(';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;

			jr_ST_ParamListForEachElementPtr (list, param) {
				if (!first_time) {
					*dest_buf = ',';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
					*dest_buf = ' ';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
				}
				buf_length	= end_ptr - dest_buf;
				status	= jr_ST_DeclarationToString (
							jr_ST_ParamTypePtr (param),
							jr_ST_FindBaseTypeSymbol (jr_ST_ParamTypePtr (param)),
							jr_ST_ParamStr (param),
							0,	/* nesting_level */
							dest_buf, buf_length + 1
						);
				if (status != 0) goto return_bad;
				dest_buf = strchr (dest_buf, 0);

				first_time = 0;
			}
			*dest_buf = ')';  dest_buf++;  if (dest_buf == end_ptr) goto return_bad;
			*dest_buf = 0;
		}
	}

	if (1) {
		status = 0;
	}
	else {
		return_bad : status = -1;
	}

	return (status);
}

