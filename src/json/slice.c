#include <ctype.h>
#include <string.h>

#include <json/selector.h>
#include <json/slice.h>


json_slice_t
json_slice_new(const char *src, size_t len)
{
	json_slice_t slice;
	slice.src = src;
	slice.len = len;
	return slice;
}


static bool
read_size(const char *src, size_t len, size_t *result)
{
	*result = 0;
	for (size_t i = 0; i < len; i++) {
		if (!('0' <= src[i] && src[i] <= '9')) {
			return false;
		}

		*result = (*result) * 10 + (src[i] - '0');
	}
	return true;
}


typedef struct {
	const char *begin;
	const char *end;
} parse_state_t;

static bool
skip_whitespace(parse_state_t *state)
{
	while (state->begin != state->end) {
		if (!isspace(*state->begin)) {
			return true;
		}
		state->begin++;
	}
	return true;
}

static bool
skip_char(parse_state_t *state, char c)
{
	if (state->begin == state->end) {
		return false;
	}

	if (*state->begin != c) {
		return false;
	}

	state->begin++;
	return true;
}

static bool
skip_string(parse_state_t *state)
{
	for (;;) {
		if (state->begin == state->end) {
			return false;
		}

		/* Note that we do NOT consume the closing quotemark. */
		if (*state->begin == '"') {
			return true;
		}

		/*
		 * Original JSON specification states that only certain symbols
		 * are allowed after a backslash. But we don't really care so
		 * we happily accept anything that follows it.
		 */
		if (*state->begin == '\\') {
			state->begin++;
			if (state->begin == state->end) {
				return false;
			}
		}

		/* Everything else should be a valid unicode character. */
		state->begin++;
	}
}


static bool
parse_true(parse_state_t *state, json_slice_t *output);

static bool
parse_false(parse_state_t *state, json_slice_t *output);

static bool
parse_null(parse_state_t *state, json_slice_t *output);

static bool
parse_string(parse_state_t *state, json_slice_t *output);

static bool
parse_number(parse_state_t *state, json_slice_t *output);

static bool
parse_object(parse_state_t *state, json_slice_t *output);

static bool
parse_array(parse_state_t *state, json_slice_t *output);

static bool
parse_value(parse_state_t *state, json_slice_t *output);

static bool
traverse_object(parse_state_t *state, json_selector_iter_t *iter,
                json_slice_t *output);

static bool
traverse_array(parse_state_t *state, json_selector_iter_t *iter,
               json_slice_t *output);

static bool
traverse_value(parse_state_t *state, json_selector_iter_t *iter,
               json_slice_t *output);


static bool
parse_true(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	output->src = state->begin;
	if (!skip_char(state, 't')) return false;
	if (!skip_char(state, 'r')) return false;
	if (!skip_char(state, 'u')) return false;
	if (!skip_char(state, 'e')) return false;
	output->len = state->begin - output->src;
	return true;
}

static bool
parse_false(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	output->src = state->begin;
	if (!skip_char(state, 'f')) return false;
	if (!skip_char(state, 'a')) return false;
	if (!skip_char(state, 'l')) return false;
	if (!skip_char(state, 's')) return false;
	if (!skip_char(state, 'e')) return false;
	output->len = state->begin - output->src;
	return true;
}

static bool
parse_null(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	output->src = state->begin;
	if (!skip_char(state, 'n')) return false;
	if (!skip_char(state, 'u')) return false;
	if (!skip_char(state, 'l')) return false;
	if (!skip_char(state, 'l')) return false;
	output->len = state->begin - output->src;
	return true;
}

static bool
parse_string(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	output->src = state->begin;
	if (!skip_char(state, '"')) return false;
	if (!skip_string(state)) return false;
	if (!skip_char(state, '"')) return false;
	output->len = state->begin - output->src;
	return true;
}

static bool
parse_number(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	output->src = state->begin;

	for (;;) {
		/*
		 * We never expect numbers to be at the very and of the file,
		 * so this is not that bad actually and saves some code
		 * duplication.
		 */
		if (state->begin == state->end) return false;
		switch (*state->begin) {
		case '-':
		case '+':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
		case 'e':
		case 'E':
			state->begin++;
			break;
		default:
			output->len = state->begin - output->src;
			/*
			 * We are successful only if we have actually managed
			 * to read something (i.e. an empty string is not a
			 * valid number).
			 */
			return output->len > 0;
		}
	}
}

static bool
parse_object(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	output->src = state->begin;
	if (!skip_char(state, '{')) return false;
	if (!skip_whitespace(state)) return false;
	if (state->begin != state->end && *state->begin == '}') {
		state->begin++;
		output->len = state->begin - output->src;
		return true;
	}
	for (;;) {
		json_slice_t key, value;
		if (!parse_string(state, &key)) return false;
		if (!skip_whitespace(state)) return false;
		if (!skip_char(state, ':')) return false;
		if (!parse_value(state, &value)) return false;
		if (!skip_whitespace(state)) return false;
		if (state->begin == state->end) {
			return false;
		}
		if (*state->begin == '}') {
			state->begin++;
			output->len = state->begin - output->src;
			return true;
		} else if (*state->begin == ',') {
			state->begin++;
			continue;
		} else {
			return false;
		}
	}
}

