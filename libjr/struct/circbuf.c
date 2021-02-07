#include "ezport.h"

#include <stdio.h>
#include <string.h>

#include "jr/malloc.h"

#include "jr/circbuf.h"


void jr_CB_CircularBufferInit (buffer, max_elements, element_size)
	jr_CB_CircularBuffer *buffer;
	jr_int max_elements;
	jr_int element_size;
{
	memset (buffer, 0, sizeof (*buffer));

	buffer->element_array = jr_malloc (max_elements * element_size);

	buffer->max_size = max_elements * element_size;
	buffer->element_size = element_size;
	buffer->head = buffer->element_array;
	buffer->tail = buffer->element_array;
	buffer->size = 0;
	buffer->num_elements = 0;
}

void jr_CB_CircularBufferUndo (buffer)
	jr_CB_CircularBuffer *buffer;
{
	jr_free (buffer->element_array);
}

void jr_CB_CircularBufferReset (buffer)
	jr_CB_CircularBuffer *buffer;
{
	jr_int i;
	char *element;

	element = buffer->head;

	for (i=0; i < buffer->num_elements; 
			  i++, element += buffer->element_size) {

		if (element == buffer->element_array + buffer->max_size) {
			element = buffer->element_array;
		}
	}

	buffer->head = buffer->element_array;
	buffer->tail = buffer->element_array;
	buffer->size = 0;
	buffer->num_elements = 0;
}

void *jr_CB_SetNewTail (buffer, element, bumped_element)
	jr_CB_CircularBuffer *buffer;
	void *element;
	void *bumped_element;
{
	void *return_value;

	if (buffer->size == buffer->max_size) {
		/* make room for the new entry
		 */
		if (bumped_element) {
			memcpy (bumped_element, buffer->head, buffer->element_size);
		}

		buffer->head += buffer->element_size;

		if (buffer->head == buffer->element_array + buffer->max_size) {
			buffer->head = buffer->element_array;
		}
		return_value = bumped_element;
	}
	else {
		buffer->size += buffer->element_size;
		buffer->num_elements ++;
		return_value = 0;
	}

	memcpy (buffer->tail, element, buffer->element_size);
	buffer->tail += buffer->element_size;

	if (buffer->tail == buffer->element_array + buffer->max_size) {
		buffer->tail = buffer->element_array;
	}
	return (return_value);
}

void *jr_CB_SetNewHead (buffer, element, bumped_element)
	jr_CB_CircularBuffer *buffer;
	void *element;
	void *bumped_element;
{
	void *return_value;

	if (buffer->size == buffer->max_size) {
		/* make room for the new entry
		 */
		buffer->tail -= buffer->element_size;

		if (buffer->tail < buffer->element_array) {
			buffer->tail = buffer->element_array + buffer->max_size;
		}

		if (bumped_element) {
			memcpy (bumped_element, buffer->tail, buffer->element_size);
		}
		return_value = bumped_element;
	}
	else {
		buffer->size += buffer->element_size;
		buffer->num_elements ++;
		return_value = 0;
	}

	buffer->head -= buffer->element_size;

	if (buffer->head < buffer->element_array) {
		buffer->head = buffer->element_array + 
					   buffer->max_size - buffer->element_size;
	}

	memcpy (buffer->head, element, buffer->element_size);
	return (return_value);
}

void jr_CB_GetHead (buffer, element)
	jr_CB_CircularBuffer *buffer;
	void *element;
{
	buffer->size -= buffer->element_size;
	buffer->num_elements --;

	if (element) {
		memcpy (element, buffer->head, buffer->element_size);
	}

	buffer->head += buffer->element_size;

	if (buffer->head == buffer->element_array + buffer->max_size) {
		buffer->head = buffer->element_array;
	}
}

void jr_CB_GetTail (buffer, element)
	jr_CB_CircularBuffer *buffer;
	void *element;
{
	buffer->size -= buffer->element_size;
	buffer->num_elements --;

	buffer->tail -= buffer->element_size;

	if (buffer->tail < buffer->element_array) {
		buffer->tail = buffer->element_array + buffer->max_size - 
					   buffer->element_size;
	}

	if (element) {
		memcpy (element, buffer->tail, buffer->element_size);
	}
}

void *jr_CB_ElementPtr (buffer, i)
	jr_CB_CircularBuffer *buffer;
	jr_int i;
{
	i *= buffer->element_size;
	i += buffer->head - buffer->element_array;

	if (i >= buffer->max_size) {
		i -= buffer->max_size;
	}

	return (buffer->element_array + i);
}

void Getjr_CB_ElementPtr (buffer, i, element)
	jr_CB_CircularBuffer *buffer;
	jr_int i;
	void *element;
{
	i *= buffer->element_size;
	i += buffer->head - buffer->element_array;

	if (i >= buffer->max_size) {
		i -= buffer->max_size;
	}

	memcpy (element, buffer->element_array + i, buffer->element_size);
}

