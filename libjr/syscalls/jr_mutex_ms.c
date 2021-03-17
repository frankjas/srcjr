#include "ezport.h"
/*
** 3/9/07: in case ostype_winnt is defined in ezport.h
*/

#ifdef ostype_winnt

#include <windows.h>
#include <string.h>

#include "jr/apr.h"

typedef struct {
	HANDLE				handle;
	CRITICAL_SECTION	crit_section;
} jr_mutex_ms_t;

void jr_mutex_init (
	jr_mutex_t *				mutex_ptr,
	jr_int						is_recursive)
{
	jr_mutex_ms_t *				ms_info_ptr				= (void *) mutex_ptr->arch_data.microsoft_data;

	if (sizeof (jr_mutex_ms_t) > sizeof (mutex_ptr->arch_data.microsoft_data)) {
		abort();
	}
	if (is_recursive) {
		InitializeCriticalSection (&(ms_info_ptr)->crit_section);
	}
	else {
		ms_info_ptr->handle		= CreateEvent(NULL, FALSE, TRUE, NULL);
	}

	mutex_ptr->is_recursive		= is_recursive != 0;
}

void jr_mutex_undo (mutex_ptr)
	jr_mutex_t *				mutex_ptr;
{
	jr_mutex_ms_t *				ms_info_ptr				= (void *) mutex_ptr->arch_data.microsoft_data;

	if (mutex_ptr->is_recursive) {
		DeleteCriticalSection (&(ms_info_ptr)->crit_section);
	}
	else {
		CloseHandle (ms_info_ptr->handle);
	}
}

void jr_mutex_lock (mutex_ptr)
	jr_mutex_t *				mutex_ptr;
{
	jr_mutex_ms_t *				ms_info_ptr				= (void *) mutex_ptr->arch_data.microsoft_data;

	if (mutex_ptr->is_recursive) {
		EnterCriticalSection (&(ms_info_ptr)->crit_section);
	}
	else {
		WaitForSingleObject(ms_info_ptr->handle, INFINITE);
		/*
		** 3/2/07: can only return WAIT_OBJECT_0 (success) or WAIT_ABONDONED, which means
		** the thread holding the mutex died before releasing it,
		** which means the resource is free or the thread exitted unexpectedly,
		** which means there's a bug in the program
		*/
	}
}

void jr_mutex_unlock (mutex_ptr)
	jr_mutex_t *				mutex_ptr;
{
	jr_mutex_ms_t *				ms_info_ptr				= (void *) mutex_ptr->arch_data.microsoft_data;

	if (mutex_ptr->is_recursive) {
		LeaveCriticalSection (&(ms_info_ptr)->crit_section);
	}
	else {
        if (SetEvent(ms_info_ptr->handle) == 0) {
            /*
			** 3/2/07: error case.
			*/
        }
	}
}

/*
** 3/2/07: verify that the declaration macro is syntactically valid
*/
jr_thread_once_declare_and_init (TestControl);


void jr_thread_once_call (
	jr_thread_once_t *			once_control_ptr,
	void						(*some_func)())
{
	if (0) {
		TestControl.control_value	= 0;			/* 3/2/07: shutup compiler warning */
	}

	if (!InterlockedExchange(&once_control_ptr->control_value, 1)) {
		(*some_func) ();
	}
}


#else

#ifdef ostype_macosx
#pragma GCC diagnostic ignored "-Winfinite-recursion"
#endif

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
#endif
