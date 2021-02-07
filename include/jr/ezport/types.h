#ifndef __jr_EZPORT_TYPES_H___
#define __jr_EZPORT_TYPES_H___

/*
** use #defines so 'unsigned jr_short' is valid syntax
** use jr_int for all 32 bit quantities
*/ 

#include <limits.h>
#include <stdint.h>

#if defined (__GNUC__)
/*
** 10/26/05 <float.h> is in gcc's private include dir, /usr/lib/gcc/...
** is indicated by GNU_INCLUDE_DIR for the JR parsing library.
** If this generates an error, check the value of that env. variable.
*/
#include <float.h>
#endif

#ifdef _WIN64
#	define jr_IS_64_BIT				1
#endif

#if __WORDSIZE == 64
#	define jr_IS_64_BIT				1
#endif

#if __LP64__ == 64
#	define jr_IS_64_BIT				1
#endif


#define jr_short		 	short

#define jr_CHAR_MAX			INT8_MAX
#define jr_UCHAR_MAX		UINT8_MAX
#define jr_SCHAR_MAX		INT8_MAX

#define jr_SHRT_MAX			INT16_MAX
#define jr_SHRT_MIN			INT16_MIN

#define jr_USHRT_MAX		UINT16_MAX

#define jr_INT_MAX			INT32_MAX
#define jr_INT_MIN			INT32_MIN
#define jr_UINT_MAX			UINT32_MAX

#define jr_FLOAT_MAX		FLT_MAX
#define jr_FLOAT_MIN		FLT_MIN

#ifdef replace_2_byte_int_with_4_byte_long
#	define jr_int			long
#else
#	define jr_int			int
#endif

#ifdef jr_IS_64_BIT
#	define jr_long			long
#else
#	define jr_long			long long
#endif

/*
 * These belong in jr/misc.h, but their values are justified
 * by the code here.  Any changes here requires changing these
 * constants
 */

#define jr_INT_BIT_SIZE_POWER		5		/* bits in an int are 2^5 = 32, used for division */ 
#define jr_SHORT_BIT_SIZE_POWER		4		/* bits in a short are 2^4 = 16 */
#define jr_CHAR_BIT_SIZE_POWER		3		/* bits in a char are 2^3 = 8  */

#define jr_INT_NUM_BITS				32
#define jr_INT_BIT_SIZE_MASK		(jr_INT_NUM_BITS - 1)	/* used for modulus */

#define jr_INT_NUM_BYTES			(jr_INT_NUM_BITS / 8)
#define jr_INT_NUM_BYTES_MASK		(jr_INT_NUM_BYTES - 1)	/* used for modulus */

/*
** 3/15/07: Under Unix/Linux, see pthread.h for definition of PTHREAD_ONCE_INIT
*/
#ifdef ostype_winnt
#	define				jr_PTHREAD_ONCE_INIT				{0}
#elif defined(ostype_cygwin)
#	if (__version__ <= 5100)
#		define			jr_PTHREAD_ONCE_INIT				{{(void*) 20, 0}}
#	else
#		define			jr_PTHREAD_ONCE_INIT				{{(void*) 19, 0}}
#	endif
#elif defined(ostype_macosx)
#	define				jr_PTHREAD_ONCE_INIT				{{0x30B1BCBA, {0}}}
#else
#	define				jr_PTHREAD_ONCE_INIT				{0}
#endif


/*
** 3/21/07: May only need to explicitly declare "dllimport" on global variables.
** It looks like the MS linker automatically links functions if the __imp__ version
** of an undefined function is available.
** NOTE: jr_EXTERN() can probably only be used on global vars going into libjr.dll.
** Due to MS's scheme, each library needs their own jr_EXTERN() and corresponding
** ifdefs.  See APR_DECLARE and APU_DECLARE (in "apr" and "apr-utils") as examples.
**
** NOTE: when compiling "libjr" files, define "is_libjr_build" to avoid warnings
*/
#if defined(ostype_winnt)  &&  defined(PIC) && !defined(is_libjr_build) 
#	define jr_EXTERN(type)			extern __declspec(dllimport) type
#	define jr_EXTERN_DEF(type)		__declspec(dllimport) type
#else
#	define jr_EXTERN(type)			extern type
#	define jr_EXTERN_DEF(type)		type
#endif
#endif
