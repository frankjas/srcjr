#include "ezport.h"

#include <apr-1/apr_pools.h>

#include "jr/apr.h"
#include "jr/malloc.h"

void jr_pool_init (
	jr_pool_t *					pool_ptr,
	jr_pool_t *					opt_parent_pool_ptr)
{
	memset (pool_ptr, 0, sizeof (*pool_ptr));

	pool_ptr->parent_pool_ptr	= opt_parent_pool_ptr;

	jr_apr_initialize();
}

void jr_pool_undo (
	jr_pool_t *					pool_ptr)
{
	if (pool_ptr->apr_pool_ptr) {
		/*
		** 5/13/08: If there's no pool, there's either no apr_file_ptr or it's the write
		** of a pipe, which will be closed when the parent pipe is closed.
		*/
		apr_pool_destroy (pool_ptr->apr_pool_ptr);
	}
}

jr_pool_t *jr_pool_create(
	jr_pool_t *					opt_parent_pool_ptr)
{
	jr_pool_t *					pool_ptr;

	if (opt_parent_pool_ptr) {
		pool_ptr	= jr_pool_alloc( opt_parent_pool_ptr, sizeof(*pool_ptr));
	}
	else {
		pool_ptr	= jr_malloc( sizeof(*pool_ptr));
	}
	jr_pool_init( pool_ptr, opt_parent_pool_ptr);

	return pool_ptr;
}

void jr_pool_destroy (
	jr_pool_t *					pool_ptr)
{
	jr_pool_t *					parent_pool_ptr = pool_ptr->parent_pool_ptr;

	jr_pool_undo( pool_ptr);

	if (parent_pool_ptr == 0) {
		jr_free( pool_ptr);
	}
}

void *jr_pool_alloc(
	jr_pool_t *					pool_ptr,
	size_t						mem_size)
{
	jr_int						status;

	if (pool_ptr->apr_pool_ptr == 0) {
		
		status = apr_pool_create(
					(apr_pool_t **) &pool_ptr->apr_pool_ptr, (apr_pool_t *) pool_ptr->parent_pool_ptr
				);

		if (status != 0) {
			return 0;
		}
	}
	return apr_palloc( pool_ptr->apr_pool_ptr, mem_size);
}


char *jr_pool_strdup( 
	jr_pool_t *					pool_ptr,
	const char *				src_str)
{
	char *						new_str = jr_pool_alloc( pool_ptr, strlen( src_str) + 1);
	jr_int						z;

	if (new_str == 0) {
		return 0;
	}

	for (z=0; src_str[z] != 0; z++) {
		new_str[z] = src_str[z];
	}
	new_str[z] = 0;

	return new_str;
}
