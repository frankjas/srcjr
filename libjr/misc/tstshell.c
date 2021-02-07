#include "ezport.h"

#include <errno.h>
#include <stdlib.h>

#include "jr/test/shell.h"

#include "jr/malloc.h"
#include "jr/error.h"
#include "jr/misc.h"
#include "jr/llist.h"
#include "jr/string.h"

#include "jr/long.h"

#include "g2c/parser.h"
#include "s2c/scanner.h"

void jr_TestShellInit (shell_info, parse_fn, scan_fn, var_undo_fn, var_print_fn, prompt_str)
	jr_TestShellType *		shell_info;
	jr_int					(*parse_fn)		();
	jr_int					(*scan_fn)		();
	void					(*var_undo_fn)	();
	void					(*var_print_fn) ();
	const char *			prompt_str;
{
	memset (shell_info, 0, sizeof (jr_TestShellType));

	shell_info->parse_fn				= parse_fn;
	shell_info->scan_fn					= scan_fn;
	shell_info->var_undo_fn				= var_undo_fn;
	shell_info->var_print_fn			= var_print_fn;

	shell_info->prompt_str				= jr_strdup (prompt_str);

	shell_info->verbose					= 0;
	shell_info->echo					= 1;
	shell_info->save_command			= 1;
	shell_info->save_file_commands		= 1;
	shell_info->do_history				= 1;
	shell_info->break_on_bad_status		= 0;

	jr_AListInit (shell_info->command_list, sizeof (jr_TestShellLineStruct));
	jr_IB_InputBufferInit (shell_info->input_buffer);

	jr_LListInit (shell_info->help_info_list, sizeof (jr_TestShellHelpType));

	jr_HTableInit (
		shell_info->variable_table, sizeof (jr_TestShellVarInfoType),
		jr_TestShellVarInfoHash, jr_TestShellVarInfoCmp
	);

	jr_AListInit (shell_info->include_dir_list, sizeof (char *));
}


void jr_TestShellUndo (shell_info)
	jr_TestShellType *	shell_info;
{
	const char *		include_dir;
	jr_int				z;

	jr_free (shell_info->prompt_str);

	if (shell_info->script_file) {
		jr_free (shell_info->script_file);
	}

	jr_AListForEachElementIndex( shell_info->include_dir_list, z) {
		include_dir	= jr_AListNativeElement( shell_info->include_dir_list, z, char *);
		jr_free( include_dir);
	}
	jr_AListUndo (shell_info->include_dir_list);

	jr_TestShellEmptyHistory (shell_info);
	jr_AListUndo (shell_info->command_list);

	{
		jr_TestShellHelpType *			help_info;

		jr_LListForEachElementPtr (shell_info->help_info_list, help_info) {
			jr_TestShellHelpUndo (help_info);
		}
		jr_LListUndo (shell_info->help_info_list);
	}
	{
		jr_TestShellVarInfoType *		var_info;

		jr_HTableForEachElementPtr (shell_info->variable_table, var_info) {
			jr_TestShellVarInfoUndo (shell_info, var_info);
		}

		jr_HTableUndo (shell_info->variable_table);
	}

	jr_IB_InputBufferUndo (shell_info->input_buffer);
	/*
	** 9/23/05: do this last so that line numbers will still work
	** (for use in the debugger)
	*/

}


void jr_TestShellSetFilePtr (shell_info, script_rfp, script_file)
	jr_TestShellType *		shell_info;
	FILE *					script_rfp;
	const char *			script_file;
{
	if (script_file) {
		shell_info->script_file			= jr_strdup (script_file);
	}
	shell_info->script_rfp				= script_rfp;
	shell_info->verbose					= script_rfp == stdin;
}


void jr_TestShellAddIncludeDir (shell_info, include_dir)
	jr_TestShellType *		shell_info;
	const char *			include_dir;
{
	jr_AListNativeSetNewTail( shell_info->include_dir_list, jr_strdup( include_dir), char *);
}

