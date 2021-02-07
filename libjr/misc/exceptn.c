#include "exn_proj.h"

jr_HTable		jr_ExceptionTable [1];
static jr_int	DidInit = 0;

jr_int jr_ExceptionRaiseWithCoreDump (exception_name, core_dump, opt_error_buf)
	char *				exception_name;
	jr_int				core_dump;
	char *				opt_error_buf;
{
	jr_ExceptionStack *	exception_stack;
	jr_ExceptionType *	exception_ptr;
	jr_int				index;


	if (jr_ExceptionBreakPoint (0) != 0) {
		/*
		 * this function servers only as a place to
		 * set a break point in the debugger.  Its
		 * should be compiled with the debugger
		 * option on
		 */
		return (-1);
	}

	if (!DidInit) {
		jr_HTableInitForPtrValues (
			jr_ExceptionTable, jr_strhash_pjw,
			(int (*) (const void *, const void *)) strcmp
		);
		DidInit = 1;
	}

	index = jr_HTableFindPtrValueIndex (jr_ExceptionTable, exception_name);

	if (index == -1) {
		return (-1);
	}

	exception_stack = jr_HTableDataPtrValue (jr_ExceptionTable, index);

	if (core_dump && exception_stack->core_dump) {
		jr_coredump ();
	}

	if (jr_AListSize (exception_stack->jmp_buf_list) < 1) return (-1);

	exception_ptr = jr_AListNativeTail (exception_stack->jmp_buf_list, jr_ExceptionType *);

	if (core_dump && exception_ptr->core_dump) {
		jr_coredump ();
	}

	if (opt_error_buf) {
		strcpy (exception_ptr->error_buf, opt_error_buf);
	}
	else {
		strcpy (exception_ptr->error_buf, "<no msg>");
	}

	siglongjmp (exception_ptr->jump_buffer, -1);
}


jr_ExceptionType *jr_ExceptionGetPtrFromVector (exception_vector, error_buf)
	char **				exception_vector;
	char *				error_buf;
{
	jr_ExceptionStack *	exception_stack;
	jr_ExceptionType *	exception_ptr;
	char *				exception_name;
	jr_int				vector_length;
	jr_int				i, index;


	if (!DidInit) {
		jr_HTableInitForPtrValues (
			jr_ExceptionTable, jr_strhash_pjw,
			(int (*) (const void *, const void *)) strcmp
		);
		DidInit = 1;
	}

	exception_ptr	= jr_ExceptionCreate (error_buf);
	vector_length	= jr_VectorLength (exception_vector);

	for (i = 0;  i < vector_length; i++) {

		exception_name = exception_vector [i];

		index	= jr_HTableFindPtrValueIndex (jr_ExceptionTable, exception_name);

		if (index == -1) {
			char *		new_str;

			new_str			= jr_strdup (exception_name);
			index			= jr_HTableNewPtrValueIndex (jr_ExceptionTable, new_str);

			exception_stack	= jr_ExceptionStackCreate ();

			jr_HTableSetDataPtrValue (jr_ExceptionTable, index, exception_stack);
		}

		exception_stack = jr_HTableDataPtrValue (jr_ExceptionTable, index);

		jr_ExceptionStackPush (exception_stack, exception_ptr);
	}

	return (exception_ptr);
}


void jr_ExceptionUnCatchVector (exception_vector)
	char **				exception_vector;
{
	jr_ExceptionStack *	exception_stack;
	char *				exception_name;
	jr_int				vector_length;
	jr_int				i, index;


	if (!DidInit) {
		jr_HTableInitForPtrValues (
			jr_ExceptionTable, jr_strhash_pjw,
			(int (*) (const void *, const void *)) strcmp
		);
		DidInit = 1;
	}

	vector_length	= jr_VectorLength (exception_vector);

	for (i = 0;  i < vector_length; i++) {
		exception_name = exception_vector [i];

		index = jr_HTableFindPtrValueIndex (jr_ExceptionTable, exception_name);

		if (index == -1) {
			return;
		}

		exception_stack = jr_HTableDataPtrValue (jr_ExceptionTable, index);

		exception_stack = jr_ExceptionStackPop (exception_stack);

		if (exception_stack) {
			jr_ExceptionStackDestroy (exception_stack);

			exception_name	= jr_HTableKeyPtrValue (jr_ExceptionTable, index);

			jr_HTableDeleteIndex (jr_ExceptionTable, index);

			jr_free (exception_name);

			if (jr_HTableSize (jr_ExceptionTable) == 0) {
				jr_HTableUndo (jr_ExceptionTable);
				DidInit = 0;
			}
		}
	}
}


