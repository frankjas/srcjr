#ifndef		__jr_string_h__
#define		__jr_string_h__

#include "ezport.h"

#include <stdio.h>
#include <ctype.h>

extern void			jr_RemoveAllOfChar (
						char *		modifiable_string,
						jr_int		char_val_removed) ;

extern void			jr_RemoveContiguousDuplicateChar (
						char *	string_with_possible_duplicate_chars,	/*      mypath//tmp///dir */
						jr_int	char_to_be_eliminated					/* '/'  mypath/tmp/dir    */
					) ;

extern jr_int		jr_CountAllOfChar (
							const char *		search_string,
							jr_int				char_val_counted);

extern void			jr_ReplaceAllOfChar (
						char *		modifiable_string,
						jr_int		old_char_val,
						jr_int		new_char_val) ;

extern const char *	jr_EscapedStringFromControlChar (jr_int c) ;

extern const char *	jr_StringFromControlChar (
						jr_int		char_val) ;

extern void			jr_CopyControlCharsToEscapedChars (
						char *			modifiable_target_string,
						jr_int			target_array_bounds,
						const char *	source_string) ;

extern void			jr_ControlCharsToEscapedChars (
						char *		modifiable_string) ;

extern jr_int		jr_RemoveTrailingWhiteSpace (
						char *		modifiable_string) ;

extern char **		jr_extendpath (
						char **			jr_malloced_env_vector,
						const char *	pathname) ;

extern const char *	jr_PathTail (
						const char *			file_path);

extern const char *jr_GetTrailingPath (
						const char *				path_string,
						char *						trailing_path_buf,
						jr_int						trailing_path_buf_size);

extern void			jr_NormalizePath (
						char *					file_path);

extern void			jr_EscapedCharsToControlChars (
						char *			modifiable_string) ;

extern jr_int		jr_EndsWithOddBackslashes (
						const char *	string) ;

extern jr_int		jr_IsString (
							char *			data_ptr,
							jr_int			data_length);

extern jr_int		jr_StringIsAllDigits (
						const char *	string) ;

extern jr_int		jr_StringIsValidIdentifier (
						const char *	potential_identifier_string) ;

extern char *		jr_strnchr (
						const char *	str,
						jr_int			c,
						jr_int			len);

extern char *		jr_rstrchr (
						const char *	end_str,
						const char *	str,
						jr_int			c);

#ifdef	compiler_has_ansi_support
	extern jr_int		jr_ColumnScanf (
							const char *	input_str,
							const char *	control_str,
							...
						) ;
#else
	extern jr_int		jr_ColumnScanf () ;
#endif

extern void			jr_ConvertToUpperCase (
						char *		modifiable_string) ;

extern void			jr_ConvertToLowerCase (
						char *		modifiable_string) ;

extern void			jr_CopyAndConvertCase (
						char *			modifiable_target_string,
						jr_int			target_array_bounds,
						const char *	source,
						const char *	case_control_string) ;

extern jr_int		jr_StringIsAllUpper (
						const char *	string) ;

extern jr_int		jr_StringIsAllLower (
						const char *	string) ;

extern jr_int		jr_CountSubByteMatches (
						const void *	source_buf_arg,
						jr_int			source_len,
						const void *	pattern_arg,
						jr_int			patlen) ;


extern jr_int			jr_IsGlobMatch				PROTO((
							const char *			text,
							const char *			pattern,
							jr_int					match_slashes,
							jr_int					match_leading_dot
						)) ;

extern jr_int			jr_HasGlobPatternChars		PROTO((
							const char *			potential_pattern_string
						)) ;

extern jr_int			jr_GlobPatternOffset		PROTO((
							const char *			potential_pattern_string
						)) ;

/*
** 8-8-2011: alternate API using APR
*/

#define					jr_GLOB_MATCH_PATH_SEPARATOR		0x0001
#define					jr_GLOB_MATCH_PERIOD				0x0002
#define					jr_GLOB_NO_ESCAPES					0x0004
#define					jr_GLOB_IGNORE_CASE					0x0008

extern jr_int			jr_StringHasGlobChars(
							const char *			string);

extern jr_int			jr_StringHasGlobMatch(
							const char *			string,
							const char *			pattern,
							unsigned jr_int			flags);


extern const char *	jr_regex_strerror (
						jr_int						reg_errno);


