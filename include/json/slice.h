#ifndef __FILE_VERTICA_JSON_SLICE_H
#define __FILE_VERTICA_JSON_SLICE_H


#include <stdbool.h>
#include <stddef.h>


typedef struct {
	const char *src;
	size_t len;
} json_slice_t;

json_slice_t
json_slice_new(const char *src, size_t len);

bool
json_slice_query(const json_slice_t *input,
                 const char *selector, size_t selector_len,
                 json_slice_t *output);

typedef enum {
	JSON_ARRAY_ITER_OK,
	JSON_ARRAY_ITER_END,
	JSON_ARRAY_ITER_FAIL
} json_array_iter_result_t;

typedef struct {
	const char *begin;
	const char *end;
} json_array_iter_t;

json_array_iter_result_t
json_array_iter_init(json_array_iter_t *iter, const json_slice_t *input);

json_array_iter_result_t
json_array_iter_next(json_array_iter_t *iter, json_slice_t *result);



#endif
