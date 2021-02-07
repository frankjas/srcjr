extern "C" {
#	include "jr/malloc.h"
}
#include <new>

#ifdef ostype_winnt
	#pragma warning( disable : 4290 )

	/*
	** 3/23/07: for MS compiler, which ignores throw() arguments
	*/
#endif

#if __GNUC__ >= 7 && __GNUC_MINOR__ >= 5
#define _jr_THROW_BAD_ALLOC
#else
#define _jr_THROW_BAD_ALLOC throw(std::bad_alloc)
#endif

#if defined(ostype_macosx) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 2
#define _jr_NOEXCEPT _NOEXCEPT
#else
#define _jr_NOEXCEPT
#endif

/*
 * overload the global new and delete operators
 * to perform bounds checking, etc.
 */

void* operator new(std::size_t num_bytes, const std::nothrow_t&) throw()
{
	void *	ptr;

	jr_malloc_set_called_by_new (1);
	{
		ptr	= jr_malloc (num_bytes);
	}
	jr_malloc_set_called_by_new (0);

	return ptr;
}

void *operator new(std::size_t num_bytes) _jr_THROW_BAD_ALLOC
{
	void *	ptr;

	/*
	** 3/22/07: same signature as: void * operator new (size_t num_bytes)
	*/
	jr_malloc_set_called_by_new (1);
	{
		ptr	= jr_malloc (num_bytes);
	}
	jr_malloc_set_called_by_new (0);

	if (ptr == 0) {
		std::bad_alloc	e;

		throw e;
	}
	return ptr;
}

void *operator new[](std::size_t num_bytes) _jr_THROW_BAD_ALLOC
{
	void *	ptr;

	jr_malloc_set_called_by_new (1);
	{
		ptr	= jr_malloc (num_bytes);
	}
	jr_malloc_set_called_by_new (0);

	if (ptr == 0) {
		std::bad_alloc	e;

		throw e;
	}
	return ptr;
}


void *operator new[](std::size_t num_bytes, std::nothrow_t const&) throw()
{
	void *	ptr;

	jr_malloc_set_called_by_new (1);
	{
		ptr	= jr_malloc (num_bytes);
	}
	jr_malloc_set_called_by_new (0);

	return ptr;
}

void operator delete (void *ptr) _jr_NOEXCEPT
{
	jr_free (ptr);
}


void operator delete (void *ptr, std::size_t)
{
	/*
	** 8-14-2020: this decl needed as of gcc 7.5.0
	*/
	jr_free (ptr);
}


void operator delete[] (void *ptr) _jr_NOEXCEPT
{
	jr_free (ptr);
}

void operator delete[] (void *ptr, std::size_t)
{
	/*
	** 8-14-2020: this decl needed as of gcc 7.5.0
	*/
	jr_free (ptr);
}
