#include <stdio.h>
#include <string.h>

#include "jr/misc.h"
#include "jr/string.h"
#include "jr/malloc.h"
#include "jr/trace.h"

#include "jr/attribut.h"

void main(argc,argv)
	jr_int argc ;
	char **argv ;
{
	char linebuf[1024] ;
	char namebuf[128] ;
	char valuebuf[128] ;
	jr_AttributeManagerType		attribute_manager[1] ;
	jr_AttributeList			attribute_list[1] ;

	jr_set_trace(jr_malloc_stats) ;
	jr_set_trace(jr_malloc_calls) ;

	jr_AttributeManagerInit(attribute_manager) ;
	jr_AttributeListInit(attribute_list, attribute_manager) ;

	fprintf(stderr, "Enter attribute pair: ") ;
	while (fgets(linebuf, sizeof(linebuf), stdin)) {
		sscanf(linebuf, "%s %s", namebuf, valuebuf) ;
		jr_AttributeListAddAttribute(attribute_list, namebuf, valuebuf) ;
	
		fprintf(stderr, "Enter attribute pair: ") ;
	}
	
	fprintf(stderr, "\n\n") ;
	{
		jr_AttributeType *	attribute_ptr ;

		jr_AttributeListForEachAttributePtr(attribute_list, attribute_ptr) {
			fprintf(stdout, "%s (%#010x) : %s (%#010x)\n",
				attribute_ptr->name_string,		(unsigned) attribute_ptr->name_string,
				attribute_ptr->value_string,	(unsigned) attribute_ptr->value_string
			) ;
		}
	}

	jr_AttributeListUndo(attribute_list) ;
	jr_AttributeManagerUndo(attribute_manager) ;

	fprintf(stderr, "\n\n") ;

	if (jr_do_trace(jr_malloc_stats)) {
		jr_malloc_stats(stdout, "After Free") ;
	}
	if (jr_do_trace(jr_malloc_calls)) {
		jr_malloc_dump() ;
	}

	exit(0) ;
}
