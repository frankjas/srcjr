#include "pfx_proj.h"

jr_PrefixInfoType	jr_PrefixInfoArray [jr_PREFIX_MAX_TYPES];
jr_int				jr_PrefixNumTypes									= 0;

static jr_mutex_t	jr_PrefixMutex[1];

jr_thread_once_declare_and_init (ThreadOnceControl);


void jr_PrefixMutexInit ()
{
	jr_mutex_init (jr_PrefixMutex, 0);
}

void jr_PrefixAddContainerType (
						container_name, element_prefix_size_fn,
						prefix_transform_fn, set_prefix_size_fn, needs_transform_fn
					)
	char *			container_name;
	jr_int			(*element_prefix_size_fn)();
	void 			(*prefix_transform_fn) ();
	void 			(*set_prefix_size_fn) ();
	jr_int			needs_transform_fn;
{
	jr_thread_once_call (&ThreadOnceControl, jr_PrefixMutexInit);
	jr_mutex_lock (jr_PrefixMutex);

	if (jr_PrefixNumTypes  >=  jr_PREFIX_MAX_TYPES) {
		fprintf (stderr, "jr_PrefixAddContainerType(%s): Too many prefix types, max. is %d\n",
			container_name, jr_PREFIX_MAX_TYPES
		);
		abort ();
	}

	jr_PrefixNumTypes ++;

	jr_PrefixInfoArray [jr_PrefixNumTypes-1].prefix_id				= jr_PrefixNumTypes - 1;
	jr_PrefixInfoArray [jr_PrefixNumTypes-1].element_prefix_size_fn	= element_prefix_size_fn;
	jr_PrefixInfoArray [jr_PrefixNumTypes-1].prefix_transform_fn	= prefix_transform_fn;
	jr_PrefixInfoArray [jr_PrefixNumTypes-1].set_prefix_size_fn		= set_prefix_size_fn;
	jr_PrefixInfoArray [jr_PrefixNumTypes-1].needs_transform_fn		= needs_transform_fn;

	/*
	 * Reserve an entry, whose container_name field is guaranteed to be 0
	 */

	jr_PrefixInfoArray [jr_PrefixNumTypes-1].container_name			= container_name;
	/*
	 * Put the container name in last, so that we don't have to mutex lock
	 * the code which uses this type info array.  That code always
	 * stops when the container name is 0.
	 */

	jr_mutex_unlock (jr_PrefixMutex);
}

