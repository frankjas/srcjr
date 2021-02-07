#ifndef __BINSET_HEADER__
#define __BINSET_HEADER__

#include "ezport.h"

#include <stdio.h>


typedef struct {
	jr_int				num_bits;
	unsigned jr_int *	int_array;
} jr_BitSet;

/*
 * 7/23/2005: can't use small in array local to the struct
 * since then the object can't be moved.
 */

extern jr_BitSet *	jr_BitSetCreate			PROTO ((jr_int num_bits));
extern void			jr_BitSetInit			PROTO ((jr_BitSet *bset, jr_int num_bits));
extern void			jr_BitSetDestroy		PROTO ((jr_BitSet *bset));
extern void			jr_BitSetUndo			PROTO ((jr_BitSet *bset));
extern jr_int		jr_BitSetSize			PROTO ((jr_BitSet *bset));
extern jr_int		jr_BitSetIsEmpty		PROTO ((jr_BitSet *bset));
extern jr_int		jr_BitSetIsFull			PROTO ((jr_BitSet *bset));
extern void			jr_BitSetEmpty			PROTO ((jr_BitSet *bset));
extern void			jr_BitSetAddAll			PROTO ((jr_BitSet *bset));
extern void			jr_BitSetAddSet			PROTO ((jr_BitSet *bsetb, jr_BitSet *bseta));
extern void			jr_BitSetUnion			PROTO ((
						jr_BitSet *			bsetc,
						jr_BitSet *			bseta,
						jr_BitSet *			bsetb
					));

extern void			jr_BitSetChangeNumBits	PROTO ((jr_BitSet *bset, jr_int new_num_bits));

extern void			jr_BitSetPrint			PROTO ((jr_BitSet *bset, FILE *wfp));


#define				jr_BitSetNumBits(bset)	((bset)->num_bits)


#define				jr_BitSetElementInt(bset, b)							\
					((bset)->int_array[((unsigned) (b)) >> jr_INT_BIT_SIZE_POWER])

#define				jr_BitSetElementMask(bset, b)							\
					(1 << ((b) & jr_INT_BIT_SIZE_MASK))


#define				jr_BitSetHasElement(bset, b)							\
					((jr_BitSetElementInt (bset, b)  &  jr_BitSetElementMask (bset, b)) != 0)

#define				jr_BitSetAddElement(bset, b)							\
					((void) (jr_BitSetElementInt (bset, b)  |=  jr_BitSetElementMask (bset, b)))

#define				jr_BitSetDeleteElement(bset, b)							\
					((void) (jr_BitSetElementInt (bset, b)  &=  ~jr_BitSetElementMask (bset, b)))


#define				jr_BitSetForEachPossibleElement(bset, b)				\
					for ((b) = 0;  (b) < (bset)->num_bits;  (b)++)
	
#define				jr_BitSetForEachElement(bset, b)						\
					jr_BitSetForEachPossibleElement (bset, b) if (jr_BitSetHasElement(bset, b))

#define				jr_BitSetForEachElementSparse(bset, b)					\
					for (	(b) = 0;										\
							(b) < (bset)->num_bits);						\
							jr_BitSetElementInt (bset, b)					\
								? (b)++										\
								: (	(b) += jr_INT_NUM_BITS,					\
									(b) -= (b) & jr_INT_BIT_SIZE_MASK)		\
						) if (jr_BitSetHasElement (bset, b))

#define				jr_BitSetIsSparse(bset, num_elements)					\
					(jr_BitSetNumBits(bset) / (num_elements)  >= 32)

/*
 * 7/21/2005: Normal loop: 3 accesses, 1 compare, 1 increment
 *            Sparse loop: 3 accesses, 1 compare, 1 increment, 1 shift, 1 access, 1 test
 *
 * Rounding in favor of a straight loop: 1 compare, 1 increment
 *                               sparse: 1 compare, 1 increment, 1 shift, 1 access, 1 test
 *
 * Total cost: 5N versus 2N.
 * Question: how many skips in a sparse loop will it take to make them even?
 * In other words, for what S is 5N - 64S < 2N (use 64 since a skip saves 32 increments, compares)?
 * 5N - 2N < 64S
 * 3N < 64S
 * (3/64) N < S
 *
 * Need to skip 3 out of 64 ints.
 *
 * Conclusion: the above is a pretty good approximation.
 */

#endif