jr_ExceptionType *jr_ExceptionGetPtr (exception_name, error_buf)
	char *				exception_name;
	char *				error_buf;
{
	jr_ExceptionType *	exception_ptr;
	char *				exception_vector[2];

	exception_vector[0]	= exception_name;
	exception_vector[1]	= 0;


	exception_ptr = jr_ExceptionGetPtrFromVector (exception_vector, error_buf);

	return (exception_ptr);
}


void jr_ExceptionUnCatch (exception_name)
	char *				exception_name;
{
	char *				exception_vector[2];

	exception_vector[0]	= exception_name;
	exception_vector[1]	= 0;

	jr_ExceptionUnCatchVector (exception_vector);
}


void jr_ExceptionSetCoreDumpOnRaise (exception_name, value)
	char *		exception_name;
	jr_int		value;
{
	jr_ExceptionStack *	exception_stack;
	jr_int				index;


	if (!DidInit) {
		jr_HTableInitForPtrValues (
			jr_ExceptionTable, jr_strhash_pjw,
			(int (*) (const void *, const void *)) strcmp
		);
		DidInit = 1;
	}

	index = jr_HTableFindPtrValueIndex (jr_ExceptionTable, exception_name);

	if (index == -1) {
		return;
	}

	exception_stack = jr_HTableDataPtrValue (jr_ExceptionTable, index);

	exception_stack->core_dump = value;
}

jr_ExceptionType *jr_ExceptionCreate (error_buf)
	char *				error_buf;
{
	jr_ExceptionType *	exception_ptr;

	exception_ptr	= jr_malloc (sizeof (jr_ExceptionType));
	memset (exception_ptr, 0, sizeof (jr_ExceptionType));

	exception_ptr->error_buf	= error_buf;

	return (exception_ptr);
}

void jr_ExceptionDestroy (exception_ptr)
	jr_ExceptionType *	exception_ptr;
{
	jr_free (exception_ptr);
}

jr_ExceptionStack * jr_ExceptionStackCreate ()
{
	jr_ExceptionStack *	exception_stack;

	exception_stack = jr_malloc (sizeof (jr_ExceptionStack));

	jr_ExceptionStackInit (exception_stack);

	return (exception_stack);
}

void jr_ExceptionStackInit (exception_stack)
	jr_ExceptionStack *	exception_stack;
{
	memset ((void *)exception_stack, 0, sizeof (*exception_stack));

	jr_AListInit (exception_stack->jmp_buf_list, sizeof (jr_ExceptionType)); 
}

void jr_ExceptionStackDestroy (exception_stack)
	jr_ExceptionStack *	exception_stack;
{
	jr_ExceptionStackUndo (exception_stack);

	jr_free (exception_stack);
}

void jr_ExceptionStackUndo (exception_stack)
	jr_ExceptionStack *	exception_stack;
{
	jr_AListUndo (exception_stack->jmp_buf_list);
}

jr_ExceptionType *jr_ExceptionStackPush (exception_stack, exception_ptr)
	jr_ExceptionStack *	exception_stack;
	jr_ExceptionType *	exception_ptr;
{

	jr_AListNativeSetNewTail (
		exception_stack->jmp_buf_list, exception_ptr, jr_ExceptionType *
	);

	exception_ptr->num_references++;

	return (exception_ptr);
}


jr_ExceptionStack *jr_ExceptionStackPop (exception_stack)
	jr_ExceptionStack *	exception_stack;
{
	jr_ExceptionType *	exception_ptr;

	if (jr_AListSize (exception_stack->jmp_buf_list) > 0) {
		exception_ptr = jr_AListNativeTail (
			exception_stack->jmp_buf_list, jr_ExceptionType *
		);
		jr_AListDeletedTailPtr (exception_stack->jmp_buf_list);

		exception_ptr->num_references--;

		if (exception_ptr->num_references == 0) {
			jr_ExceptionDestroy (exception_ptr);
		}
	}
	if (jr_AListSize (exception_stack->jmp_buf_list) == 0) {
		return (exception_stack);
	}
	return (0);
}

