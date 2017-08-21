#ifndef __FILE_JSON_COPY_RESULT_HPP
#define __FILE_JSON_COPY_RESULT_HPP


#include <Vertica.h>

extern "C" {
#include <json/slice.h>
}


class JsonValueResult {
public:

	static void copyResult(const json_slice_t &json, Vertica::VString &result)
	{
		/* The function fails when g++ O3 level of optimization is used (Vertica v8.1.1-0)
		*/
		result.copy(json.src, json.len);
	}
};

class JsonStringResult {
public:

	static void copyResult(const json_slice_t &json, Vertica::VString &result)
	{
		if (json.len >= 2 && json.src[0] == '"' && json.src[json.len - 1] == '"') {
			result.copy(json.src + 1, json.len - 2);
		} else {
			result.setNull();
		}
	}
};

class JsonUnquotedResult {
public:

	static void copyResult(const json_slice_t &json, Vertica::VString &result)
	{
		if (json.len >= 2 && json.src[0] == '"' && json.src[json.len - 1] == '"') {
			result.copy(json.src + 1, json.len - 2);
		} else {
			result.copy(json.src, json.len);
		}
	}
};


#endif
