#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tap.h>
#include <fastmap.h>

int main(void)
{
	fastmap_pair_t pairs[] = {
		{ "0001", "aaaa" },
		{ "0002", "bbbb" },
		{ "0003", "cccc" },
		{ "0004", "dddd" },
		{ "0005", "ffff" }
	};
	fastmap_pair_t pair;

	fastmap_attr_t attr;
	fastmap_inhandle_t ihandle;
	fastmap_outhandle_t ohandle;
	size_t i;
	char buf[5] = {0}, *pathname = tempnam(NULL, "fmpar");

	setvbuf(stdout, NULL, _IONBF, 0);

	fastmap_attr_init(&attr);
	fastmap_attr_setelements(&attr, 5);
	fastmap_attr_setksize(&attr, 4);
	fastmap_attr_setformat(&attr, FASTMAP_PAIR);

	plan(16);

	fastmap_outhandle_init(&ohandle, &attr, pathname);
	for (i = 0; i < (sizeof(pairs) / sizeof(pairs[0])); i++)
		ok(fastmap_outhandle_put(&ohandle, (fastmap_element_t*)&pairs[i]) == FASTMAP_OK, "fastmap_outhandle_put(\"%s\")", pairs[i].key);
	fastmap_outhandle_destroy(&ohandle);

	fastmap_inhandle_init(&ihandle, pathname);
	for (i = 0; i < (sizeof(pairs) / sizeof(pairs[0])); i++)
	{
		pair.key = pairs[i].key;
		ok(fastmap_inhandle_get(&ihandle, (fastmap_element_t*)&pair) == FASTMAP_OK, "fastmap_inhandle_get(\"%s\")", pair.key);
		strncpy(buf, pair.value, strlen(pairs[i].value));
		is(buf, pairs[i].value, "%s == %s", buf, pairs[i].value);
	}	

	fastmap_inhandle_destroy(&ihandle);

	ok(unlink(pathname) == 0, "unlink()");
	free(pathname);

	done_testing();
}
