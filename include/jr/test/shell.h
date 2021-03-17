#ifndef __tstcmds_h___
#define __tstcmds_h___

#include "ezport.h"

#include <stdio.h>
#include <time.h>

#include "jr/inputbuf.h"
#include "jr/alist.h"
#include "jr/htable.h"
#include "jr/llist.h"

typedef struct {
	jr_IB_InputBuffer	input_buffer [1];
	jr_AList			command_list [1];
	jr_int				status;

	char *				prompt_str;
	char *				script_file;
	FILE *				script_rfp;

	jr_int				(*parse_fn)		();
	jr_int				(*scan_fn)		();
	void				(*var_undo_fn)	();
	void				(*var_print_fn) ();

	jr_LList			help_info_list [1];
	jr_HTable			variable_table [1];

	jr_AList			include_dir_list [1];

	jr_int				recursive_call;

	unsigned verbose					: 1;
	unsigned save_command				: 1;
	unsigned do_history					: 1;
	unsigned quit						: 1;
	unsigned echo						: 1;
	unsigned break_on_bad_status		: 1;
	unsigned save_file_commands			: 1;
	unsigned continue_loop				: 1;
	unsigned break_loop					: 1;
	unsigned assert_next_is_bad			: 1;
	unsigned check_malloc_bounds		: 1;
} jr_TestShellType ;


extern void			jr_TestShellInit			PROTO ((
						jr_TestShellType *		shell_info,
						jr_int					(*parse_fn)		(),
						jr_int					(*scan_fn)		(),
						void					(*var_undo_fn)	(),
						void					(*var_print_fn) (),
						const char *			prompt_str
					));

extern void			jr_TestShellUndo			PROTO ((
						jr_TestShellType *		shell_info
					));


extern jr_int		jr_TestShellReadFromFileName	PROTO ((
						jr_TestShellType *		shell_info,
						const char *			file_name,
						char *					error_buf
					));

extern jr_int		jr_TestShellReadFromFilePtr	PROTO ((
						jr_TestShellType *		shell_info,
						FILE *					rfp,
						const char *			file_name,
						char *					error_buf
					));

extern jr_int		jr_TestShellRead			PROTO ((
						jr_TestShellType *		shell_info,
						char *					error_buf
					));

extern jr_int		jr_TestShellReadString		PROTO ((
						jr_TestShellType *		shell_info,
						char *					input_string,
						char *					error_buf
					));

extern void			jr_TestShellSetFilePtr	PROTO ((
						jr_TestShellType *		shell_info,
						FILE *					rfp,
						const char *			file_name
					));

extern void			jr_TestShellEmptyHistory			PROTO ((
						jr_TestShellType *				shell_info
					));

extern void			jr_TestShellAddIncludeDir	PROTO((
						jr_TestShellType *		shell_info,
						const char *			include_dir
					));

extern jr_int		jr_TestShellPushNewInputFile		PROTO ((
						jr_TestShellType *				shell_info,
						const char *					file_name,
						char *							error_buf
					));

extern void			jr_TestShellSave					PROTO ((
						jr_TestShellType *				shell_info,
						FILE *							wfp
					));


extern void			jr_TestShellAddHelpMessage		PROTO ((
						jr_TestShellType *				shell_info,
						char *							command_name,
						char *							help_message
					));

extern void			jr_TestShellPrintHelpMessage		PROTO ((
						jr_TestShellType *				shell_info,
						char *							command_name,
						FILE *							wfp
					));

extern void			jr_TestShellAddStandardHelpMessages	PROTO ((
						jr_TestShellType *				shell_info
					));

extern void			jr_TestShellBeforeRecursion			PROTO ((
						jr_TestShellType *				shell_info
					));

extern void			jr_TestShellAfterRecursion			PROTO ((
						jr_TestShellType *				shell_info
					));

extern void			jr_TestShellBreakPoint (
						jr_TestShellType *				shell_info,
						jr_int							is_for_command);

extern void			jr_TestShellPrintLocation (
						jr_TestShellType *				shell_info,
						FILE *							opt_wfp);


