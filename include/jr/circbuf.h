#ifndef __CIRCBUF_HEADER__
#define __CIRCBUF_HEADER__

#include "ezport.h"

typedef struct {
	char *element_array;
	char *head;
	char *tail;

	jr_int size;
	jr_int num_elements;

	jr_int max_size;
	jr_int element_size;
} jr_CB_CircularBuffer;

extern void jr_CB_CircularBufferInit PROTO((jr_CB_CircularBuffer *buffer, 
	jr_int max_elements, jr_int element_size));

extern void jr_CB_CircularBufferUndo PROTO((jr_CB_CircularBuffer *buffer)); 

extern void jr_CB_CircularBufferReset PROTO((jr_CB_CircularBuffer *buffer));

extern void jr_CB_GetTail PROTO((jr_CB_CircularBuffer *buffer, void *element));
extern void jr_CB_GetHead PROTO((jr_CB_CircularBuffer *buffer, void *element));

extern void *jr_CB_SetNewTail PROTO((jr_CB_CircularBuffer *buffer, 
	void *element, void *bumped_element));

extern void *jr_CB_SetNewHead PROTO((jr_CB_CircularBuffer *buffer, 
	void *element, void *bumped_element));

extern void *jr_CB_ElementPtr PROTO((jr_CB_CircularBuffer *buffer, jr_int i));
extern void Getjr_CB_ElementPtr PROTO((jr_CB_CircularBuffer *buffer, jr_int i, void *element));

#define jr_CB_CircularBufferSize(buffer)	((buffer)->num_elements)

/* if the buffer is full, this doesn't loop at all
#define jr_CB_ForEachElementPtr(buffer, ptr)						\
	for (	(ptr) = (void *) (buffer)->head;						\
			(void *) (ptr) != (void *) (buffer)->tail;				\
			(ptr) = (void*) ((char *) (ptr) + (buffer)->element_size),					\
			(char *) (ptr) == (buffer)->element_array + (buffer)->max_size ? \
			(ptr) = (void *) (buffer)->element_array : 0)
*/

#define jr_CB_ForEachElementIndex(buffer, i) \
	for ((i)=0; (i) < (buffer)->num_elements; (i)++)

#endif
