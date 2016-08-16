#ifndef __FILE_VERTICA_JSON_SELECTOR_H
#define __FILE_VERTICA_JSON_SELECTOR_H


#include <stdbool.h>
#include <stddef.h>


typedef struct {
	const char *curr_begin;
	const char *curr_end;
	const char *end;
} json_selector_iter_t;

void
json_selector_iter_init(json_selector_iter_t *iter,
                        const char *str, size_t len);

bool
json_selector_iter_next(json_selector_iter_t *iter);

bool
json_selector_iter_eqcmp(const json_selector_iter_t *iter,
                         const char *str, size_t len);


#endif
