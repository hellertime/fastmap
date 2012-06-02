#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tap.h>
#include <fastmap.h>

int main(void)
{
	fastmap_blob_t blobs[] = {
		{ "0001", "foo",    strlen("foo")    },
		{ "0002", "bar",    strlen("bar")    },
		{ "0003", "baz",    strlen("baz")    },
		{ "0004", "quux",   strlen("quux")   },
		{ "0005", "quorum", strlen("quorum") }
	};
	fastmap_blob_t blob;

	fastmap_attr_t attr;
	fastmap_inhandle_t ihandle;
	fastmap_outhandle_t ohandle;
	size_t i;
	char buf[7] = {0}, *pathname = tempnam(NULL, "fmblb");

	setvbuf(stdout, NULL, _IONBF, 0);

	fastmap_attr_init(&attr);
	fastmap_attr_setelements(&attr, 5);
	fastmap_attr_setksize(&attr, 4);
	fastmap_attr_setvsize(&attr, 5);
	fastmap_attr_setformat(&attr, FASTMAP_BLOB);

	plan(16);

	fastmap_outhandle_init(&ohandle, &attr, pathname);

	for (i = 0; i < (sizeof(blobs) / sizeof(blobs[0])); i++)
		ok(fastmap_outhandle_put(&ohandle, (fastmap_element_t*)&blobs[i]) == FASTMAP_OK, "fastmap_outhandle_put(\"%s\")", blobs[i].key);
	fastmap_outhandle_destroy(&ohandle);

	fastmap_inhandle_init(&ihandle, pathname);

	for (i = 0; i < (sizeof(blobs) / sizeof(blobs[0])); i++)
	{
		blob.key = blobs[i].key;
		ok(fastmap_inhandle_get(&ihandle, (fastmap_element_t*)&blob) == FASTMAP_OK, "fastmap_inhandle_get(\"%s\")", blob.key);
		strncpy(buf, blob.value, strlen(blobs[i].value));
		buf[strlen(blobs[i].value)] = '\0';
		is(buf, blobs[i].value, "%s == %s", buf, blobs[i].value);
	}	

	fastmap_inhandle_destroy(&ihandle);

	ok(unlink(pathname) == 0, "unlink()");
	free(pathname);

	done_testing();
}
