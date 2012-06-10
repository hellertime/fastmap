#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <tap.h>
#include <fastmap.h>

int main(void)
{
	fastmap_block_t block;
	fastmap_attr_t attr;
	fastmap_inhandle_t ihandle;
	fastmap_outhandle_t ohandle;
	int i, seen_anything_but_success;
	char buf[33], buf2[30], *pathname = tempnam(NULL, "fm1MB");

	plan(26);

	fastmap_attr_init(&attr);
	fastmap_attr_setrecords(&attr, 1000000);
	fastmap_attr_setksize(&attr, 32);
	fastmap_attr_setvsize(&attr, 29);
	fastmap_attr_setformat(&attr, FASTMAP_BLOCK);

	ok(fastmap_outhandle_init(&ohandle, &attr, pathname) == FASTMAP_OK, "created fastmap");

	seen_anything_but_success = 0;

	for (i = 0; i < 1000000; i++)
	{
		sprintf(buf, "<<<<----%016d---->>>>", i);
		block.key = buf;
		sprintf(buf2, ">>>---%016d---<<<<", i);
		block.value = buf2;
		if (fastmap_outhandle_put(&ohandle, (fastmap_record_t*)&block) != FASTMAP_OK && !seen_anything_but_success)
		{
			seen_anything_but_success = 1;
			diag("unable to put key: '%016d'", i);
		}

		if (i % 100000 == 0)
		{
			ok(seen_anything_but_success == 0, "success after: %07d writes", i);
		}
	}
	ok(seen_anything_but_success == 0, "success after: %07d writes", i);

	ok(fastmap_outhandle_destroy(&ohandle) == FASTMAP_OK, "saved fastmap");

	ok(fastmap_inhandle_init(&ihandle, pathname) == FASTMAP_OK, "opened fastmap");

	seen_anything_but_success = 0;

	for (i = 0; i < 1000000; i++)
	{
		sprintf(buf, "<<<<----%016d---->>>>", i);
		block.key = buf;
		sprintf(buf2, ">>>---%016d---<<<<", i);
		if (fastmap_inhandle_get(&ihandle, (fastmap_record_t*)&block) != FASTMAP_OK && !seen_anything_but_success)
		{
			seen_anything_but_success = 1;
			diag("unable to get key: '%016d'", i);
		}

		if (strncmp(block.value, buf2, ihandle.handle.attr.vsize) != 0 && !seen_anything_but_success)
		{
			seen_anything_but_success = 1;
			diag("value mismatch for key: '%016d'", i);
		}

		if (i % 100000 == 0)
		{
			ok(seen_anything_but_success == 0, "success after: %07d reads", i);
		}
	}
	ok(seen_anything_but_success == 0, "success after: %07d reads", i);

	ok(fastmap_inhandle_destroy(&ihandle) == FASTMAP_OK, "closed fastmap");

	unlink(pathname);

	done_testing();
}
