#include <stdlib.h>

#include "xadmin.h"

struct array *
array_create(uint32_t n, size_t size)
{
	struct array *a;

	if (!(n != 0 && size != 0)) {
		fprintf(stderr, "ERROR: array_create n[%d] size[%lu]\n", n, size);
		return NULL;
	}

	a = malloc(sizeof(*a));
	if (a == NULL) {
		return NULL;
	}

	a->elem = malloc(n * size);
	if (a->elem == NULL) {
		free(a);
		return NULL;
	}

	a->nelem = 0;
	a->size = size;
	a->nalloc = n;

	return a;
}

void
array_destroy(struct array *a)
{
	array_deinit(a);
	free(a);
}

rstatus_t
array_init(struct array *a, uint32_t n, size_t size)
{
	if (!(n != 0 && size != 0)) {
		fprintf(stderr, "ERROR: array_init n[%d] size[%lu]\n", n, size);
		return LD_ERROR;
	}

	a->elem = malloc(n * size);
	if (a->elem == NULL) {
		return LD_ENOMEM;
	}

	a->nelem = 0;
	a->size = size;
	a->nalloc = n;

	return LD_OK;
}

void
array_deinit(struct array *a)
{
	if (!(a->nelem == 0)) {
		fprintf(stderr, "ERROR: array_deint a->nelem[%d]\n", a->nelem);
		return ;
	}

	if (a->elem != NULL) {
		free(a->elem);
	}
}

uint32_t
array_idx(struct array *a, void *elem)
{
	uint8_t *p, *q;
	uint32_t off, idx;

	if (!(elem >= a->elem)) {
		fprintf(stderr, "ERROR: array_idx elem[%p] >= a->elem[%p]\n", elem, a->elem);
		return LD_ERROR;
	}

	p = a->elem;
	q = elem;
	off = (uint32_t)(q - p);

	if (!(off % (uint32_t)a->size == 0)) {
		fprintf(stderr, "ERROR: array_idx off[%d] %% a->size[%lu] == 0\n", off, a->size);
		return LD_ERROR;
	}

	idx = off / (uint32_t)a->size;

	return idx;
}

void *
array_push(struct array *a)
{
	void *elem, *new;
	size_t size;

	if (a->nelem == a->nalloc) {

		/* the array is full; allocate new array */
		size = a->size * a->nalloc;
		new = realloc(a->elem, 2 * size);
		if (new == NULL) {
			return NULL;
		}

		a->elem = new;
		a->nalloc *= 2;
	}

	elem = (uint8_t *)a->elem + a->size * a->nelem;
	a->nelem++;

	return elem;
}

void *
array_pop(struct array *a)
{
	void *elem;

	if (!(a->nelem != 0)) {
		fprintf(stderr, "ERROR: array_pop a->nelem[%d]\n", a->nelem);
		return NULL;
	}

	a->nelem--;
	elem = (uint8_t *)a->elem + a->size * a->nelem;

	return elem;
}

void *
array_get(struct array *a, uint32_t idx)
{
	void *elem;

	if (!(a->nelem != 0)) {
		fprintf(stderr, "ERROR: array_pop a->nelem[%d]\n", a->nelem);
		return NULL;
	}

	if (!(idx < a->nelem)) {
		fprintf(stderr, "ERROR: array_idx idx[%d] < a->nelem[%d]\n", idx, a->nelem);
		return NULL;
	}

	elem = (uint8_t *)a->elem + (a->size * idx);

	return elem;
}

void *
array_top(struct array *a)
{
	if (!(a->nelem != 0)) {
		fprintf(stderr, "ERROR: array_pop a->nelem[%d]\n", a->nelem);
		return NULL;
	}

	return array_get(a, a->nelem - 1);
}

void
array_swap(struct array *a, struct array *b)
{
	struct array tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/*
 * Sort nelem elements of the array in ascending order based on the
 * compare comparator.
 */
void
array_sort(struct array *a, array_compare_t compare)
{
	if (!(a->nelem != 0)) {
		fprintf(stderr, "ERROR: array_pop a->nelem[%d]\n", a->nelem);
		return ;
	}

	qsort(a->elem, a->nelem, a->size, compare);
}

/*
 * Calls the func once for each element in the array as long as func returns
 * success. On failure short-circuits and returns the error status.
 */
rstatus_t
array_each(struct array *a, array_each_t func, void *data)
{
	uint32_t i, nelem;

	if (!(array_n(a) != 0)) {
		fprintf(stderr, "ERROR: array_each array_n(a)[%d] != 0\n", array_n(a));
		return LD_ERROR;
	}

	if (!(func != 0)) {
		fprintf(stderr, "ERROR: array_each func[%p]\n", func);
		return LD_ERROR;
	}

	for (i = 0, nelem = array_n(a); i < nelem; i++) {
		void *elem = array_get(a, i);
		rstatus_t status;

		status = func(elem, data);
		if (status != LD_OK) {
			return status;
		}
	}

	return LD_OK;
}
