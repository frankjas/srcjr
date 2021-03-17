#ifndef __esis_grove_hpp___
#define __esis_grove_hpp___

/******* ESIS Info ********/

extern "C" {
#	include "ezport.h"
#	include "jr/esis/lib.h"
};

#include "jr/esis/io.hpp"


class jr_ESIS_InputGroveClass : public jr_ESIS_InputClass {
public :
	jr_ESIS_TreeType	tree_info [1];

	void *				operator new		(size_t size, jr_ESIS_InputGroveClass *esis_info);

	void				undo				();
	void				destroy				();

	jr_int				element_begin		(
							char *			element_name,
							jr_int			keyword_number,
							char *			error_buf
						);

	jr_int				element_end			(
							char *			element_name,
							jr_int			keyword_number,
							char *			error_buf
						);

	jr_int				element_data		(char *data,			char *error_buf);

	void				set_is_conforming	(jr_int value);
};


extern void				jr_ESIS_InputGroveInit			(jr_ESIS_InputGroveClass *esis_grove);
extern void				jr_ESIS_InputGroveUndo			(jr_ESIS_InputGroveClass *esis_grove);

extern jr_ESIS_InputGroveClass *jr_ESIS_InputGroveCreate	(void);
extern void				jr_ESIS_InputGroveDestroy		(jr_ESIS_InputGroveClass *esis_grove);


#endif
