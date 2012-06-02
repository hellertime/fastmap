#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tap.h>
#include <fastmap.h>

int cmp(fastmap_attr_t *attr, const void *a, const void *b)
{
	return memcmp(a, b, attr->ksize);
}

int main(void)
{
	fastmap_atom_t atoms[] = {
		{ "0001" },
		{ "0002" },
		{ "0003" },
		{ "0004" },
		{ "0005" },
	};
	fastmap_atom_t atom;

	fastmap_attr_t attr;
	fastmap_inhandle_t ihandle;
	fastmap_outhandle_t ohandle;
	size_t i,x;
	fastmap_format_t a;
	char *pathname = tempnam(NULL, "fminh");

	setvbuf(stdout, NULL, _IONBF, 0);

	fastmap_attr_init(&attr);
	fastmap_attr_setelements(&attr, 5);
	fastmap_attr_setksize(&attr, 4);
	fastmap_attr_setformat(&attr, FASTMAP_ATOM);

	fastmap_outhandle_init(&ohandle, &attr, pathname);
	for (i = 0; i < (sizeof(atoms) / sizeof(atoms[0])); i++)
	{
		fastmap_element_t *element = (fastmap_element_t*)&atoms[i];
		fastmap_outhandle_put(&ohandle, element);
	}
	fastmap_outhandle_destroy(&ohandle);

	plan(20);

	ok(fastmap_inhandle_init(NULL, pathname) == EINVAL, "fastmap_inhandle_init(NULL)");
	ok(fastmap_inhandle_init(&ihandle, "") != FASTMAP_OK, "fastmap_inhandle_init(\"\")");
	ok(fastmap_inhandle_init(&ihandle, pathname) == FASTMAP_OK, "fastmap_inhandle_init()");
	ok(fastmap_inhandle_setcmpfunc(&ihandle, cmp) == FASTMAP_OK, "fastmap_inhandle_setcmpfunc()");

	ok(fastmap_inhandle_getattr(&ihandle, &attr) == FASTMAP_OK, "fastmap_inhandle_getattr()");
	ok(fastmap_attr_getelements(&attr, &x) == FASTMAP_OK, "fastmap_attr_getelements()");
	cmp_ok(x, "==", 5, "elements: %d", x);
	ok(fastmap_attr_getksize(&attr, &x) == FASTMAP_OK, "fastmap_attr_getksize()");
	cmp_ok(x, "==", 4, "key size: %d", x);
	ok(fastmap_attr_getformat(&attr, &a) == FASTMAP_OK, "fastmap_attr_getformat()");
	cmp_ok(a, "==", FASTMAP_ATOM, "format: FASTMAP_ATOM");

	for (i = 0; i < 5; i++)
	{
		fastmap_element_t *element = (fastmap_element_t*)&atoms[i];
		ok(fastmap_inhandle_get(&ihandle, element) == FASTMAP_OK, "fastmap_inhandle_get(\"%s\")", atoms[i].key);
	}

	atom.key = "0006";
	ok(fastmap_inhandle_get(&ihandle, (fastmap_element_t*)&atom) == FASTMAP_NOT_FOUND, "fastmap_inhandle_get(\"0006\")");

	ok(fastmap_inhandle_destroy(NULL) == EINVAL, "fastmap_inhandle_destroy(NULL)");
	ok(fastmap_inhandle_destroy(&ihandle) == FASTMAP_OK, "fastmap_inhandle_destroy()");

	ok(unlink(pathname) == 0, "unlink()");
	free(pathname);

	done_testing();
}
