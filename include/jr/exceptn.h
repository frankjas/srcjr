#ifndef _exception_h_
#define _exception_h_

#include "ezport.h"

#include <setjmp.h>

#include "jr/htable.h"

/*
 * We don't want to require _POSIX_SOURCE to include this
 * file.  So we use what we know is available in either case
 */
#ifdef _POSIX_SOURCE
#	define jr_sigsetjmp		sigsetjmp
#	define jr_sigjmp_buf	sigjmp_buf
#else
#	define jr_sigsetjmp		setjmp
#	define jr_sigjmp_buf	jmp_buf
#endif


typedef struct {
	jr_sigjmp_buf	jump_buffer;
	void			(*handler_fn) ();

	char *			error_buf;
	jr_int			num_references;

	unsigned 		core_dump				: 1;
} jr_ExceptionType;


extern jr_int			jr_ExceptionRaiseWithCoreDump	PROTO ((
								char *					exception_name,
								jr_int					core_dump,
								char *					opt_error_buf
						));

extern jr_int			jr_ExceptionBreakPoint			PROTO ((
							jr_int		status
						));

#define jr_ExceptionRaiseOrCoreDump(exception_name, opt_error_buf)		\
		jr_ExceptionRaiseWithCoreDump (exception_name, 1, opt_error_buf)

#define jr_ExceptionRaise(exception_name, opt_error_buf)		\
		jr_ExceptionRaiseWithCoreDump (exception_name, 0, opt_error_buf)



#define jr_ExceptionCatch(exception_name, error_buf)						\
		jr_sigsetjmp (															\
			jr_ExceptionGetPtr (exception_name, error_buf)->jump_buffer, 1	\
		)

#define jr_ExceptionCatchVector(exception_vector, error_buf)			\
		jr_sigsetjmp (														\
			jr_ExceptionGetPtrFromVector (exception_vector, error_buf)->jump_buffer, 1	\
		)

extern jr_ExceptionType *	jr_ExceptionGetPtr				PROTO ((
								char *				exception_name,
								char *				error_buf
							));

extern jr_ExceptionType *	jr_ExceptionGetPtrFromVector	PROTO ((
								char **				exception_vector,
								char *				error_buf
							));

extern void		jr_ExceptionUnCatchVector	PROTO ((char **exception_vector));
extern void		jr_ExceptionUnCatch			PROTO ((char *exception_name));

extern void		jr_ExceptionSetCoreDumpOnRaise		PROTO ((
						char *		exception_name,
						jr_int		value
				));

#define jr_ExceptionCoreDumpOnRaise(exception_name)			\
		jr_ExceptionSetCoreDumpOnRaise (exception_name, 1)

#define jr_ExceptionCatchOnRaise(exception_name)			\
		jr_ExceptionSetCoreDumpOnRaise (exception_name, 0)

extern jr_HTable	jr_ExceptionTable [1];

#define jr_ExceptionForEachName(index, exception_name)				\
		jr_HTableForEachElementIndex (jr_ExceptionTable, index)		\
			if (exception_name = jr_HTableKeyPtrValue (jr_ExceptionTable, index))

#endif
