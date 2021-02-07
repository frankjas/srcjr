#ifndef _jr_misc_h__
#define _jr_misc_h__

#include "ezport.h"

#include <stdio.h>

#define jr_INT_NULL		jr_INT_MIN
#define jr_SHRT_NULL	jr_SHRT_MIN
#define jr_PI			(3.14159265358979323846L)



#define jr_abs(x)		(x < 0 ? -x : x)
#define jr_max(x,y)		((x)>(y)?(x):(y))
#define jr_min(x,y)		((x)<(y)?(x):(y))

#define jr_array_num_elements(array_name)									\
		(sizeof (array_name) / sizeof (array_name[0]))

#define jr_word_align_up(s)													\
		((s) & jr_INT_NUM_BYTES_MASK										\
			? ((s) + jr_INT_NUM_BYTES) & ~jr_INT_NUM_BYTES_MASK				\
			: (s)															\
		)

#define jr_word_align_down(s)												\
		(((jr_int)(s)) & ~jr_INT_NUM_BYTES_MASK)


extern unsigned jr_int	jr_next2power			PROTO ((jr_int number));
extern jr_int         	jr_is2power				PROTO ((jr_int number));
extern unsigned jr_int  jr_get2power			PROTO ((jr_int power));
extern jr_int			jr_log_of_2_power		PROTO ((jr_int number));


#define jr_HasFlag(var,mask)	((var) &   (mask))
#define jr_SetFlag(var,mask)	((var) |=  (mask))
#define jr_UnSetFlag(var,mask)	((var) &= ~(mask))


#define jr_UIntMaskedValue(l,  mask)	((l) & (mask))

#define jr_UIntMaskedAsgn(lp, mask, v)	(*(lp) = (*(lp) & ~(mask))  |  ((v) & (mask)))



/******** Compare And Hash Functions ********/

typedef jr_int (*jr_CmpFnType)	PROTO ((const void *, const void *));
typedef jr_int (*jr_HashFnType)	PROTO ((const void *));

extern jr_int	jr_rev_strcmp				PROTO ((const char *s1, const char *s2));
extern jr_int	jr_leading_strcmp			PROTO ((const char *s1, const char * s2));
extern jr_int	jr_sub_strcmp				PROTO ((const char *big_str, const char *small_str));
extern jr_int	jr_strrwordcasecmp			PROTO ((const void *void_arg_1, const void *void_arg_2));
				/*
				 * Compares on the last word of the string, ignoring case.
				 */


extern jr_int	jr_strpcmp					PROTO ((const void *, const void *)) ;
extern jr_int	jr_stricmp					PROTO ((const void *, const void *)) ;
extern jr_int	jr_trailing_filename_pcmp	PROTO ((const char **, const char **)) ;

extern jr_int	jr_opt_strcmp				PROTO ((const char *, const char *));
extern jr_int	jr_opt_strcasecmp			PROTO ((const char *, const char *));

extern jr_int	jr_canon_strcmp				PROTO ((const char *, const char *));
extern jr_int	jr_opt_canon_strcmp			PROTO ((const char *, const char *));
extern jr_int	jr_canon_is_prefix			PROTO ((const char *, const char *));

extern jr_int	jr_ptrcmp					PROTO ((const void *, const void *)) ;
extern jr_int	jr_ptrhash					PROTO ((const void *));

extern jr_int	jr_ptrptrcmp				PROTO ((const void **, const void **)) ;

extern jr_int	jr_intcmp					PROTO ((jr_int, jr_int)) ;
extern jr_int	jr_inthash					PROTO ((jr_int));

extern jr_int	jr_rintcmp					PROTO ((jr_int, jr_int)) ;
extern jr_int	jr_rintpcmp					PROTO ((const jr_int *, const jr_int *)) ;

extern jr_int	jr_intpcmp					PROTO ((const void *, const void *)) ;
extern jr_int	jr_intphash					PROTO ((const void *));

extern jr_int	jr_intstrcmp				PROTO ((const jr_int *, const jr_int *));
extern jr_int	jr_intstrhash				PROTO ((const jr_int *));
extern jr_int	jr_intstrlen				PROTO ((const jr_int *));
extern jr_int	jr_intstrbytes				PROTO ((const jr_int *));
extern jr_int	jr_intstrlenhash			PROTO ((const jr_int *, jr_int));

extern jr_int	jr_shortstrcmp				PROTO ((const short *, const short *));
extern jr_int	jr_shortstrhash				PROTO ((const short *));
extern jr_int	jr_shortstrbytes			PROTO ((const short *));
extern jr_int	jr_shortstrlenhash			PROTO ((const short *, jr_int));

extern jr_int	jr_ushort_add_max (
					unsigned jr_short *			ushort_ref,
					jr_int						addend);

extern jr_int	jr_posintcmp				PROTO ((jr_int, jr_int)) ;
						/*
						 * non-zero integers compared, zero > all 
						 */