static bool
traverse_object(parse_state_t *state,json_selector_iter_t *iter,
                json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	if (!skip_char(state, '{')) return false;
	if (!skip_whitespace(state)) return false;
	if (state->begin != state->end && *state->begin == '}') {
		state->begin++;
		output->len = state->begin - output->src;
		return true;
	}
	for (;;) {
		json_slice_t key;
		if (!parse_string(state, &key)) return false;
		if (!skip_whitespace(state)) return false;
		if (!skip_char(state, ':')) return false;

		/*
		 * `key` is an escaped string so we need to account for the
		 * wrapping quotemarks.
		 */
		if (json_selector_iter_eqcmp(iter, key.src + 1, key.len - 2)) {
			/*
			 * We have managed to match the selector iterator, so
			 * now are either done (there are no further iterator
			 * values) in which case we output parsed value or
			 * there are selector fragments waiting to be matched.
			 */
			if (json_selector_iter_next(iter)) {
				return traverse_value(state, iter, output);
			} else {
				return parse_value(state, output);
			}
		}
		
		/*
		 * The key is not matched, so we parse a value and throw it
		 * away.
		 */
		json_slice_t value;
		if (!parse_value(state, &value)) return false;
		if (!skip_whitespace(state)) return false;
		if (!skip_char(state, ',')) return false;
	}
}

static bool
parse_array(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	output->src = state->begin;
	if (!skip_char(state, '[')) return false;
	if (!skip_whitespace(state)) return false;
	if (state->begin != state->end && *state->begin == ']') {
		state->begin++;
		output->len = state->begin - output->src;
		return true;
	}
	for (;;) {
		json_slice_t value;
		if (!parse_value(state, &value)) return false;
		if (!skip_whitespace(state)) return false;
		if (*state->begin == ']') {
			state->begin++;
			output->len = state->begin - output->src;
			return true;
		}
		if (!skip_char(state, ',')) return false;
	}
}

static bool
traverse_array(parse_state_t *state, json_selector_iter_t *iter,
               json_slice_t *output)
{
	size_t index;
	if (!read_size(iter->curr_begin, iter->curr_end - iter->curr_begin,
                       &index)) {
		return false;
	}

	if (!skip_whitespace(state)) return false;
	if (!skip_char(state, '[')) return false;

	/* So for `index` times we ignore parsed values. */
	for (size_t i = 0; i < index; i++) {
		json_slice_t value;
		if (!parse_value(state, &value)) return false;
		if (!skip_whitespace(state)) return false;
		if (!skip_char(state, ',')) return false;
	}

	/*
	 * See similar comment in the `traverse_object` method for explanation
	 * of this fragment.
	 */
	if (json_selector_iter_next(iter)) {
		return traverse_value(state, iter, output);
	} else {
		return parse_value(state, output);
	}
}

static bool
parse_value(parse_state_t *state, json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	if (state->begin == state->end) return false;
	switch (*state->begin) {
	case '"': return parse_string(state, output);
	case '{': return parse_object(state, output);
	case '[': return parse_array(state, output);
	case 't': return parse_true(state, output);
	case 'f': return parse_false(state, output);
	case 'n': return parse_null(state, output);
	default: return parse_number(state, output);
	}
}

static bool
traverse_value(parse_state_t *state, json_selector_iter_t *iter,
               json_slice_t *output)
{
	if (!skip_whitespace(state)) return false;
	if (state->begin == state->end) {
		return false;
	}

	switch (*state->begin) {
	case '{': return traverse_object(state, iter, output);
	case '[': return traverse_array(state, iter, output);
	default: return false;
	}
}

bool
json_slice_query(const json_slice_t *input,
                 const char *selector, size_t selector_len,
                 json_slice_t *output)
{
	json_selector_iter_t iter;
	json_selector_iter_init(&iter, selector, selector_len);

	parse_state_t state;
	state.begin = input->src;
	state.end = input->src + input->len;
	return traverse_value(&state, &iter, output);
}

json_array_iter_result_t
json_array_iter_init(json_array_iter_t *iter, const json_slice_t *input)
{
	parse_state_t state;
	state.begin = input->src;
	state.end = input->src + input->len;

	if (!skip_whitespace(&state)) return JSON_ARRAY_ITER_FAIL;
	if (!skip_char(&state, '[')) return JSON_ARRAY_ITER_FAIL;

	iter->begin = state.begin;
	iter->end = state.end;
	return JSON_ARRAY_ITER_OK;
}

json_array_iter_result_t
json_array_iter_next(json_array_iter_t *iter, json_slice_t *result)
{
	parse_state_t state;
	state.begin = iter->begin;
	state.end = iter->end;

	if (!skip_whitespace(&state)) return JSON_ARRAY_ITER_FAIL;
	if (state.begin == state.end) return JSON_ARRAY_ITER_FAIL;
	if (*state.begin == ']') return JSON_ARRAY_ITER_END;

	if (!parse_value(&state, result)) return JSON_ARRAY_ITER_FAIL;
	if (!skip_whitespace(&state)) return JSON_ARRAY_ITER_FAIL;
	if (state.begin == state.end) return JSON_ARRAY_ITER_FAIL;
	if (*state.begin == ',') state.begin++;
	iter->begin = state.begin;
	/* No need to set `iter->end` since it does not change. */
	return JSON_ARRAY_ITER_OK;
}
