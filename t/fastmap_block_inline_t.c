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
	fastmap_block_t blocks[] = {
		{ "0001", "aaaaxyzz" },
		{ "0002", "bbbbxyxx" },
		{ "0003", "ccccxyab" },
		{ "0004", "ddddxycc" },
		{ "0005", "ffffxdef" }
	};
	fastmap_block_t block;

	fastmap_attr_t attr;
	fastmap_inhandle_t ihandle;
	fastmap_outhandle_t ohandle;
	size_t i;
	char buf[9] = {0}, *pathname = tempnam(NULL, "fmbli");

	setvbuf(stdout, NULL, _IONBF, 0);

	fastmap_attr_init(&attr);
	fastmap_attr_setrecords(&attr, 5);
	fastmap_attr_setksize(&attr, 4);
	fastmap_attr_setvsize(&attr, 8);
	fastmap_attr_setformat(&attr, FASTMAP_BLOCK);

	plan(18);

	fastmap_outhandle_init(&ohandle, &attr, pathname);

	ok(!(ohandle.handle.flags & 0x02), "INLINE_BLOCK");

	for (i = 0; i < (sizeof(blocks) / sizeof(blocks[0])); i++)
		ok(fastmap_outhandle_put(&ohandle, (fastmap_record_t*)&blocks[i]) == FASTMAP_OK, "fastmap_outhandle_put(\"%s\")", blocks[i].key);
	fastmap_outhandle_destroy(&ohandle);

	fastmap_inhandle_init(&ihandle, pathname);

	ok(!(ihandle.handle.flags & 0x02), "INLINE_BLOCKS");

	for (i = 0; i < (sizeof(blocks) / sizeof(blocks[0])); i++)
	{
		block.key = blocks[i].key;
		ok(fastmap_inhandle_get(&ihandle, (fastmap_record_t*)&block) == FASTMAP_OK, "fastmap_inhandle_get(\"%s\")", block.key);
		strncpy(buf, block.value, strlen(blocks[i].value));
		is(buf, blocks[i].value, "%s == %s", buf, blocks[i].value);
	}	

	fastmap_inhandle_destroy(&ihandle);

	ok(unlink(pathname) == 0, "unlink()");
	free(pathname);

	done_testing();
}
