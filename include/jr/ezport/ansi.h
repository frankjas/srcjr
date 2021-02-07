#ifndef __EZPORT_ANSI_H__
#define __EZPORT_ANSI_H__

/*
 * The ANSI standard specifies that there should be a #define for
 * the constant __STDC__ if you are using an ANSI compiler.
 *
 * 3/9/07: some compilers don't define __STDC__ even if the support
 * ANSI constructs when they also add other non-ANSI constructs.
 * So use the constant "compiler_has_ansi_support" instead,
 * which is now true for all compilers.
 */

#define compiler_has_ansi_support

#define PROTO(args)	args
/*
** 1/23/07: PROTO is needed for old-style JR header files
*/

#if 0
	/*
	** 1/23/07: Obsolete, non-ANSI compilers don't exist any longer
	*/

#	define PROTO(args)	()

#	ifdef _change_void_to_char
#	ifndef void
#		define void char
#	endif
#	endif

#	ifndef const
#		define const	/* const not supported in old C */
#	endif

	/*
	 * If we are not using an ANSI compiler and we have not
	 * specified any defines for "void" then replace every
	 * occurence of the word "void" with the word "char".
	 * This will have the effect of changing references
	 * to  (void *)  type into  (char *)  type.
	 * This will result in some type mismatch warnings,
	 * but all code should still be semantically equivalent.
	 */ 
#endif

#endif