jr_int jr_TestShellReadFromFileName (shell_info, file_name, error_buf)
	jr_TestShellType *		shell_info;
	const char *			file_name;
	char *					error_buf;
{
	FILE *					rfp ;
	jr_int					status;

	if (strcmp (file_name, "-") == 0) {
		status = jr_TestShellReadFromFilePtr (shell_info, stdin, "<stdin>", error_buf) ;
	}
	else {
		rfp = fopen (file_name, "r");

		if (rfp == NULL) {
			jr_esprintf (error_buf, "couldn't open for reading: %s", strerror (errno));
			return -1;
		}

		status = jr_TestShellReadFromFilePtr (shell_info, rfp, file_name, error_buf);

		fclose (rfp);
	}

	if (status != 0) {
		return -1;
	}
	return 0;
}

jr_int jr_TestShellReadFromFilePtr (shell_info, rfp, file_name, error_buf)
	jr_TestShellType *		shell_info;
	FILE *					rfp;
	const char *			file_name;
	char *					error_buf;
{
	jr_int					status;


	jr_TestShellSetFilePtr (shell_info, rfp, file_name);

	if (rfp == stdin) {
		shell_info->echo					= 1;
		shell_info->verbose					= 1;
		jr_TestShellSetBreakOnBadStatus (shell_info, 0);
	}
	else {
		shell_info->echo					= 0;
		shell_info->verbose					= 0;
		jr_TestShellSetBreakOnBadStatus (shell_info, 1);
	}

	status = jr_TestShellRead (shell_info, error_buf);

	return status;
}

