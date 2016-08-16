#include <json/selector.h>


static void
json_selector_iter_advance(json_selector_iter_t *iter)
{
	for (;;) {
		if (iter->curr_end == iter->end) {
			return;
		}

		if (*iter->curr_end == '.') {
			return;
		}
		if (*iter->curr_end == '\\') {
			iter->curr_end++;
			if (iter->curr_end == iter->end) {
				return;
			}
		}
		iter->curr_end++;
	}
}

void
json_selector_iter_init(json_selector_iter_t *iter,
                        const char *str, size_t len)
{
	iter->curr_begin = str;
	iter->curr_end = str;
	iter->end = str + len;
	json_selector_iter_advance(iter);
}

bool
json_selector_iter_next(json_selector_iter_t *iter)
{
	if (iter->curr_end == iter->end) {
		return false;
	}

	iter->curr_end++;
	iter->curr_begin = iter->curr_end;
	json_selector_iter_advance(iter);
	return true;
}

bool
json_selector_iter_eqcmp(const json_selector_iter_t *iter,
                         const char *str, size_t len)
{
	const char *iter_curr = iter->curr_begin;
	const char *str_curr = str;
	for (;;) {
		if (iter_curr == iter->curr_end &&
		    str_curr == str + len) {
			return true;
		}
		if (iter_curr == iter->curr_end ||
		    str_curr == str + len) {
			return false;
		}

		if (*str_curr == '.') {
			if (*iter_curr != '\\') {
				return false;
			}
			iter_curr++;
			if (iter_curr == iter->curr_end) {
				return false;
			}
		}
		if (*str_curr != *iter_curr) {
			return false;
		}
		str_curr++;
		iter_curr++;
	}
}
