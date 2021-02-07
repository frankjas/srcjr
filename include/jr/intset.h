#ifndef _intseth
#define _intseth

#include "ezport.h"

typedef struct {
	jr_int *array;
	jr_int next_index;
	jr_int max_size;
} jr_IntSet;

extern jr_IntSet    *jr_IntSetCreate PROTO((void)) ;
extern void    jr_IntSetInit PROTO((jr_IntSet *iset)) ;
extern jr_IntSet    *jr_IntSetCreateFromIntStr PROTO((jr_int *ia)) ;
extern jr_int       jr_IntSetCmp PROTO((jr_IntSet *iseta, jr_IntSet *isetb)) ;
extern jr_int       jr_IntSetFind PROTO((jr_IntSet *iset, jr_int n)) ;
extern jr_int       *jr_IntSetToIntStr PROTO((jr_IntSet *iset)) ;
extern void       jr_IntSetAdd PROTO((jr_IntSet *iset, jr_int n)) ;
extern void       jr_IntSetAddMax PROTO((jr_IntSet *iset, jr_int n)) ;
extern void       jr_IntSetAddSet PROTO((jr_IntSet *iseta, jr_IntSet *isetb)) ;
extern void       jr_IntSetDelete PROTO((jr_IntSet *iset, jr_int n)) ;
extern void       jr_IntSetDestroy PROTO((jr_IntSet *iset)) ;
extern void       jr_IntSetUndo PROTO((jr_IntSet *iset)) ;
extern jr_int        jr_IntSetFindIndex PROTO((jr_IntSet *iset, jr_int n)) ;

extern void       jr_IntSetIncreaseSize PROTO((jr_IntSet *iset, jr_int minelements)) ;

extern void       jr_IntSetUnion PROTO((
	jr_IntSet *isetc, jr_IntSet *iseta, jr_IntSet *isetb
)) ;
extern void       jr_IntSetIntersection PROTO((
jr_IntSet *isetc, jr_IntSet *iseta, jr_IntSet *isetb
)) ;


#define jr_IntSetSize(iset)	((iset)->next_index)
#define jr_IntSetArray(iset)	((iset)->array)
#define jr_IntSetElement(iset, i)	((iset)->array[i])
#define jr_IntSetDeleteMax(iset) ((iset)->next_index--)
#define jr_IntSetForEachElement(iset, i, n)	for (i=0, n=jr_IntSetElement(iset, 0); i < (iset)->next_index; i++, n=jr_IntSetElement(iset, i))
#define jr_IntSetEmpty(iset)	((iset)->next_index = 0)
#define jr_IntSetIsEmpty(iset)	((iset)->next_index == 0)

/******** Local functions and macros *******/

#				endif