jr_int jr_TestShellRead (shell_info, error_buf)
	jr_TestShellType *		shell_info;
	char *					error_buf;
{
	g2c_ParserType			parser[1];
	s2c_ScannerType			scanner [1];
	jr_int					line_buffer_size;
	jr_int					line_length;
	char *					line_buffer;
	char *					tmp_ptr;

	jr_int					assert_this_is_bad;


	g2c_ParserInit (parser);
	s2c_ScannerInit (scanner);
	s2c_ScannerSetParser (scanner, parser);

	g2c_ParserInitErrorRecovery (parser, 0, stderr, 0, 0);
	g2c_ParserSetNoParseOnRepair (parser);

	line_buffer_size			= 1024;
	line_buffer					= jr_malloc (line_buffer_size);

	if (!shell_info->recursive_call) {
		/*
		 * use SwitchContext so that the context must be closed
		 * explicitly, this prevents the 0th context
		 * from closing during recursive calls
		 */
		jr_IB_SwitchContextToFile (shell_info->input_buffer,
			shell_info->script_file, jr_IB_DONT_SAVE_NAME, 
			shell_info->script_rfp, jr_IB_DONT_FCLOSE_RFP
		);
	}


	while (1) {
		if (jr_TestShellIsInteractive (shell_info)) {
			if (shell_info->recursive_call) {
				fprintf (stderr, "\n    %d: loop> ", (jr_int) jr_AListSize (shell_info->command_list));
			}
			else {
				fprintf (stderr, "\n%d: %s> ",
					(jr_int) jr_AListSize (shell_info->command_list),
					shell_info->prompt_str
				);
			}
		}

		shell_info->status			= 0;
		shell_info->save_command	= 1;

		if (jr_IB_gets (shell_info->input_buffer, line_buffer, line_buffer_size) == NULL) {
			if (jr_IB_IsNestedContext (shell_info->input_buffer)) {
				jr_IB_CloseCurrentContext (shell_info->input_buffer);
				continue;
			}
			else {
				break;
			}
		}

		/*
		 * Check for a partially read line, either no newline, or a backslash before
		 */
		tmp_ptr	= strchr (line_buffer, '\n');

		while (tmp_ptr == 0  ||  tmp_ptr  &&  tmp_ptr[-1] == '\\') {
			if (tmp_ptr) {
				tmp_ptr[-1] = ' ';
				tmp_ptr[0]	= 0;
			}
			line_buffer_size	+= 1024;
			line_buffer			= jr_realloc (line_buffer, line_buffer_size);

			line_length			= (jr_int) strlen (line_buffer);

			if (	jr_IB_gets (
						shell_info->input_buffer,
						line_buffer + line_length,
						line_buffer_size - line_length
					) == NULL
				) {
				break;
			}
			tmp_ptr	= strchr (line_buffer, '\n');
		}

		s2c_ScannerOpenStringContext (
			scanner,
			jr_IB_CurrentContextName (shell_info->input_buffer), s2c_DONT_SAVE_NAME,
			line_buffer, s2c_DONT_FREE_STR
		);

		s2c_ScannerCurrentContextSetLine (
			scanner, jr_IB_CurrentContextLine (shell_info->input_buffer) - 1
		);

		if (	shell_info->echo
			&&  jr_IB_CurrentContextRfp (shell_info->input_buffer) != stdin) {

			fprintf (stderr, "\n%s %d: %s> %s",
				jr_IB_CurrentContextName (shell_info->input_buffer),
				jr_IB_CurrentContextLine (shell_info->input_buffer) - 1,
				shell_info->prompt_str,
				line_buffer
			);
		}

		assert_this_is_bad					= shell_info->assert_next_is_bad;
		shell_info->assert_next_is_bad		= 0;

		jr_TestShellBreakPoint (shell_info, 0);		/* to provide a place to set breakpoints */

		(*shell_info->parse_fn) (parser, scanner, shell_info->scan_fn, shell_info);

		if (jr_TestShellCheckMallocBounds (shell_info)) {

			if (jr_malloc_check_bounds (error_buf)  !=  NULL) {

				jr_esprintf (error_buf, "%.64s:%d: malloc bounds check failed: %s",
					jr_IB_CurrentContextName (shell_info->input_buffer),
					jr_IB_CurrentContextLine (shell_info->input_buffer) - 1,
					error_buf
				);
				fprintf (stderr, "\t%s: %s", error_buf, line_buffer);

				shell_info->status = -1;
				break;
			}
		}

		if (g2c_ParserStatus (parser) == g2c_ACCEPT_STATUS) {
			if (assert_this_is_bad) {
				if (shell_info->status == 0) {
					if (jr_TestShellIsInteractive (shell_info)) {
						jr_esprintf (error_buf, "assert bad failed: good command status"); 
						fprintf (stderr, "\tAssert bad failed: good command status: %s\n",
							line_buffer
						);
					}
					else {
						jr_esprintf (error_buf, "%.64s:%d: assert bad failed: good command status",
							jr_IB_CurrentContextName (shell_info->input_buffer),
							jr_IB_CurrentContextLine (shell_info->input_buffer) - 1
						);
						fprintf (stderr, "\t%s: %s", error_buf, line_buffer);
					}
					if (shell_info->break_on_bad_status) {
						shell_info->status = -1;
						break;
					}
				}
			}
			else if (shell_info->status != 0) {
				if (jr_TestShellIsInteractive (shell_info)) {
					jr_esprintf (error_buf, "bad command status"); 
					fprintf (stderr, "\tBad command status: %s\n", line_buffer);
				}
				else {
					jr_esprintf (error_buf, "%.64s:%d: bad command status",
						jr_IB_CurrentContextName (shell_info->input_buffer),
						jr_IB_CurrentContextLine (shell_info->input_buffer) - 1
					);
					fprintf (stderr, "\t%s: %s", error_buf, line_buffer);
				}
				if (shell_info->break_on_bad_status) break;
			}

			if (	shell_info->do_history
				&&	(jr_TestShellIsInteractive (shell_info) ||  shell_info->save_file_commands)) {

				jr_TestShellLineStruct *	command_line;

				command_line				= jr_AListNewTailPtr (shell_info->command_list);
				command_line->line_str		= jr_strdup (line_buffer);
				command_line->save_command	= shell_info->save_command;
			}

			if (shell_info->quit) break;
		}
		else {
			if (jr_TestShellIsInteractive (shell_info)) {
				jr_esprintf (error_buf, "syntax error");
			}
			else {
				jr_esprintf (error_buf, "%.64s:%d: syntax error",
					jr_IB_CurrentContextName (shell_info->input_buffer),
					jr_IB_CurrentContextLine (shell_info->input_buffer) - 1
				);
			}
			fprintf (stderr, "\tCommand not executed: %s\n", error_buf);

			g2c_ParserReset (parser);

			/*
			 * either close the current file, or terminate the line
			 * we're reading from
			 */
			if (s2c_ScannerContextDepth (scanner) > 0) {
				s2c_ScannerCloseCurrentContext (scanner);
			}
			shell_info->status = -1;
			if (shell_info->break_on_bad_status) break;
		}
	}

	if (shell_info->recursive_call) {
		if (jr_TestShellIsInteractive (shell_info)) {
			clearerr (stdin);
			fprintf (stderr, "Returning from loop\n");
		}
	}

	s2c_ScannerUndo (scanner);
	g2c_ParserUndo (parser);

	jr_free (line_buffer);

	return (shell_info->status);
}