extern jr_int	jr_posintpcmp				PROTO ((const jr_int *, const jr_int *)) ;
						/*
						 * non-zero integers compared, zero > all 
						 */

extern jr_int	jr_always_true_cmp			() ;
extern jr_int	jr_always_false_cmp			() ;


extern jr_int	jr_strhash				PROTO ((const void *)) ;
extern jr_int	jr_strhash_pjw			PROTO ((const void *));
extern jr_int	jr_strcasehash_pjw		PROTO ((const void *));
extern jr_int	jr_strlenhash			PROTO ((const void *, jr_int));



/******** Other Functions ********/

extern void				jr_coredump				PROTO ((void));
extern void				jr_debugger_trap		PROTO ((void));

extern char *			jr_MakeDiagFileName		PROTO ((
							char *				diag_file_buf,
							jr_int				diag_file_buf_size,
							const char *		file_name
						));

extern char				jr_DiagFileDirectory[256];

#define					jr_SetDiagFileDirectory(dir_path)				\
						strncpy (jr_DiagFileDirectory, dir_path, sizeof (jr_DiagFileDirectory) - 1)
						/*
						 * Has a null, since globals start out all zero
						 */

#define					jr_HasDiagFileDirectory()						\
						(jr_DiagFileDirectory[0])

#define					jr_GetDiagFileDirectory()						\
						(jr_DiagFileDirectory)


extern void				jr_freeref				PROTO ((char **ptr_to_be_derefd_and_freed)) ;

extern void				jr_memswitch			PROTO ((void *p1, void *p2, jr_int nbytes));
extern jr_int			jr_memisset				PROTO ((void *p1, jr_int value, jr_int nbytes));
extern void				jr_memscoot				PROTO ((
							void *				buffer,
							jr_int				buffer_size,
							jr_int				hole_offset,
							jr_int				hole_size
						));


extern void *			jr_BinarySearchInsertPtr			PROTO((
							const void *	base,
							jr_int			num_items,
							jr_int			sizeof_each_item,
							jr_int			(*compf)(),
							const void *	search_key
						)) ;

extern void *			jr_BinarySearch						PROTO((
							const void *	base,
							jr_int			num_items,
							jr_int			sizeof_each_item,
							jr_int			(*compf)(),
							const void *	key_or_pattern
						)) ;

extern void *			jr_LinearSearch						PROTO((
							const void *	base,
							jr_int			num_items,
							jr_int			sizeof_each_item,
							jr_int			(*compf)(),
							const void *	key_or_pattern
						)) ;

extern double			jr_StringExpressionToDouble	PROTO((
							const char *			string
						)) ;


extern unsigned long	jr_CRC_seeded_chksum	PROTO((
							unsigned char *		p,
							jr_int				len,
							short				seed
						)) ;
extern unsigned long	jr_CRC_chksum			PROTO((
							void *				byte_string_arg,
							jr_int				length
						)) ;


extern jr_int			jr_EnvTabStops			PROTO ((void));


extern void *			jr_heap_base(void);

extern jr_int			jr_backtrace			PROTO ((
							void **				pc_array,
							jr_int				pc_array_size
						));

extern jr_int			jr_random_bytes (
							void *						bytes_ptr,
							jr_int						bytes_length,
							char *						opt_error_buf);

extern jr_int			jr_base64_encode (
							char *				encoded_dest,
							const char *		plain_src,
							jr_int				plain_src_length);

extern jr_int			jr_base64_encode_binary (
							char *					encoded_dest,
							const unsigned char *	plain_src,
							jr_int					plain_src_length);

/******** PostScript ********/

extern jr_int		jr_PostScriptLineGetPageNumber		PROTO ((
						char *							lbuf
					));

extern char *		jr_PostScriptLineGetShowText		PROTO ((
						char *							lbuf,
						char *							text
					));

typedef struct {
	float lower_left_x ;
	float lower_left_y ;
	float upper_right_x ;
	float upper_right_y ;
	float range_x ;
	float range_y ;
} jr_PS_BoundingBoxStruct ;

typedef struct {
	float		trans_x ;
	float		trans_y ;
	float		scale_x ;
	float		scale_y ;
	float		rotation ;

	jr_int		justify ;

	float		vert_space ;
	float		page_width;
	float		margin_width;
} jr_PS_TransformStruct ;


extern jr_int		jr_PostScriptFileGetBoundingBox		PROTO ((
						FILE *							rfp,
						jr_PS_BoundingBoxStruct *		box_ptr
					));

extern void			jr_PostScriptPrintTransform			PROTO ((
						jr_PS_TransformStruct *			trans_info,
						jr_PS_BoundingBoxStruct *		box_ptr,
						FILE *							wfp,
						char *							roff_ps_prefix
					));

extern void			jr_PS_TransformInit					PROTO ((
						jr_PS_TransformStruct *			trans_info,
						float							trans_x,
						float							trans_y,
						float							scale_x,
						float							scale_y,
						float							rotation,

						jr_int							justify,
						float							vert_space,
						float							page_width,
						float							margin_width
					));

#endif
