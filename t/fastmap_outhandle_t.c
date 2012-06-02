#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tap.h>
#include <fastmap.h>

int main(void)
{
	fastmap_atom_t atoms[] = {
		{ "0001" },
		{ "0002" },
		{ "0003" },
		{ "0004" },
		{ "0005" },
	};

	fastmap_attr_t attr;
	fastmap_outhandle_t ohandle;
	size_t i;
	char *pathname = tempnam(NULL, "fmout");

	setvbuf(stdout, NULL, _IONBF, 0);

	fastmap_attr_init(&attr);
	fastmap_attr_setelements(&attr, 5);
	fastmap_attr_setksize(&attr, 4);
	fastmap_attr_setformat(&attr, FASTMAP_ATOM);

	plan(12);

	ok(fastmap_outhandle_init(NULL, &attr, pathname) == EINVAL, "fastmap_outhandle_init(NULL_HANDLE)");
	ok(fastmap_outhandle_init(&ohandle, NULL, pathname) == EINVAL, "fastmap_outhandle_init(NULL_ATTR)");
	ok(fastmap_outhandle_init(&ohandle, &attr, "") != FASTMAP_OK, "fastmap_outhandle_init(\"\")");
	ok(fastmap_outhandle_init(&ohandle, &attr, pathname) == FASTMAP_OK, "fastmap_outhandle_init()");

	for (i = 0; i < (sizeof(atoms) / sizeof(atoms[0])); i++)
	{
		fastmap_element_t *element = (fastmap_element_t*)&atoms[i];
		ok(fastmap_outhandle_put(&ohandle, element) == FASTMAP_OK, "fastmap_outhandle_put(%s)", atoms[i].key);
	}

	ok(fastmap_outhandle_destroy(NULL) == EINVAL, "fastmap_outhandle_destroy(NULL)");
	ok(fastmap_outhandle_destroy(&ohandle) == FASTMAP_OK, "fastmap_outhandle_destroy()");

	ok(unlink(pathname) == 0, "unlink()");
	free(pathname);

	done_testing();
}