jr_int jr_TestShellReadString (shell_info, input_string, error_buf)
	jr_TestShellType *		shell_info;
	char *					input_string;
	char *					error_buf;
{
	g2c_ParserType			parser[1];
	s2c_ScannerType			scanner [1];

	jr_int					assert_this_is_bad;


	g2c_ParserInit (parser);
	s2c_ScannerInit (scanner);
	s2c_ScannerSetParser (scanner, parser);

	g2c_ParserInitErrorRecovery (parser, 0, stderr, error_buf, jr_ERROR_LEN);
	g2c_ParserSetNoParseOnRepair (parser);


	shell_info->status			= 0;
	shell_info->save_command	= 1;


	s2c_ScannerOpenStringContext (
		scanner, "string", s2c_DONT_SAVE_NAME,
		input_string, s2c_DONT_FREE_STR
	);

	assert_this_is_bad					= shell_info->assert_next_is_bad;
	shell_info->assert_next_is_bad		= 0;

	(*shell_info->parse_fn) (parser, scanner, shell_info->scan_fn, shell_info);

	if (g2c_ParserStatus (parser) == g2c_ACCEPT_STATUS) {
		if (assert_this_is_bad) {
			if (shell_info->status == 0) {
				jr_esprintf (error_buf, "assert bad failed: good command status"); 
				shell_info->status = -1;
			}
		}
		else if (shell_info->status != 0) {
			jr_esprintf (error_buf, "bad command status"); 
		}
	}
	else {
		/*
		 * error_buf is filled up automatically
		 */
		shell_info->status = -1;
	}

	s2c_ScannerUndo (scanner);
	g2c_ParserUndo (parser);

	return (shell_info->status);
}


void jr_TestShellEmptyHistory (shell_info)
	jr_TestShellType *	shell_info;
{
	jr_TestShellLineStruct *command_line;

	jr_AListForEachElementPtr (shell_info->command_list, command_line) {
		jr_free (command_line->line_str);
	}
	jr_AListEmpty (shell_info->command_list);
}


jr_int jr_TestShellPushNewInputFile(
	jr_TestShellType *				shell_info,
	const char *					file_name,
	char *							error_buf)
{
	FILE *							rfp;
	const char *					include_dir;
	char *							tmp_path			= 0;
	jr_int							z;
	jr_int							status;


	rfp = fopen( file_name, "r");

	if (rfp == 0) {
		/*
		** 3-21-2013: use the first error if it's not in the path.
		*/
		jr_esprintf( error_buf, "%s", strerror( errno));

		jr_AListForEachElementIndex( shell_info->include_dir_list, z) {
			include_dir	= jr_AListNativeElement( shell_info->include_dir_list, z, char *);

			tmp_path		= jr_malloc( strlen( include_dir) + 1 + strlen( file_name) + 1);

			sprintf( tmp_path, "%s/%s", include_dir, file_name);

			rfp = fopen( tmp_path, "r");

			if (rfp != 0) {
				break;
			}
		}
		if (rfp == 0) {
			status = jr_NOT_FOUND_ERROR;
			goto return_status;
		}
	}

	jr_IB_SwitchContextToFile (
		shell_info->input_buffer, file_name, jr_IB_SAVE_NAME, rfp, jr_IB_FCLOSE_RFP
	);
	rfp = 0;


	status = 0;

	return_status : {
		if (rfp) {
			fclose( rfp);
		}
		if (tmp_path) {
			jr_free( tmp_path);
		}
	}

	return status;
}

void jr_TestShellSave (shell_info, wfp)
	jr_TestShellType *				shell_info;
	FILE *							wfp;
{
	jr_TestShellLineStruct *		command_line;

	jr_AListForEachElementPtr (shell_info->command_list, command_line) {
		if (command_line->save_command) {
			fprintf (wfp, "%s", command_line->line_str);
		}
	}
}

void jr_TestShellAddHelpMessage (shell_info, command_name, help_message)
	jr_TestShellType *		shell_info;
	char *					command_name;
	char *					help_message;
{
	jr_TestShellHelpType *	help_info;


	help_info				= jr_LListNewTailPtr (shell_info->help_info_list);

	jr_TestShellHelpInit (help_info, command_name, help_message);
}

