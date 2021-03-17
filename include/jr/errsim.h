#ifndef _jr_errsim_h___
#define _jr_errsim_h___

extern jr_int			jr_iferr_sim (
							jr_int						is_err,
							jr_int						is_orig_err,
							jr_int						negate_err,
							const char *				file_name,
							jr_int						line_number);

#ifdef jr_IFERR_ID

#	define				iferr(v) \
						iferr_macro (jr_IFERR_ID, __LINE__) (v,0,0, jr_IFERR_ID, __LINE__)

#	define				iferr_sim(v) \
						iferr_macro (jr_IFERR_ID, __LINE__) (v,1,0, jr_IFERR_ID, __LINE__)

#	define				ifnerr(v) \
						iferr_macro (jr_IFERR_ID, __LINE__) (v,0,1, jr_IFERR_ID, __LINE__)

#	define				ifnerr_sim(v) \
						iferr_macro (jr_IFERR_ID, __LINE__) (v,1,1, jr_IFERR_ID, __LINE__)


#	define				iferr_lic(v) \
						iferr_lic_macro (jr_IFERR_ID, __LINE__) (v,0,0, jr_IFERR_ID, __LINE__)

#	define				iferr_sim_lic(v) \
						iferr_lic_macro (jr_IFERR_ID, __LINE__) (v,1,0, jr_IFERR_ID, __LINE__)

#	define				ifnerr_lic(v) \
						iferr_lic_macro (jr_IFERR_ID, __LINE__) (v,0,1, jr_IFERR_ID, __LINE__)

#	define				ifnerr_sim_lic(v) \
						iferr_lic_macro (jr_IFERR_ID, __LINE__) (v,1,1, jr_IFERR_ID, __LINE__)

#else

#	define				iferr(v)							if (v)
#	define				iferr_sim(v)						if (v)
#	define				ifnerr(v)							if (v)
#	define				ifnerr_sim(v)						if (v)

#	define				iferr_lic(v)						if (v)
#	define				iferr_sim_lic(v)					if (v)
#	define				ifnerr_lic(v)						if (v)
#	define				ifnerr_sim_lic(v)					if (v)

#endif

#define					iferr_assert(v)						if (v)


#define					iferr_macro(file_id, line)			iferr_macro_2 (file_id, line)
#define					iferr_lic_macro(file_id, line)		iferr_lic_macro_2 (file_id, line)
/*
 * 7/26/2005: Need an extra layer of macro call to get ## to work as desired under gcc,
 * i.e. to get the arguments substituted before the ## happens.
 */

#ifdef jr_IFERR_USE_CUSTOM
#	define				iferr_macro_2(file_id, line)		iferr_ ## file_id ## line
#elif jr_IFERR_USE_CACHED
#	define				iferr_macro_2(file_id, line)		jr_iferr_cached
#else
#	define				iferr_macro_2(file_id, line)		jr_iferr
#endif


#ifdef jr_IFERR_USE_CUSTOM
#	define				iferr_lic_macro_2(file_id, line)	iferr_lic_ ## file_id ## line
#elif jr_IFERR_USE_CACHED
#	define				iferr_lic_macro_2(file_id, line)	jr_iferr_cached
#else
#	define				iferr_lic_macro_2(file_id, line)	jr_iferr
#endif


#define					jr_iferr(is_err, is_orig_err, negate_err, file_id, line)			\
						if (jr_iferr_sim (is_err, is_orig_err, negate_err, __FILE__, line))
							

#define					jr_iferr_cached(is_err, is_orig_err, negate_err, file_id, line)		\
						if (jr_iferr_var (file_id, line) >= 0								\
							? jr_iferr_var (file_id, line)									\
							: jr_iferr_var (file_id, line) = jr_iferr_sim (					\
								is_err, is_orig_err, negate_err, __FILE__, line				\
							)																\
						)
							
#define					jr_iferr_var(base, suffix)	jr_iferr_id (base, suffix)

#ifndef jr_iferr_id
#	define				jr_iferr_id(base, suffix)	base ## suffix
#endif


/******** Diagnostic Error Macros ********/

/*
** 1/19/07: Unused in code
*/

#define					jr_NUM_FAKE_ERRORS					1024
extern char				jr_HasFakeError[];


#define jr_set_fake_error(n)		(jr_check_fake_error_macro (n); jr_HasFakeError [n] = 1)

#ifdef jr_NO_FAKE_ERRORS
#	define jr_use_fake_error(n)		(0)
#else
#	define jr_use_fake_error(n)		(jr_HasFakeError [n])
#endif


#ifdef compiler_has_ansi_support
#	define jr_check_fake_error_macro(n)		jr_check_fake_error (n, __FILE__, __LINE__)
#else
#	define jr_check_fake_error_macro(n)		jr_check_fake_error (n, 0, 0)
#endif


extern void				jr_check_fake_error			PROTO ((
							jr_int					error_number,
							const char *			file_name,
							jr_int					line_number
						));

#endif
