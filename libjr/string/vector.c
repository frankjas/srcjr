#include "ezport.h"

#include <stdio.h>

#include "jr/vector.h"
#include "jr/malloc.h"


void *		jr_VectorCreateWithOneItem (zeroth_item_value)
	const void *	zeroth_item_value;
{
	void **		new_vector;

	new_vector = (void **) jr_malloc (2 * sizeof (void *));

	new_vector[0] = (void *) zeroth_item_value;
	new_vector[1] = 0;

	return ((void *)new_vector);
}

void *	jr_VectorCreateFromVectorAndItem(source_vector_arg, source_item)
			void *	source_vector_arg ;
			void *	source_item ;
{
	void **		source_vector = source_vector_arg ;
	jr_int		source_vector_size ;
	jr_int		target_vector_size ;
	void **		target_vector ;
	jr_int		i ;

	source_vector_size = jr_VectorLength(source_vector_arg) ;
	target_vector_size = source_vector_size + 1 ;
	target_vector = jr_malloc((target_vector_size + 1) * sizeof(void *)) ;
	
	for (i= 0 ; source_vector[i] ; i++) {
		target_vector[i] = source_vector[i] ;
	}
	target_vector[i] = source_item ;
	i++ ;

	target_vector[i] = 0 ;

	return((void *) target_vector) ;
}


void jr_VectorDestroy (vector_arg)
	const void *		vector_arg ;
{
	jr_VectorUndo (vector_arg);

	jr_free(vector_arg) ;
}

void jr_VectorUndo(vector_arg)
	const void *		vector_arg;
{ 
	const void **		vp ;

	for (vp = (void *)vector_arg; *vp; vp++) {
		jr_free (*vp) ;
	}
}


jr_int jr_VectorLength (vector_arg)
	const void *	vector_arg;
{
	const void **	vp		= (const void **) vector_arg ;
	jr_int			dimen	= 0;

	while ( *vp++ ) dimen++;
	return (dimen);
}

void *jr_VectorDup(vector_arg, savefunc)
	const void *	vector_arg ;
	void *			(*savefunc)() ;
{
	char **			source_vector = (char **) vector_arg ;
	char **			target_vector ;
	jr_int			vector_length = jr_VectorLength(source_vector) ;
	jr_int			i ;
	
	target_vector = (char **) jr_malloc((vector_length + 1) * sizeof(char *)) ;

	for (i=0; source_vector[i]; i++) {
		if (savefunc) {
			target_vector[i] = (*savefunc)(source_vector[i]) ;
		}
		else {
			target_vector[i] = source_vector[i] ;
		}
	}
	target_vector[i] = 0 ;

	return((void *)target_vector) ;
}

void jr_VectorConcatenate(target_vector_arg, source_vector_arg)
	void *		target_vector_arg ;
	void *		source_vector_arg ;
{
	void **		target_vector = target_vector_arg ;
	void **		source_vector = source_vector_arg ;
	jr_int		target_vector_size ;
	jr_int		i ;

	target_vector_size = jr_VectorLength(target_vector) ;

	for (i= 0 ; source_vector[i] ; i++) {
		target_vector[target_vector_size + i] = source_vector[i] ;
	}

	target_vector[target_vector_size + i] = 0 ;
}


jr_int jr_VectorFindMaximum(void_vector_arg, n, maxfunc)
	const void *		void_vector_arg ;
	jr_int				n ;
	jr_int				(*maxfunc)() ;
{
	void **			vector_arg = (void **) void_vector_arg ;
	jr_int			max_so_far = 0 ;
	jr_int			current_value ;
	jr_int			i ;

	for (i = 0 ; i < n; i++) {
		current_value = (*maxfunc)(vector_arg[i]) ;

		if (current_value > max_so_far) {
			max_so_far = current_value ;
		}
	}

	return(max_so_far) ;
}