void jr_TestShellPrintHelpMessage (shell_info, command_name, wfp)
	jr_TestShellType *		shell_info;
	char *					command_name;
	FILE *					wfp;
{
	jr_TestShellHelpType *	help_info;
	jr_int					found_help			= 0;


	if (command_name) {
		jr_LListForEachElementPtr (shell_info->help_info_list, help_info) {
			if (strcmp (jr_TestShellHelpCommandName (help_info), command_name)  ==  0) {
				fprintf (wfp, "\t%s\n", jr_TestShellHelpMessage (help_info));
				found_help		= 1;
			}
		}
	}
	else {
		found_help		= 1;
		jr_LListForEachElementPtr (shell_info->help_info_list, help_info) {
			fprintf (wfp, "\t%s\n", jr_TestShellHelpMessage (help_info));
		}
	}

	if (! found_help) {
		fprintf (wfp, "\tNo help for: '%s', try:\n", command_name);
		jr_LListForEachElementPtr (shell_info->help_info_list, help_info) {
			if (strstr (jr_TestShellHelpCommandName (help_info), command_name)) {
				fprintf (wfp, "\t%s\n", jr_TestShellHelpMessage (help_info));
			}
			if (strstr (jr_TestShellHelpMessage (help_info), command_name)) {
				fprintf (wfp, "\t%s\n", jr_TestShellHelpMessage (help_info));
			}
		}
	}
}


void jr_TestShellAddStandardHelpMessages (shell_info)
	jr_TestShellType *			shell_info;
{
	jr_TestShellAddHelpMessage (shell_info, "exit",
		"exit                                  : exit the program");
	jr_TestShellAddHelpMessage (shell_info, "read",
		"read    commands   from \"file\"        : read commands from file_name");
	jr_TestShellAddHelpMessage (shell_info, "history",
		"write   history  to   \"file\"          : write 'saved' commands to file_name");
	jr_TestShellAddHelpMessage (shell_info, "history",
		"append  history  to   \"file\"          : append commands to file_name");
	jr_TestShellAddHelpMessage (shell_info, "history",
		"history [on off]                      : add (or not) commands to history");
	jr_TestShellAddHelpMessage (shell_info, "echo",
		"echo    [on off]                      : ");
	jr_TestShellAddHelpMessage (shell_info, "echo",
		"echo    [<text>] [date]               : echo <text>, followed by the current date");
	jr_TestShellAddHelpMessage (shell_info, "eof",
		"eof                                   : simulate an end-of-file");
	jr_TestShellAddHelpMessage (shell_info, "verbose",
		"verbose [on off]                      : ");
	jr_TestShellAddHelpMessage (shell_info, "save",
		"save    file commands                 : commands read from files added to history");
	jr_TestShellAddHelpMessage (shell_info, "discard",
		"discard file commands                 : opposite of above & add 'read' command to history");
	jr_TestShellAddHelpMessage (shell_info, "empty",
		"empty   history                       : empty the current history");
	jr_TestShellAddHelpMessage (shell_info, "print",
		"print   history                       : print the current history");
	jr_TestShellAddHelpMessage (shell_info, "print",
		"print   variables [\"file\"]            : print the all variables to the file");
	jr_TestShellAddHelpMessage (shell_info, "print",
		"print   <V> [<V> ...] [diagnostic]    : print the variables listed");
	jr_TestShellAddHelpMessage (shell_info, "assert",
		"assert  next command is bad           : expects following command to fail");

	jr_TestShellAddHelpMessage (shell_info, "", "");

}


void jr_TestShellSetVarInfo (shell_info, var_name, token_number, data_ptr)
	jr_TestShellType *			shell_info;
	const char *				var_name;
	jr_int						token_number;
	const void *				data_ptr;
{
	jr_TestShellVarInfoType		var_info[1];
	jr_TestShellVarInfoType *	found_var_info;


	jr_TestShellVarInfoInit (var_info, var_name, token_number, data_ptr);

	found_var_info = jr_HTableDeleteElement (shell_info->variable_table, var_info);

	if (found_var_info) {
		jr_TestShellVarInfoUndo (shell_info, found_var_info);
	}

	jr_HTableSetNewElementIndex (shell_info->variable_table, var_info);
}


