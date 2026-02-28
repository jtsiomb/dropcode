/* dynarr - dynamic resizable C array data structure
 * author: John Tsiombikas <nuclear@member.fsf.org>
 * license: public domain
 */
#ifndef DYNARR_H_
#define DYNARR_H_

#include <stdio.h>
#include <stdlib.h>

/* The _nf suffixed variants (no-fail) check the result of the operation,
 * and call abort on failure.
 *
 * usage example:
 * -------------
 * int *arr = dynarr_alloc(0, sizeof *arr);
 *
 * int x = 10;
 * dynarr_push_nf(arr, &x);
 * x = 5;
 * dynarr_push_nf(arr, &x);
 * x = 42;
 * dynarr_push_nf(arr, &x);
 *
 * for(i=0; i<dynarr_size(arr); i++) {
 *     printf("%d\n", arr[i]);
 *  }
 *  dynarr_free(arr);
 */

void *dynarr_alloc(int elem, int szelem);
void *dynarr_alloc_nf(int elem, int szelem);
void dynarr_free(void *da);
void *dynarr_resize(void *da, int elem);

/* dynarr_empty returns non-zero if the array is empty
 * Complexity: O(1) */
int dynarr_empty(void *da);
/* dynarr_size returns the number of elements in the array
 * Complexity: O(1) */
int dynarr_size(void *da);

void *dynarr_clear(void *da);

/* stack semantics */
void *dynarr_push(void *da, void *item);
void *dynarr_pop(void *da);

/* Finalize the array. No more resizing is possible after this call.
 * Use free() instead of dynarr_free() to deallocate a finalized array.
 * Returns pointer to the finalized array.
 * dynarr_finalize can't fail.
 * Complexity: O(n)
 */
void *dynarr_finalize(void *da);

/* helper macros */
#define dynarr_resize_nf(da, n) \
	do { \
		if(!((da) = dynarr_resize((da), (n)))) { \
			fprintf(stderr, "failed to resize dynamic array\n"); \
			abort(); \
		} \
	} while(0)
#define dynarr_clear_nf(da) \
	do { \
		if(!((da) = dynarr_clear(da))) { \
			fprintf(stderr, "failed to clear dynamic array\n"); \
			abort(); \
		} \
	} while(0)
#define dynarr_push_nf(da, item) \
	do { \
		if(!((da) = dynarr_push((da), (item)))) { \
			fprintf(stderr, "failed to append to dynamic array\n"); \
			abort(); \
		} \
	} while(0)
#define dynarr_pop_nf(da) \
	do { \
		if(!((da) = dynarr_pop(da))) { \
			fprintf(stderr, "failed to remove from dynamic array\n"); \
			abort(); \
		} \
	} while(0)

/* utility macros to push characters to a string. assumes and maintains
 * the invariant that the last element is always a zero
 */
#define dynarr_strpush_nf(da, c) \
	do { \
		char cnull = 0, ch = (char)(c); \
		dynarr_pop_nf(da); \
		dynarr_push_nf((da), &ch); \
		dynarr_push_nf((da), &cnull); \
	} while(0)

#define dynarr_strpop_nf(da) \
	do { \
		char cnull = 0; \
		dynarr_pop_nf(da); \
		dynarr_pop_nf(da); \
		dynarr_push_nf((da), &cnull); \
	} while(0)


#endif	/* DYNARR_H_ */