#define				jr_REGEX_NO_FLAGS					0x0000
#define				jr_REGEX_POSIX_EXTENDED_FLAG		0x0001
#define				jr_REGEX_IGNORE_CASE_FLAG			0x0002

extern const char *	jr_RegExSub (
						const char *				source_string,
						const char *				pattern_string,
						const char *				replace_string,
						jr_int						flags,
						char *						error_buf);

extern jr_int		jr_RegExMatch (
						const char *				source_string,
						const char *				pattern_string,
						jr_int						flags,
						char *						error_buf);

extern char *		jr_ReplaceSubStringGlobally (
						const char *	source_buf_arg,
						const char *	old_pattern_arg,
						const char *	new_pattern_arg) ;

extern void *		jr_ReplaceSubByteMatches (
						const void *	source_buf_arg,
						jr_int			source_buf_len,
						const void *	old_pattern_arg,
						jr_int			old_pattern_len,
						const void *	new_pattern_arg,
						jr_int			new_pattern_len) ;

extern char *		jr_StringFgets (
						char *			modifiable_target_string,
						jr_int			target_array_bounds,
						const char *	source_string) ;

extern char *		jr_FirstNonWhiteSpacePtr (	/* was skip */
						const char *	search_string) ;

extern jr_int		jr_FirstNonWhiteSpaceIndex (
						const char *	search_string) ;

extern char *		jr_NextNonWhiteSpacePtr (
						const char *	search_string) ;
						/*
						** 1/24/07: Skip over non-white space followed by white-space
						*/

extern char *		jr_FirstWhiteSpacePtr (
						const char *	search_string) ;

extern jr_int		jr_FirstWhiteSpaceIndex (
						const char *	search_string) ;

extern jr_int		jr_SubStringIndex (
						const char *	search_string,
						const char *	pattern) ;

extern void			jr_StringUnQuote (
						char *			modifiable_string) ;

extern char *		jr_TTY_LettersFromDeviceName (
						char *	dev_name_string) ;

/******** URL Encoding/Decoding ********/

extern char *		jr_StringEncodeForURL (
						const char *				input_string);

extern char *		jr_StringDecodeFromURLWithFixReturn (
						const char *				input_string,
						jr_int						fix_return_newline);

#define				jr_StringDecodeFromURLAndFixReturn(input_string)				\
					jr_StringDecodeFromURLWithFixReturn (input_string, 1)
					/*
					 * Remove <CR> from <CR><LF> sequences
					 */

#define				jr_StringDecodeFromURL(input_string)				\
					jr_StringDecodeFromURLWithFixReturn (input_string, 0)

extern char *		jr_StringEncodeForQuotes (
						const char *				input_string,
						const char *				quotes_string);

#define				jr_StringDecode(input_string)				\
					jr_StringDecodeFromURL (input_string)


/******** URL Encoding/Decoding ********/

#define				jr_tolower(c) (isupper(c) ? (c) | 040 : (c))
#define				jr_toupper(c) (islower(c) ? (c) &(~040) : (c))


#define				jr_ForEachOccurenceInStr(str,pattern,pos,i)							\
					for (																\
						(pos) = jr_SubStringIndex(str,pattern);							\
						(pos)>=0;														\
						(pos) = (														\
							(i) = jr_SubStringIndex ((str)+(pos)+1, pattern)) < 0		\
							? (i)														\
							: (i)+(pos)+1												\
					)

#if 0
/*
** 1/24/07: Dangerous use of sscanf()
*/
#define				jr_ForEachWordInString(str,ptr,word,word_size)						\
					for (																\
						(ptr) = jr_FirstNonWhiteSpacePtr(str);							\
						*(ptr);															\
						(ptr) += strlen(word), (ptr) = jr_FirstNonWhiteSpacePtr(ptr)	\
					) if (sscanf(ptr,"%s", word)<1) break ; else

#define				jr_ForEachLineInBuffer(buf,line,ptr)								\
					for (																\
						(ptr)=(buf);													\
						*(ptr);															\
						(ptr) += strlen(line) + 1										\
					) if (sscanf (ptr, "%[^\n]", line) < 1) break; else
#endif

#define				jr_ForEachLinePtrInBuffer(buf,cp)									\
					for (																\
						(cp) = (buf);													\
						(cp) && *(cp);													\
						(cp) = strchr (cp, '\n'), (cp)									\
								? (cp) = jr_FirstNonWhiteSpacePtr(cp)					\
								: 0														\
					)


#endif