jr_TestShellVarInfoType *jr_TestShellGetVarInfo (shell_info, var_name)
	jr_TestShellType *			shell_info;
	const char *				var_name;
{
	jr_TestShellVarInfoType		var_info [1];
	jr_TestShellVarInfoType *	found_var_info;


	jr_TestShellVarInfoInit (var_info, var_name, 0, 0);

	found_var_info = jr_HTableFindElementPtr (shell_info->variable_table, var_info);

	jr_TestShellVarInfoUndo (shell_info, var_info);

	return found_var_info;
}


jr_int jr_TestShellGetIntValue (shell_info, var_name, integer_tkn, ulong_tkn)
	jr_TestShellType *			shell_info;
	char *						var_name;
	jr_int						integer_tkn;
	jr_int						ulong_tkn;
{
	jr_TestShellVarInfoType *	var_info;

	var_info		= jr_TestShellGetVarInfo (shell_info, var_name);

	if (var_info == 0) {
		fprintf (stderr, "\tVariable '%s' does not exist\n", var_name);
		jr_TestShellSetBadStatus (shell_info);

		return 0;
	}

	if (jr_TestShellVarInfoTokenNumber (var_info) == ulong_tkn) {
		jr_ULong *				ulong_ptr		= jr_TestShellVarInfoDataPtr (var_info);

		if (! jr_ULongIsUInt (*ulong_ptr)) {
			fprintf (stderr, "\tULong variable '%s' is not an integer, has value %u:%u\n",
				var_name, jr_ULongMSW (*ulong_ptr), jr_ULongLSW (*ulong_ptr)
			);
			jr_TestShellSetBadStatus (shell_info);

			return 0;
		}
		return jr_ULongLSW (*ulong_ptr);
	}
	if (jr_TestShellVarInfoTokenNumber (var_info) == integer_tkn) {
		jr_int *				int_ptr		= jr_TestShellVarInfoDataPtr (var_info);

		return *int_ptr;
	}

	fprintf (stderr, "\tVariable '%s' is not a number\n", var_name);
	jr_TestShellSetBadStatus (shell_info);

	return 0;
}


void jr_TestShellSetIntValue (shell_info, var_name, integer_tkn, value)
	jr_TestShellType *			shell_info;
	char *						var_name;
	jr_int						integer_tkn;
	jr_int						value;
{
	jr_TestShellSetVarInfo (
		shell_info, var_name, integer_tkn,
		jr_memdup (&value, sizeof (jr_int))
	);
}


void *jr_TestShellGetDataPtr (shell_info, var_name)
	jr_TestShellType *			shell_info;
	const char *				var_name;
{
	jr_TestShellVarInfoType *	var_info;
	
	var_info = jr_TestShellGetVarInfo (shell_info, var_name);

	if (var_info == 0) {
		return 0;
	}

	return (void *) jr_TestShellVarInfoDataPtr (var_info);
}

jr_int jr_TestShellDeleteVariable (shell_info, var_name)
	jr_TestShellType *			shell_info;
	const char *				var_name;
{
	jr_TestShellVarInfoType	var_info [1];
	jr_TestShellVarInfoType *	deleted_var_info;


	jr_TestShellVarInfoInit (var_info, var_name, 0, 0);

	deleted_var_info = jr_HTableDeleteElement (shell_info->variable_table,  var_info);

	jr_TestShellVarInfoUndo (shell_info, var_info);
	
	if (deleted_var_info == 0) {
		return -1;
	}
	jr_TestShellVarInfoUndo (shell_info, deleted_var_info);

	return 0;
}

void * jr_TestShellExtractVariable (shell_info, var_name)
	jr_TestShellType *			shell_info;
	const char *				var_name;
{
	jr_TestShellVarInfoType		var_info [1];
	jr_TestShellVarInfoType *	deleted_var_info;
	const void *				data_ptr;


	jr_TestShellVarInfoInit (var_info, var_name, 0, 0);

	deleted_var_info = jr_HTableDeleteElement (shell_info->variable_table,  var_info);

	jr_TestShellVarInfoUndo (shell_info, var_info);
	
	if (deleted_var_info == 0) {
		return 0;
	}

	data_ptr = jr_TestShellVarInfoDataPtr (deleted_var_info);
	jr_TestShellVarInfoSetDataPtr (deleted_var_info, 0);

	jr_TestShellVarInfoUndo (shell_info, deleted_var_info);

	return (void *) data_ptr;
}

