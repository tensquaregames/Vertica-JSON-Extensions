#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <json/selector.h>


int main()
{
	json_selector_iter_t iter;

	const char *selector = "foo.123.zażółć gęślą jaźń.4-8-15-16-23-42.bar\\.baz.quux";
	json_selector_iter_init(&iter, selector, strlen(selector));
	assert(json_selector_iter_eqcmp(&iter, "foo", strlen("foo")) == true);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, "123", strlen("123")) == true);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, "zażółć gęślą jaźń", strlen("zażółć gęślą jaźń")) == true);;
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, "4-8-15-16-23-42", strlen("4-8-15-16-23-42")) == true);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, "bar.baz", strlen("bar.baz")) == true);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, "quux", strlen("quux")) == true);
	assert(json_selector_iter_next(&iter) == false);

	const char *escaped = "abc\\.def\\.ghi.123\\.456..def\t\n\\.\t.$";
	const char *frag1 = "abc.def.ghi";
	const char *nfrag1 = "abc\\.def.ghi";
	const char *frag2 = "123.456";
	const char *nfrag2 = "123\\.456";
	const char *frag3 = "";
	const char *frag4 = "def\t\n.\t";
	const char *nfrag4 = "def\t\n\\.\t";
	const char *frag5 = "$";
	json_selector_iter_init(&iter, escaped, strlen(escaped));
	assert(json_selector_iter_eqcmp(&iter, frag1, strlen(frag1)) == true);
	assert(json_selector_iter_eqcmp(&iter, nfrag1, strlen(nfrag1)) == false);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, frag2, strlen(frag2)) == true);
	assert(json_selector_iter_eqcmp(&iter, nfrag2, strlen(nfrag2)) == false);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, frag3, strlen(frag3)) == true);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, frag4, strlen(frag4)) == true);
	assert(json_selector_iter_eqcmp(&iter, nfrag4, strlen(nfrag4)) == false);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, frag5, strlen(frag5)) == true);
	assert(json_selector_iter_next(&iter) == false);

	const char *empty = "";
	json_selector_iter_init(&iter, empty, 0);
	assert(json_selector_iter_eqcmp(&iter, "", 0) == true);
	assert(json_selector_iter_next(&iter) == false);

	const char *weird = ".\\..";
	json_selector_iter_init(&iter, weird, strlen(weird));
	assert(json_selector_iter_eqcmp(&iter, "", 0) == true);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, ".", 1) == true);
	assert(json_selector_iter_next(&iter) == true);
	assert(json_selector_iter_eqcmp(&iter, "", 0) == true);
	assert(json_selector_iter_next(&iter) == false);

	return 0;
}