typedef struct {
	char *		line_str;

	unsigned	save_command			: 1;
} jr_TestShellLineStruct;


#define jr_TestShellIsVerbose(shell_info)				((shell_info)->verbose)
#define jr_TestShellStatus(shell_info)					((shell_info)->status)
#define jr_TestShellHasBadStatus(shell_info)			((shell_info)->status == -1)

#define jr_TestShellQuit(shell_info)					((shell_info)->quit)
#define jr_TestShellBreakLoop(shell_info)				((shell_info)->break_loop)
#define jr_TestShellContinueLoop(shell_info)			((shell_info)->continue_loop)
#define jr_TestShellInRecursion(shell_info)				((shell_info)->recursive_call)
#define jr_TestShellEndRecursion(shell_info)			((shell_info)->end_recursion)
#define jr_TestShellCheckMallocBounds(shell_info)		((shell_info)->check_malloc_bounds)

#define jr_TestShellDontSaveCommand(shell_info)			((shell_info)->save_command = 0)
#define jr_TestShellSetBadStatus(shell_info)			((shell_info)->status = -1)
#define jr_TestShellSetQuit(shell_info, v)				((shell_info)->quit = (v) != 0)
#define jr_TestShellSetBreakLoop(shell_info, v)			((shell_info)->break_loop = (v) != 0)
#define jr_TestShellSetContinueLoop(shell_info, v)		((shell_info)->continue_loop = (v) != 0)
#define jr_TestShellSetBreakOnBadStatus(shell_info, v)	((shell_info)->break_on_bad_status = (v) != 0)
#define jr_TestShellSetEndRecursion(shell_info, v)		((shell_info)->end_recursion = (v) != 0)
#define jr_TestShellSetCheckMallocBounds(shell_info, v)	((shell_info)->check_malloc_bounds = (v) != 0)

#define jr_TestShellAssertNextCommandIsBad(shell_info)	((shell_info)->assert_next_is_bad = 1)

#define jr_TestShellFileName(shell_info)				\
		jr_IB_CurrentContextName ((shell_info)->input_buffer)

#define jr_TestShellFilePtr(shell_info)					\
		jr_IB_CurrentContextRfp ((shell_info)->input_buffer)

#define jr_TestShellLineNumber(shell_info)				\
		(jr_IB_CurrentContextLine ((shell_info)->input_buffer) - 1)

#define jr_TestShellIsInteractive(shell_info)			\
		(jr_IB_CurrentContextRfp ((shell_info)->input_buffer) == stdin)


/******** Variable Support Functions ********/

typedef struct {
	const char *	variable_name;

	jr_int			token_number;
	const void *	data_ptr;
} jr_TestShellVarInfoType;


extern void				jr_TestShellSetVarInfo			PROTO ((
							jr_TestShellType *			shell_info,
							const char *				var_name,
							jr_int						token_number,
							const void *				data_ptr
						));

extern jr_TestShellVarInfoType *jr_TestShellGetVarInfo	PROTO ((
							jr_TestShellType *			shell_info,
							const char *				var_name
						));

extern void *			jr_TestShellGetDataPtr			PROTO ((
							jr_TestShellType *			shell_info,
							const char *				var_name
						));

#define					jr_TestShellGetStringValue(shell_info, var_name)	\
						((const char *)jr_TestShellGetDataPtr (shell_info, var_name))

extern jr_int			jr_TestShellGetIntValue			PROTO ((
							jr_TestShellType *			shell_info,
							char *						var_name,
							jr_int						integer_tkn,
							jr_int						ulong_tkn
						));

extern void				jr_TestShellSetIntValue			PROTO ((
							jr_TestShellType *			shell_info,
							char *						var_name,
							jr_int						integer_tkn,
							jr_int						value
						));

extern jr_int			jr_TestShellDeleteVariable	PROTO ((
							jr_TestShellType *			shell_info,
							const char *				var_name
						));

extern void *			jr_TestShellExtractVariable	PROTO ((
							jr_TestShellType *			shell_info,
							const char *				var_name
						));

