#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <json/selector.h>
#include <json/slice.h>


#define STRING_TEST_POSITIVE(json_src, selector_src, expectation) \
{\
	json_slice_t json = json_slice_new(json_src, strlen(json_src));\
	json_slice_t result;\
	assert(json_slice_query(&json, selector_src, strlen(selector_src), &result) == true);\
	assert(strncmp(result.src, expectation, result.len) == 0);\
}

#define STRING_TEST_NEGATIVE(json_src, selector_src) \
{\
	json_slice_t json = json_slice_new(json_src, strlen(json_src));\
	json_slice_t result;\
	assert(json_slice_query(&json, selector_src, strlen(selector_src), &result) == false);\
}


char *load_json(const char *filepath)
{
	FILE *file = fopen(filepath, "rb");
	assert(file != NULL);

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(sizeof(char) * (size + 1));
	assert(buffer);
	assert(fread(buffer, 1, size, file) == size);
	buffer[size] = 0;

	fclose(file);
	return buffer;
}


int main()
{
	const char *empty_json = "";
	STRING_TEST_NEGATIVE(empty_json, "foo");
	STRING_TEST_NEGATIVE(empty_json, "bar");
	STRING_TEST_NEGATIVE(empty_json, "");

	const char *emptykey_json = "{ \"\": 42, \"bar\": { \"\": true }, \"baz.\": false }";
	STRING_TEST_POSITIVE(emptykey_json, "", "42");
	STRING_TEST_POSITIVE(emptykey_json, "bar.", "true");
	STRING_TEST_NEGATIVE(emptykey_json, "bar\\.");
	STRING_TEST_POSITIVE(emptykey_json, "baz\\.", "false");
	STRING_TEST_NEGATIVE(emptykey_json, "baz.");

	const char *simple_json = "{ \"foo\": { \"bar\": 42, \"baz\": 4815162342 } }";
	STRING_TEST_POSITIVE(simple_json, "foo.bar", "42");
	STRING_TEST_POSITIVE(simple_json, "foo.baz", "4815162342");
	STRING_TEST_POSITIVE(simple_json, "foo", "{ \"bar\": 42, \"baz\": 4815162342 }");
	STRING_TEST_NEGATIVE(simple_json, "bar");
	STRING_TEST_NEGATIVE(simple_json, "baz");
	STRING_TEST_NEGATIVE(simple_json, "foo.quux");

	const char *array_json = "[ { \"foo\": [] }, { \"bar\": [-5, true, false, null] } ]";
	STRING_TEST_POSITIVE(array_json, "0.foo", "[]");
	STRING_TEST_NEGATIVE(array_json, "0.bar");
	STRING_TEST_NEGATIVE(array_json, "0.foo.0");
	STRING_TEST_NEGATIVE(array_json, "0.foo.1");
	STRING_TEST_POSITIVE(array_json, "1.bar", "[-5, true, false, null]");
	STRING_TEST_NEGATIVE(array_json, "1.foo");
	STRING_TEST_POSITIVE(array_json, "1.bar.0", "-5");
	STRING_TEST_POSITIVE(array_json, "1.bar.1", "true");
	STRING_TEST_POSITIVE(array_json, "1.bar.2", "false");
	STRING_TEST_POSITIVE(array_json, "1.bar.3", "null");
	STRING_TEST_NEGATIVE(array_json, "1.bar.4");
	STRING_TEST_NEGATIVE(array_json, "2");

	const char *dotted_json = "{ \"foo.bar\": true, \"foo\": { \"bar\": false, \"baz.quux\": null } }";
	STRING_TEST_POSITIVE(dotted_json, "foo\\.bar", "true");
	STRING_TEST_POSITIVE(dotted_json, "foo.bar", "false");
	STRING_TEST_POSITIVE(dotted_json, "foo.baz\\.quux", "null");
	STRING_TEST_NEGATIVE(dotted_json, "foo\\.bar.baz.quux");

	char *jsoncheck1_json = load_json("test/jsons/pass1.json");
	STRING_TEST_NEGATIVE(jsoncheck1_json, "-1");
	STRING_TEST_POSITIVE(jsoncheck1_json, "0", "\"JSON Test Pattern pass1\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "1", "{\"object with 1 member\":[\"array with 1 element\"]}");
	STRING_TEST_POSITIVE(jsoncheck1_json, "2", "{}");
	STRING_TEST_POSITIVE(jsoncheck1_json, "3", "[]");
	STRING_TEST_POSITIVE(jsoncheck1_json, "4", "-42");
	STRING_TEST_POSITIVE(jsoncheck1_json, "5", "true");
	STRING_TEST_POSITIVE(jsoncheck1_json, "6", "false");
	STRING_TEST_POSITIVE(jsoncheck1_json, "7", "null");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.integer", "1234567890");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.real", "-9876.543210");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.e", "0.123456789e-12");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.E", "1.234567890E+34");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.", "23456789012E66");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.zero", "0");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.one", "1");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.space", "\" \"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.quote", "\"\\\"\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.backslash", "\"\\\\\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.controls", "\"\\b\\f\\n\\r\\t\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.slash", "\"/ & \\/\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.alpha", "\"abcdefghijklmnopqrstuvwyz\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.ALPHA", "\"ABCDEFGHIJKLMNOPQRSTUVWYZ\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.digit", "\"0123456789\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.0123456789", "\"digit\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.special", "\"`1~!@#$%^&*()_+-={':[,]}|;.</>?\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.hex", "\"\\u0123\\u4567\\u89AB\\uCDEF\\uabcd\\uef4A\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.true", "true");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.false", "false");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.null", "null");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.array", "[  ]");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.object", "{  }");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.address", "\"50 St. James Street\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.url", "\"http://www.JSON.org/\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.comment", "\"// /* <!-- --\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.# -- --> */", "\" \"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8. s p a c e d .0", "1");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8. s p a c e d .1", "2");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8. s p a c e d .2", "3");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8. s p a c e d .3", "4");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8. s p a c e d .4", "5");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8. s p a c e d .5", "6");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8. s p a c e d .6", "7");
	STRING_TEST_NEGATIVE(jsoncheck1_json, "8. s p a c e d .7");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.compact", "[1,2,3,4,5,6,7]");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.jsontext", "\"{\\\"object with 1 member\\\":[\\\"array with 1 element\\\"]}\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.quotes", "\"&#34; \\u0022 %22 0x22 034 &#x22;\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "8.\\/\\\\\\\"\\uCAFE\\uBABE\\uAB98\\uFCDE\\ubcda\\uef4A\\b\\f\\n\\r\\t`1~!@#$%^&*()_+-=[]{}|;:',\\./<>?", "\"A key can be any string\"");
	STRING_TEST_POSITIVE(jsoncheck1_json, "9", "0.5");
	STRING_TEST_POSITIVE(jsoncheck1_json, "10", "98.6");
	STRING_TEST_POSITIVE(jsoncheck1_json, "11", "99.44");
	STRING_TEST_POSITIVE(jsoncheck1_json, "12", "1066");
	STRING_TEST_POSITIVE(jsoncheck1_json, "13", "1e1");
	STRING_TEST_POSITIVE(jsoncheck1_json, "14", "0.1e1");
	STRING_TEST_POSITIVE(jsoncheck1_json, "15", "1e-1");
	STRING_TEST_POSITIVE(jsoncheck1_json, "16", "1e00");
	STRING_TEST_POSITIVE(jsoncheck1_json, "17", "2e+00");
	STRING_TEST_POSITIVE(jsoncheck1_json, "18", "2e-00");
	STRING_TEST_POSITIVE(jsoncheck1_json, "19", "\"rosebud\"");
	STRING_TEST_NEGATIVE(jsoncheck1_json, "20");
	free(jsoncheck1_json);

	char *jsoncheck2_json = load_json("test/jsons/pass2.json");
	STRING_TEST_NEGATIVE(jsoncheck2_json, "-1");
	STRING_TEST_POSITIVE(jsoncheck2_json, "0", "[[[[[[[[[[[[[[[[[[\"Not too deep\"]]]]]]]]]]]]]]]]]]");
	STRING_TEST_NEGATIVE(jsoncheck2_json, "0.-42");
	STRING_TEST_POSITIVE(jsoncheck2_json, "0.0", "[[[[[[[[[[[[[[[[[\"Not too deep\"]]]]]]]]]]]]]]]]]");
	STRING_TEST_NEGATIVE(jsoncheck2_json, "0.0.foo");
	STRING_TEST_POSITIVE(jsoncheck2_json, "0.0.0", "[[[[[[[[[[[[[[[[\"Not too deep\"]]]]]]]]]]]]]]]]");
	STRING_TEST_POSITIVE(jsoncheck2_json, "0.0.0.0.0.0.0", "[[[[[[[[[[[[\"Not too deep\"]]]]]]]]]]]]");
	STRING_TEST_POSITIVE(jsoncheck2_json, "0.0.0.0.0.0.0.0.0.0.0.0", "[[[[[[[\"Not too deep\"]]]]]]]");
	STRING_TEST_POSITIVE(jsoncheck2_json, "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0", "[[\"Not too deep\"]]");
	STRING_TEST_POSITIVE(jsoncheck2_json, "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0", "\"Not too deep\"");
	STRING_TEST_NEGATIVE(jsoncheck2_json, "0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0");

	free(jsoncheck2_json);

	char *jsoncheck3_json = load_json("test/jsons/pass3.json");
	STRING_TEST_POSITIVE(jsoncheck3_json, "JSON Test Pattern pass3.The outermost value", "\"must be an object or array.\"");
	STRING_TEST_POSITIVE(jsoncheck3_json, "JSON Test Pattern pass3.In this test", "\"It is an object.\"");
	free(jsoncheck3_json);

	return 0;
}
