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
	int i, j, k;
	char buf[1025], buf2[1025], *pathname = tempnam(NULL, "fm1MB");

	memset(buf, 'x', 1024);
	memset(buf2, 'x', 1024);

	plan(144);

	for (i = 4; i <= 9; i++)
	{
		for (j = 4; j <=9; j++)
		{
			fastmap_attr_init(&attr);
			fastmap_attr_setrecords(&attr, 100000);
			fastmap_attr_setksize(&attr, 2 << i);
			fastmap_attr_setvsize(&attr, 2 << j);
			fastmap_attr_setformat(&attr, FASTMAP_BLOCK);

			ok (fastmap_outhandle_init(&ohandle, &attr, pathname) == FASTMAP_OK, "Starting Test (%d, %d)", i, j);

			for (k = 0; k < 100000; k++)
			{
				sprintf(buf + 992, "<<<<----%016d---->>>>", k);
				block.key = buf + (1024 - (2 << i));
				sprintf(buf2 + 992, ">>>>++++%016d++++<<<<", k);
				block.value = buf2;
				if (fastmap_outhandle_put(&ohandle, (fastmap_record_t*)&block) != FASTMAP_OK)
				{
					fail("unable to write key '%s', during test (%d, %d)", buf, i, j);
					i = j = 10;
					goto fail;
				}
			}

			ok (fastmap_outhandle_destroy(&ohandle) == FASTMAP_OK, "write phase complete");
			ok (fastmap_inhandle_init(&ihandle, pathname) == FASTMAP_OK, "read phase starting");

			for (k = 0; k < 100000; k++)
			{
				sprintf(buf + 992, "<<<<----%016d---->>>>", k);
				block.key = buf + (1024 - (2 << i));
				sprintf(buf2 + 992, ">>>>++++%016d++++<<<<", k);

				if (fastmap_inhandle_get(&ihandle, (fastmap_record_t*)&block) != FASTMAP_OK)
				{
					fail("unable to lookup key '%s', during test (%d, %d)", buf, i, j);
					i = j = 10;
					goto fail;
				}

				if (memcmp(block.value, buf2, ihandle.handle.attr.vsize) != 0)
				{
					fail("value mismatch for key '%s', during test (%d, %d)", buf, i, j);
					i = j = 10;
					goto fail;
				}
			}

			ok (fastmap_inhandle_destroy(&ihandle) == FASTMAP_OK, "Finished Test (%d, %d)", i, j);
			fastmap_attr_destroy(&attr);
fail:
			unlink(pathname);
		}
	}

	free(pathname);

	done_testing();
}