void jr_TestShellPrintVariables (shell_info, is_diagnostic, wfp)
	jr_TestShellType *			shell_info;
	jr_int						is_diagnostic;
	FILE *						wfp;
{
	jr_TestShellVarInfoType *	var_info;
	jr_AList					var_list [1];
	jr_int						index;


	jr_AListInit (var_list, sizeof (jr_TestShellVarInfoType *));

	jr_TestShellForEachVarInfoPtr (shell_info, var_info) {
		jr_AListNativeSetNewTail (var_list, var_info, jr_TestShellVarInfoType *);
	}

	jr_AListQSort (var_list, jr_TestShellVarInfoPtrCmp);

	jr_AListForEachElementIndex (var_list, index) {
		var_info = jr_AListNativeElement (var_list, index, jr_TestShellVarInfoType *);

		jr_TestShellPrintVariable (shell_info, var_info, is_diagnostic, wfp);
	}

	jr_AListUndo (var_list);
}

const char *jr_TestShellVarDataPtrName (shell_info, data_ptr)
	jr_TestShellType *		shell_info;
	void *					data_ptr;
{
	jr_TestShellVarInfoType *	var_info;

	jr_TestShellForEachVarInfoPtr (shell_info, var_info) {
		if (jr_TestShellVarInfoDataPtr (var_info)  ==  data_ptr) {
			return jr_TestShellVarInfoName (var_info);
		}
	}
	return "<not-found>";
}

void jr_TestShellDestroyIfNotVarDataPtr (shell_info, token_number, data_ptr)
	jr_TestShellType *		shell_info;
	jr_int					token_number;
	void *					data_ptr;
{
	jr_TestShellVarInfoType *	var_info;

	jr_TestShellForEachVarInfoPtr (shell_info, var_info) {
		if (jr_TestShellVarInfoDataPtr (var_info)  ==  data_ptr) {
			return;
		}
	}
	jr_TestShellVarDataUndo (
		shell_info, token_number, data_ptr
	);
}

jr_TestShellVarInfoType *jr_TestShellIsVarDataPtr (shell_info, data_ptr)
	jr_TestShellType *		shell_info;
	void *					data_ptr;
{
	jr_TestShellVarInfoType *	var_info;

	jr_TestShellForEachVarInfoPtr (shell_info, var_info) {
		if (jr_TestShellVarInfoDataPtr (var_info)  ==  data_ptr) {
			return var_info;
		}
	}
	return 0;
}

void jr_TestShellVarInfoInit (var_info, variable_name, token_number, data_ptr)
	jr_TestShellVarInfoType *	var_info;
	const char *				variable_name;
	jr_int						token_number;
	const void *				data_ptr;
{
	var_info->variable_name		= jr_strdup (variable_name);
	var_info->token_number		= token_number;
	var_info->data_ptr			= data_ptr;
}

void jr_TestShellVarInfoUndo (shell_info, var_info)
	jr_TestShellType *			shell_info;
	jr_TestShellVarInfoType *	var_info;
{
	if (var_info->data_ptr) {
		/*
		 * VarDataUndo is supplied by the programmer
		 */
		jr_TestShellVarDataUndo (
			shell_info, var_info->token_number, var_info->data_ptr
		);
	}
	jr_free (var_info->variable_name);
}

jr_int jr_TestShellVarInfoHash (void_arg_1)
	const void *					void_arg_1;
{
	const jr_TestShellVarInfoType *	var_info			= void_arg_1;

	return jr_strhash_pjw (var_info->variable_name);
}


jr_int jr_TestShellVarInfoCmp (void_arg_1, void_arg_2)
	const void *					void_arg_1;
	const void *					void_arg_2;
{
	const jr_TestShellVarInfoType *	var_info_1			= void_arg_1;
	const jr_TestShellVarInfoType *	var_info_2			= void_arg_2;

	jr_int							diff;

	diff = strcmp (var_info_1->variable_name, var_info_2->variable_name);

	if (diff != 0) {
		return diff;
	}

	return 0;
}