extern void				jr_TestShellPrintVariables	PROTO ((
							jr_TestShellType *			shell_info,
							jr_int						is_diagnostic,
							FILE *						wfp
						));

#define					jr_TestShellVarDataUndo(shell_info, token_number, data_ptr)		\
						(*(shell_info)->var_undo_fn) (shell_info, token_number, data_ptr)

#define					jr_TestShellPrintVariable(shell_info, var_info, is_diagnostic, wfp) \
						(*(shell_info)->var_print_fn) (shell_info, var_info, is_diagnostic, wfp)



extern const char *		jr_TestShellVarDataPtrName	PROTO ((
							jr_TestShellType *			shell_info,
							void *						data_ptr
						));

extern void				jr_TestShellDestroyIfNotVarDataPtr	PROTO ((
							jr_TestShellType *			shell_info,
							jr_int						token_number,
							void *						data_ptr
						));

extern jr_TestShellVarInfoType *jr_TestShellIsVarDataPtr	PROTO ((
							jr_TestShellType *			shell_info,
							void *						data_ptr
						));

extern void				jr_TestShellVarInfoInit		PROTO ((
							jr_TestShellVarInfoType *	var_info,
							const char *				variable_name,
							jr_int						token_number,
							const void *				data_ptr
						));

extern void				jr_TestShellVarInfoUndo		PROTO ((
							jr_TestShellType *			shell_info,
							jr_TestShellVarInfoType *	var_info
						));

extern jr_int			jr_TestShellVarInfoHash		PROTO ((
							const void *				void_arg_1
						));

extern jr_int			jr_TestShellVarInfoCmp		PROTO ((
							const void *				void_arg_1,
							const void *				void_arg_2
						));

extern jr_int			jr_TestShellVarInfoPtrCmp		PROTO ((
							const void *				void_arg_1,
							const void *				void_arg_2
						));

#define					jr_TestShellForEachVarInfoPtr(shell_info, var_info)			\
						jr_HTableForEachElementPtr ((shell_info)->variable_table, var_info)

#define					jr_TestShellVarInfoName(var_info)							\
						((var_info)->variable_name)

#define					jr_TestShellVarInfoTokenNumber(var_info)					\
						((var_info)->token_number)

#define					jr_TestShellVarInfoDataPtr(var_info)						\
						((void *) ((var_info)->data_ptr))

#define					jr_TestShellVarInfoSetDataPtr(var_info, v)					\
						((var_info)->data_ptr = (v))

/******** Help Support ********/

typedef struct {
	char *			command_name;
	char *			help_message;
} jr_TestShellHelpType;


extern void			jr_TestShellHelpInit			PROTO ((
						jr_TestShellHelpType *		help_info,
						char *						command_name,
						char *						help_message
					));

extern void			jr_TestShellHelpUndo			PROTO ((
						jr_TestShellHelpType *		help_info
					));

#define				jr_TestShellHelpCommandName(help_info)		((help_info)->command_name)
#define				jr_TestShellHelpMessage(help_info)			((help_info)->help_message)


extern void			jr_TestShellAddExpressionHelpMessages	PROTO ((
						jr_TestShellType *				shell_info
					));

/******** Inet Interfaces ********/

typedef struct {
	const char *			host_name;
	jr_int					port_number;
	jr_int					max_msg_size;
	jr_int					max_bits_per_second;
} jr_TestInterfaceType;

extern void			jr_TestInterfaceInit(
						jr_TestInterfaceType *		interface_ptr,
						const char *		host_name,
						jr_int				port_number,
						jr_int				max_msg_size,
						jr_int				max_bits_per_second);

extern void			jr_TestInterfaceUndo(
						jr_TestInterfaceType *		interface_ptr);

#define				jr_TestInterfaceHostName( interface_ptr)			((interface_ptr)->host_name)
#define				jr_TestInterfacePortNumber( interface_ptr)			((interface_ptr)->port_number)
#define				jr_TestInterfaceMaxMsgSize( interface_ptr)			((interface_ptr)->max_msg_size)
#define				jr_TestInterfaceMaxBitsPerSecond( interface_ptr)	((interface_ptr)->max_bits_per_second)

#endif
