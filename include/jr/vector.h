#ifndef __jr_vector_h___
#define __jr_vector_h___

#include "ezport.h"

#include <stdio.h>


extern char *		jr_getenv							PROTO((
						char **							string_vector,
						const char  *					name_string
					)) ;

extern char **		jr_setenv							PROTO((
						char **							jr_malloced_env_vector,
						const char *					name_string,
						const char *					replacement_rhs_string,
						char **							old_entry_holder_for_freeing
					)) ;

/******** Vectors ********/

extern void			jr_VectorPrintDeclaration			PROTO((
						FILE *							wfp,
						const char **					string_vector,
						const char *					variable_name
					)) ;

extern void			jr_VectorDestroy					PROTO((
						const void *					jr_malloced_items_and_vector
					)) ;

extern void			jr_VectorUndo						PROTO((
						const void *					jr_malloced_items_vector
					)) ;

extern void *		jr_VectorCreateWithOneItem			PROTO((
						const void *					zeroth_item_value
					)) ;

extern void *		jr_VectorCreateFromVectorAndItem		PROTO((
						void *							source_vector,
						void *							source_item
					)) ;
					/* items are copied into new vector */
					/* placed in new vector at end */

extern void *		jr_VectorDup						PROTO((
						const void *					arg,
						void *							(*savefunc)()
					)) ;

extern jr_int		jr_VectorLength						PROTO((
						const void *					vector
					)) ;

extern void			jr_VectorDisplayLikeLs				PROTO((
						FILE *							wfp,
						const char **					vector_arg,
						jr_int							n,
						jr_int							screen_width
					)) ;

extern jr_int		jr_VectorFindMaximum				PROTO((
						const void *					vector_arg,
						jr_int							n,
						jr_int							(*maxfunc)()
					)) ;

					/*
					 * The following may be true:
					 * "significant_separators" contains the characters which will
					 * break the line in to words,
					 *
					 * "skip_separators" will do so also, but multiple consecutive
					 * occurances will be skipped over (like whitespace)
					 *
					 * Leading "white_space" will be ignored.
					 * "quotes" will turn off the meaning of any enclosed separators.
					 */

extern jr_int		jr_VectorizeWithSkipSeparators	PROTO((
						const char *			str,
						const char *			significant_sep,
						const char *			skip_sep,
						char **					target_vector,
						jr_int   				target_array_bounds
					)) ;

extern const char **jr_VectorCreateWithSkipSeparators	PROTO ((
						const char *			str,
						const char *			significant_sep,
						const char *			skip_sep
					));

extern jr_int		jr_VectorizeWithSkipSeparatorsInPlace	PROTO((
						char *					modifiable_string,
						const char *			significant_sep,
						const char *			skip_sep,
						char **					target_vector,
						jr_int   				target_array_bounds
					)) ;

extern jr_int		jr_VectorizeIntoWords		PROTO((
						const char *			str,
						const char *			sep,
						char **					target_vector,
						jr_int   				target_array_bounds
					)) ;

extern jr_int		jr_VectorizeIntoWordsInPlace		PROTO((
						char *					modifiable_string,
						const char *			sep,
						char **					target_vector,
						jr_int   				target_array_bounds
					)) ;

extern const char **jr_VectorCreateWithWords	PROTO ((
						const char *			str,
						const char *			delimiters
					));

extern jr_int		jr_VectorizeWithQuotes		PROTO ((
						const char *			str,
						const char *			delimiters,
						const char *			white_space,
						const char *			quotes,
						char **					vector,
						jr_int					vector_size
					));

extern const char **jr_VectorCreateWithQuotes	PROTO ((
						const char *			str,
						const char *			delimiters,
						const char *			white_space,
						const char *			quotes 
					));

extern void			jr_VectorConcatenate			PROTO((
							void *	target_vector,	/* target vector space had better be big enough */
							void *	source_vector	/* items are copied into target vector */
					)) ;


#define				jr_VectorForEachIndex(vector, i)		\
					for ((i)=0;  (vector)[i]  !=  0;  (i)++)

#define				jr_VectorElement(vector, i)				((vector)[i])


extern char *		jr_NewStringFromDollarVectorSubstitutions		PROTO((
							const char *		old_string,
							char **				env_style_vector
					)) ;

extern char **		jr_VectorReadDeclaration	PROTO((
							FILE *				rfp,
							char *				variable_name,
							char *				errbuf
					)) ;


extern const char **jr_VectorCreateFromCshLikeNameExpansion(
						const char *	pathname_arg,
						jr_int			match_slashes,
						jr_int			match_leading_dot);

extern const char **jr_VectorCreateFromDirectoryNameAndPattern (
						const char *	directory_name_string,
						const char *	pattern_string,
						jr_int			match_slashes,
						jr_int			match_leading_dot);

#endif
