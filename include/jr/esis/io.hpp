#ifndef _esis_io_hpp__
#define _esis_io_hpp__

extern "C" {
#	include "ezport.h"

#	include "jr/attribut.h"
#	include "jr/htable.h"

#	include "jr/esis/lib.h"
}

typedef struct {
	char *			name_str;
	jr_int			keyword_number;
} jr_ESIS_KeywordType;


class jr_ESIS_InputClass {
public :
	jr_HTable					keyword_table[1];

	jr_AttributeManagerType		attr_manager[1];
	jr_AttributeList			attr_list[1];
	
	jr_int						line_number;
	char *						file_name;

	jr_int						max_num_errors;

	unsigned					is_conforming		: 1;


	virtual void				add_attribute		(
									char *			name_str,
									char *			value_str
								);
	virtual void				set_line_number		(
									jr_int			line_number,
									char *			opt_file_name
								);


	virtual void				undo				()						= 0;
	virtual void				destroy				()						= 0;

	virtual jr_int				element_begin		(
									char *			element_name,
									jr_int			keyword_number,
									char *			error_buf
								) = 0;

	virtual jr_int				element_end			(
									char *			element_name,
									jr_int			keyword_number,
									char *			error_buf
								) = 0;

	virtual jr_int				element_data		(char *data,			char *error_buf)	= 0;

	virtual void				set_is_conforming	(jr_int value)			= 0;

	void *						operator new		(size_t size, jr_ESIS_InputClass *esis_input);
	virtual						~jr_ESIS_InputClass	() {};

};

extern void			jr_ESIS_InputInit				(jr_ESIS_InputClass *esis_info);
extern void			jr_ESIS_InputUndo				(jr_ESIS_InputClass *esis_info);

extern jr_ESIS_InputClass *jr_ESIS_InputCreate		();
extern void			jr_ESIS_InputDestroy			(jr_ESIS_InputClass *esis_info);

extern void			jr_ESIS_InputDefineKeyword		(
						jr_ESIS_InputClass *		esis_info,
						char *						name,
						jr_int						keyword_number
					);

extern jr_int		jr_ESIS_InputGetKeywordNumber	(
						jr_ESIS_InputClass *		esis_info,
						const char *				name
					);

inline jr_int		jr_ESIS_InputSetMaxNumErrors	(jr_ESIS_InputClass *esis_info, jr_int v)
{
	return esis_info->max_num_errors	= v;
}

inline jr_int		jr_ESIS_InputIsConforming		(jr_ESIS_InputClass *esis_info)
{
	return esis_info->is_conforming;
}


inline jr_int		jr_ESIS_InputLineNumber			(jr_ESIS_InputClass *esis_info)
{
	return esis_info->line_number;
}


inline char *		jr_ESIS_InputFileName			(jr_ESIS_InputClass *esis_info)
{
	return esis_info->file_name;
}

inline jr_AttributeManagerType *jr_ESIS_InputAttributeManager	(jr_ESIS_InputClass *esis_info)
{
	return esis_info->attr_manager;
}

inline const char *jr_ESIS_InputGetAttributeValue	(jr_ESIS_InputClass *esis_info, char *name)
{
	return jr_AttributeListGetValue (esis_info->attr_list, name);
}


extern jr_int		jr_ESIS_InputReadFromFileName	(
						jr_ESIS_InputClass *		esis_info,
						char *						rfn,
						char *						error_buf
					);

extern jr_int		jr_ESIS_InputReadFromFilePtr	(
						jr_ESIS_InputClass *		esis_info,
						FILE *						rfp,
						char *						rfn,
						char *						error_buf
					);


#define jr_ESIS_InputForEachAttributePtr(esis_info, attribute_ptr)		\
		jr_AttributeListForEachAttributePtr ((esis_info)->attr_list, attribute_ptr)

/******** Keyword ********/

extern void			jr_ESIS_KeywordInit				(
						jr_ESIS_KeywordType *		keyword_info,
						const char *				name_str,
						jr_int						keyword_number
					);

extern void			jr_ESIS_KeywordUndo				(
						jr_ESIS_KeywordType *		keyword_info
					);

extern jr_ESIS_KeywordType *jr_ESIS_KeywordCreate	(
						const char *				name_str,
						jr_int						keyword_number
					);

extern void			jr_ESIS_KeywordDestroy			(
						jr_ESIS_KeywordType *		keyword_info
					);

extern jr_int		jr_ESIS_KeywordHash				(
						const void *				void_arg_1
					);

extern jr_int		jr_ESIS_KeywordCmp				(
						const void *				void_arg_1,
						const void *				void_arg_2
					);
#endif