jr_int jr_TestShellVarInfoPtrCmp (void_arg_1, void_arg_2)
	const void *					void_arg_1;
	const void *					void_arg_2;
{
	jr_TestShellVarInfoType **	var_info_ptr_1		= (void *) void_arg_1;
	jr_TestShellVarInfoType **	var_info_ptr_2		= (void *) void_arg_2;

	return jr_TestShellVarInfoCmp (*var_info_ptr_1, *var_info_ptr_2);
}



void jr_TestShellHelpInit (help_info, command_name, help_message)
	jr_TestShellHelpType *		help_info;
	char *						command_name;
	char *						help_message;
{
	help_info->command_name		= jr_strdup (command_name);
	help_info->help_message		= jr_strdup (help_message);
}

void jr_TestShellHelpUndo (help_info)
	jr_TestShellHelpType *		help_info;
{
	jr_free (help_info->command_name);
	jr_free (help_info->help_message);
}

void jr_TestShellBeforeRecursion (shell_info)
	jr_TestShellType *		shell_info;
{
	shell_info->recursive_call ++;
	shell_info->continue_loop	= 0;
	shell_info->break_loop		= 0;
}

void jr_TestShellAfterRecursion (shell_info)
	jr_TestShellType *		shell_info;
{
	shell_info->recursive_call --;
	shell_info->continue_loop	= 0;
	shell_info->break_loop		= 0;
	shell_info->quit			= 0;
}

void jr_TestShellAddExpressionHelpMessages (shell_info)
	jr_TestShellType *				shell_info;
{
	jr_TestShellAddHelpMessage (shell_info, "", "");
	jr_TestShellAddHelpMessage (shell_info, "key",
		"<key>  stands for                        : \"test\", <integer>, index <N>, index end");
	jr_TestShellAddHelpMessage (shell_info, "key",
		"<key>  stands for                        : ['a,' 15], meaning string of 15 'a's");
	jr_TestShellAddHelpMessage (shell_info, "data",
		"<data> stands for                        : \"test\" or ['a' 15]");
	jr_TestShellAddHelpMessage (shell_info, "", "");

	jr_TestShellAddHelpMessage (shell_info, "expressions",
		"<I> = <expr>                             : <expr> may involve +, -, /, %, (), min(), max()");
	jr_TestShellAddHelpMessage (shell_info, "assert",
		"assert <expr> == <expr>                  : assert the expressions are equal");
	jr_TestShellAddHelpMessage (shell_info, "assert",
		"assert <expr> != <expr>                  : assert the expressions are not equal");
}

jr_int _jr_TestShellLineNumber (shell_info)
	jr_TestShellType *		shell_info;
{
	jr_int					line_number;

	line_number	= jr_IB_CurrentContextLine (shell_info->input_buffer);

	return line_number;
}


char * _jr_TestShellFileName (shell_info)
	jr_TestShellType *		shell_info;
{
	return jr_IB_CurrentContextName (shell_info->input_buffer);
}

void jr_TestShellPrintLocation (
	jr_TestShellType *		shell_info,
	FILE *					opt_wfp)
{
	if (opt_wfp == 0) {
		opt_wfp				= stderr;
	}
	fprintf (opt_wfp, "%s:%d\n",
		jr_TestShellFileName (shell_info),
		jr_TestShellLineNumber (shell_info)
	);
}

void jr_TestShellBreakPoint (
	jr_TestShellType *		shell_info,
	jr_int					is_for_command)
{
	jr_int					line_number;
	const char *			file_name;

	/*
	 * 7/28/2005: for use in the debugger to break before a certain line.
	 */

	line_number	= jr_IB_CurrentContextLine (shell_info->input_buffer);
	file_name	= jr_IB_CurrentContextName (shell_info->input_buffer);

	if (line_number || file_name) {
		return;
	}
}

void jr_TestInterfaceInit(
	jr_TestInterfaceType *		interface_ptr,
	const char *		host_name,
	jr_int				port_number,
	jr_int				max_msg_size,
	jr_int				max_bits_per_second)
{
	memset( interface_ptr, 0, sizeof( *interface_ptr));

	interface_ptr->host_name			= jr_strdup( host_name);
	interface_ptr->port_number			= port_number;
	interface_ptr->max_msg_size			= max_msg_size;
	interface_ptr->max_bits_per_second	= max_bits_per_second;
}

void jr_TestInterfaceUndo(
	jr_TestInterfaceType *		interface_ptr)
{
	jr_free( interface_ptr->host_name);
}

