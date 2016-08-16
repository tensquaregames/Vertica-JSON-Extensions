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


#endif
