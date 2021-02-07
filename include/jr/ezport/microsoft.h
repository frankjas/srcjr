#ifndef __JR_EZPORT_MICROSOFT_H___
#define __JR_EZPORT_MICROSOFT_H___

/*
** GNU/ANSI Fixes
*/

#ifndef __attribute__
#	define __attribute__(arg)
#endif

#ifdef needs_winnt_snprintf
#ifndef snprintf
#	define snprintf				_snprintf
#endif
#endif

#ifdef needs_winnt_vsnprintf
#ifndef vsnprintf
#	define vsnprintf			_vsnprintf
#endif
#endif



#ifdef needs_winnt_defines_for_APR
#	define _CRT_SECURE_NO_DEPRECATE
#	ifndef WIN32
		/*
		** 1/25/07: WIN32 is defined by including <windows.h>
		** apr.h requires it to trigger the proper behavior.
		*/
#		define WIN32 
#	endif
#endif

/* 
 * Exclusions: define variables to exclude selected header files.
 * For complete explanation, see Rector & Newcomer, Win32 Programming, pp 25ff.
 *
 * These definitions also reduce the pch and improve compiling time.
 * All the programs in the book will still compile with these definitions.
 * You can also eliminate, for example, security with #define NOSECURITY.
 *
 * WIN32_LEAN_AND_MEAN has the largest impact, halving the pch file size.
 */

#ifdef reduce_winnt_compile

#define jr_UNDO_NOCTLMGR
/*
** 5-17-2011: can't include windows.h, winsock2.h, w/o CTLMGR,
** get syntax errors in prsht.h (property sheet)
*/

#define jr_UNDO_NOMSG
/*
** 5-17-2011: can't include windows.h, winsock2.h, w/o MSG,
** get syntax errors in oleidl.h
*/

#ifndef jr_UNDO_NOATOM
#	define NOATOM
#endif

#ifndef jr_UNDO_NOCLIPBOARD
#	define NOCLIPBOARD
#endif

#ifndef jr_UNDO_NOCOMM
#	define NOCOMM
#endif

#ifndef jr_UNDO_NOCTLMGR
#	define NOCTLMGR
#endif

#ifndef jr_UNDO_NOCOLOR
#	define NOCOLOR
#endif

#ifndef jr_UNDO_NODEFERWINDOWPOS
#	define NODEFERWINDOWPOS
#endif

#ifndef jr_UNDO_NODESKTOP
#	define NODESKTOP
#endif

#ifndef jr_UNDO_NODRAWTEXT
#	define NODRAWTEXT
#endif

#ifndef jr_UNDO_NOEXTAPI
#	define NOEXTAPI
#endif

#ifndef jr_UNDO_NOGDICAPMASKS
#	define NOGDICAPMASKS
#endif

#ifndef jr_UNDO_NOHELP
#	define NOHELP
#endif

#ifndef jr_UNDO_NOICONS
#	define NOICONS
#endif

#ifndef jr_UNDO_NOTIME
#	define NOTIME
#endif

#ifndef jr_UNDO_NOIMM
#	define NOIMM
#endif

#ifndef jr_UNDO_NOKANJI
#	define NOKANJI
#endif

#ifndef jr_UNDO_NOKERNEL
#	define NOKERNEL
#endif

#ifndef jr_UNDO_NOKEYSTATES
#	define NOKEYSTATES
#endif

#ifndef jr_UNDO_NOMCX
#	define NOMCX
#endif

#ifndef jr_UNDO_NOMEMMGR
#	define NOMEMMGR
#endif

#ifndef jr_UNDO_NOMENUS
#	define NOMENUS
#endif

#ifndef jr_UNDO_NOMETAFILE
#	define NOMETAFILE
#endif

#ifndef jr_UNDO_NOMSG
#	define NOMSG
#endif

#ifndef jr_UNDO_NONCMESSAGES
#	define NONCMESSAGES
#endif

#ifndef jr_UNDO_NOPROFILER
#	define NOPROFILER
#endif

#ifndef jr_UNDO_NORASTEROPS
#	define NORASTEROPS
#endif

#ifndef jr_UNDO_NORESOURCE
#	define NORESOURCE
#endif

#ifndef jr_UNDO_NOSCROLL
#	define NOSCROLL
#endif

#ifndef jr_UNDO_NOSERVICE
#	define NOSERVICE
#endif

#ifndef jr_UNDO_NOSHOWWINDOW
#	define NOSHOWWINDOW
#endif

#ifndef jr_UNDO_NOSOUND
#	define NOSOUND
#endif

#ifndef jr_UNDO_NOSYSCOMMANDS
#	define NOSYSCOMMANDS
#endif

#ifndef jr_UNDO_NOSYSMETRICS
#	define NOSYSMETRICS
#endif

#ifndef jr_UNDO_NOSYSPARAMS
#	define NOSYSPARAMS
#endif

#ifndef jr_UNDO_NOTEXTMETRIC
#	define NOTEXTMETRIC
#endif

#ifndef jr_UNDO_NOVIRTUALKEYCODES
#	define NOVIRTUALKEYCODES
#endif

#ifndef jr_UNDO_NOWH
#	define NOWH
#endif

#ifndef jr_UNDO_NOWINDOWSTATION
#	define NOWINDOWSTATION
#endif

#ifndef jr_UNDO_NOWINMESSAGES
#	define NOWINMESSAGES
#endif

#ifndef jr_UNDO_NOWINOFFSETS
#	define NOWINOFFSETS
#endif

#ifndef jr_UNDO_NOWINSTYLES
#	define NOWINSTYLES
#endif

#ifndef jr_UNDO_OEMRESOURCE
#	define OEMRESOURCE
#endif

#endif

/*
** 1/27/07: Don't include windows header files since the defininition TokenType, VOID, etc,
** in the ansi_c parser and other identifiers that cause conflicts.
*/


#endif
