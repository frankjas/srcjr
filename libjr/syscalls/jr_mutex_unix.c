#include "ezport.h"
/*
** 3/9/07: in case ostype_winnt is defined in ezport.h
*/

#ifndef ostype_winnt

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "jr/apr.h"


#if jr_SIZEOF_PTHREAD_MUTEX_T < __SIZEOF_PTHREAD_MUTEX_T
	/*
	** 3/2/07: verify declaration assumptions
	*/
#	error jr_SIZEOF_PTHREAD_MUTEX_T < __SIZEOF_PTHREAD_MUTEX_T
#endif


void jr_mutex_init (
	jr_mutex_t *				mutex_ptr,
	jr_int						is_recursive)
{
	pthread_mutex_t *			pthread_mutex_ptr	= (void *) mutex_ptr->arch_data.pthread_data;
	pthread_mutexattr_t			attr[1];

	pthread_mutexattr_init (attr);

	if (is_recursive) {
		pthread_mutexattr_settype (attr, PTHREAD_MUTEX_RECURSIVE);
	}
	else {
#		ifdef PTHREAD_MUTEX_FAST_NP
			pthread_mutexattr_settype (attr, PTHREAD_MUTEX_FAST_NP);
#		else
			pthread_mutexattr_settype (attr, PTHREAD_MUTEX_NORMAL);
#		endif
	}

	pthread_mutex_init (pthread_mutex_ptr, attr);

	pthread_mutexattr_destroy (attr);

	mutex_ptr->is_recursive		= is_recursive != 0;
}

void jr_mutex_undo (mutex_ptr)
	jr_mutex_t *				mutex_ptr;
{
	pthread_mutex_t *			pthread_mutex_ptr	= (void *) mutex_ptr->arch_data.pthread_data;

	pthread_mutex_destroy (pthread_mutex_ptr);
}

void jr_mutex_lock (mutex_ptr)
	jr_mutex_t *				mutex_ptr;
{
	pthread_mutex_t *			pthread_mutex_ptr	= (void *) mutex_ptr->arch_data.pthread_data;

	pthread_mutex_lock (pthread_mutex_ptr);
}

void jr_mutex_unlock (mutex_ptr)
	jr_mutex_t *				mutex_ptr;
{
	pthread_mutex_t *			pthread_mutex_ptr	= (void *) mutex_ptr->arch_data.pthread_data;

	pthread_mutex_unlock (pthread_mutex_ptr);
}


jr_int jr_thread_once_cmp( const void *void_ptr_1, const void *void_ptr_2)
{
	const jr_thread_once_t *	once_ptr_1	= void_ptr_1;
	const jr_thread_once_t *	once_ptr_2	= void_ptr_2;

#if defined(ostype_cygwin)

	if (once_ptr_1->control_value.mutex != once_ptr_2->control_value.mutex) {
		return 1;
	}

	if (once_ptr_1->control_value.state != once_ptr_2->control_value.state) {
		return 1;
	}

#elif defined(ostype_macosx)
	if (once_ptr_1->control_value.__sig != once_ptr_2->control_value.__sig) {
		return 1;
	}
	if (memcmp(
			once_ptr_1->control_value.__opaque, once_ptr_2->control_value.__opaque,
			jr_THREAD_OSX_ONCE_SIZE
		) != 0) {

		return 1;
	}
#else
	if (once_ptr_1->control_value != once_ptr_2->control_value) {
		return 1;
	}
#endif

	return 0;
}

/*
** 3/2/07: verify that the declaration macro is syntactically valid
*/
jr_thread_once_declare_and_init (TestControl);


#if defined(ostype_cygwin) || defined(ostype_sunos) || defined(ostype_macosx)

void jr_thread_once_call (
	jr_thread_once_t *			once_control_ptr,
	void						(*some_func)())
{
	pthread_once_t				tmp_once_value	= PTHREAD_ONCE_INIT;

	if (jr_thread_once_cmp (&tmp_once_value, &TestControl)  != 0) {
		/*
		** 3/15/07: make sure jr_thread_once_declare_and_init() is properly defined,
		** since it uses hardcoded values to remove the include dependency on pthread.h
		** (which only exists under some versions of Unix)
		*/
		abort();
	}

	pthread_once ((void*) &once_control_ptr->control_value, some_func);
}

#else


void jr_thread_once_call (
	jr_thread_once_t *			once_control_ptr,
	void						(*some_func)())
{
	if (PTHREAD_ONCE_INIT != 0) {
		abort();
		TestControl.control_value	= 0;			/* 3/2/07: shutup compiler warning */
	}

	pthread_once (&once_control_ptr->control_value, some_func);
}

#endif


#else

static void NotCalled ()	/* define this so ranlib doesn't complain */
{
	NotCalled ();			/* use it so the compiler doesn't complain */
}
#endif
