#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include <tap.h>
#include <fastmap.h>

int main(void)
{
	fastmap_attr_t attr;
	fastmap_format_t a, b;
	size_t x, y;

	setvbuf(stdout, NULL, _IONBF, 0);

	plan(12);

	ok(fastmap_attr_init(NULL) == EINVAL, "fastmap_attr_init(NULL)");
	ok(fastmap_attr_init(&attr) == FASTMAP_OK, "fastmap_attr_init()");

	x = (size_t)rand();
	ok(fastmap_attr_setelements(&attr, x) == FASTMAP_OK, "fastmap_attr_setelements()");
	ok(fastmap_attr_getelements(&attr,&y) == FASTMAP_OK && x == y, "fastmap_attr_getelements()");

	x = (size_t)rand();
	ok(fastmap_attr_setksize(&attr, x) == FASTMAP_OK, "fastmap_attr_setksize()");
	ok(fastmap_attr_getksize(&attr, &y) == FASTMAP_OK && x == y, "fastmap_attr_getksize()");

	x = (size_t)rand();
	ok(fastmap_attr_setvsize(&attr, x) == FASTMAP_OK, "fastmap_attr_setvsize()");
	ok(fastmap_attr_getvsize(&attr, &y) == FASTMAP_OK && x == y, "fastmap_attr_getvsize()");

	a = FASTMAP_ATOM;
	ok(fastmap_attr_setformat(&attr, a) == FASTMAP_OK, "fastmap_attr_setformat()");
	ok(fastmap_attr_getformat(&attr, &b) == FASTMAP_OK && a == b, "fastmap_attr_getformat()");

	ok(fastmap_attr_destroy(NULL) == EINVAL, "fastmap_attr_destroy(NULL)");
	ok(fastmap_attr_destroy(&attr) == FASTMAP_OK, "fastmap_attr_destroy()");

	done_testing();
}
