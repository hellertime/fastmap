fastmap - A library for creating and querying fast, read-only, key-value maps.
==============================================================================

## DESCRIPTION

A fastmap is a key-value map file. It is created once in full, and is optimized
for reading values out both sequentially and non-sequentially.

The library expects that the data will be inserted in sorted order, and will
fail to create a proper fastmap if data is inserted improperly.

In addition to requiring the user to pre-sort the input data, the library also
depends on the user providing a few parameters before inserting any data:

  * The number of elements (key-value pairs) that will be inserted into the fastmap.

  * The size of each key, keys can be of any size, but all keys must be the same size.

  * The type of fastmap to build, which can be any of the valid formats (see [FORMATS][])
    If the type selected is `FASTMAP_BLOCK` the user must also provide the size of
    each value.

### Fastmap function return values

Most fastmap functions return `FASTMAP_OK` on success, and a non-zero error value
type `int` on failure. A user can obtain a string message for the given error
with a call to `fastmap_strerror(3)`. Note that the fastmap functions do not set
errno.

### Fastmap thread safety

The design of the library is such that a single thread, in a single process, will create
the fastmap file. Once created it can then be queried by both the creating thread and
other threads, even across processes -- so long as the operating system supports a form
of shared memory mapped file. On POSIX systems this is accomplished with mmap(3) using the
`MAP_SHARED` flag (on Windows, MapViewOfFile provides the mechanism). The library also
relies on the operating systems page cache to deal with the caching of fastmap pages.

## FORMATS

A fastmap supports multiple formats all are key-value storage formats, but they are provide
various benefits and trade-offs to the user.

There are four formats supported:

  * `FASTMAP_ATOM` this is a degenerate key-value format, where the key is the value.

  * `FASTMAP_PAIR` this is a fixed size key-value format, where the key size and
    the value size are the same, and all keys and values have the same size.

  * `FASTMAP_BLOCK` this format has a fixed size key, and a fixed size value, but the
    size of the value is not necessarily the same size as the key.

  * `FASTMAP_BLOB` this format has a fixed size key, but the value sizes are variable.

## FILE LAYOUT

A fastmap file is constructed incrementally. Upon initialization a header is written to the
file at offset 0 with initial values for the fastmap.

Located after the header are branch nodes consisting of a set of key, pointer pairs known
as search pairs. Branch nodes are grouped into levels such that each level contains enough
search pairs to be able to point to the start of every branch node in the next level.
The largest level contains enough search pairs to point to the start of every leaf node.

Located after the branch node levels are the leaf nodes. Depending on the fastmap format the
leaf nodes will consist of a set of keys, keys and values or keys and pointers. In any case
these are known as leaf node items.

In the case of a `FASTMAP_BLOB` the pointer will address the starting offset of the leaf node
value, the first offset will be just after the last leaf node.

## EXAMPLES

The program below demonstrates the creation of a simple `FASTMAP_ATOM` fastmap file.

`#include <errno.h>

#include <fastmap.h>

#define handle_error(err) \
  do { if (err != FASTMAP_OK) { perror(fastmap_strerror(err)); exit(EXIT_FAILURE); } } while (0)

int
main(int argc, char *argv[])
{
  fastmap_atom_t atoms[] = {
    { "0001" },
    { "0002" },
    { "0003" },
    { "0004" },
    { "0005" },
    {      0 }
  };
  fastmap_atom_t *atom;
  fastmap_outhandle_t ohandle;
  fastmap_attr_t attr;

  handle_error( fastmap_attr_init(&attr) );
  handle_error( fastmap_attr_setelements(&attr, 5) );
  handle_error( fastmap_attr_setksize(&attr, 4) );
  handle_error( fastmap_attr_setformat(&attr, FASTMAP_ATOM) );

  handle_error( fastmap_outhandle_init(&ohandle, &attr, "./fastmap-atom.map") );

  for (atom = atoms; atom; atom++)
  {
    handle_error( fastmap_outhandle_put(&ohandle, atom) );
  }

  handle_error( fastmap_outhandle_destroy(&ohandle) );

  handle_error( fastmap_attr_destroy(&attr) );

  return 0;
}`

This second program demonstrates reading the fastmap created in the previous example.

`#include <assert.h>
#include <errno.h>

#include <fastmap.h>

#define handle_error(err) \
  do { if (err != FASTMAP_OK) { perror(fastmap_strerror(err)); exit(EXIT_FAILURE); } } while (0)

int
main(int argc, char *argv[])
{
  fastmap_atom_t *atom;
  fastmap_inhandle_t ihandle;
  
  handle_error( fastmap_inhandle_init(&ihandle, "./fastmap-atom.map") );

  handle_error( fastmap_inhandle_get(&ihandle, "0001", &atom) );
  assert( atom != NULL && "If `atom` is NULL the key was not found." );

  handle_error( fastmap_inhandle_get(&ihandle, "0006", &atom) );
  assert( atom == NULL && "The key is not in the map, so `atom` is NULL" );

  handle_error( fastmap_inhandle_destroy(&ihandle) );

  return 0;
}`

This next example demonstrates creating other fastmap formats.

`#include <errno.h>

#include <fastmap.h>

#define handle_error(err) \
  do { if (err != FASTMAP_OK) { perror(fastmap_strerror(err)); exit(EXIT_FAILURE); } } while (0)

int
main(int argc, char *argv[])
{
  fastmap_pair_t pairs[] = {
    { "0001", "aaaa" },
    { "0002", "bbbb" },
    { "0003", "cccc" },
    { "0004", "dddd" },
    { "0005", "ffff" },
    {      0,      0 }
  };

  fastmap_block_t blocks[] = {
    { "0001", "aabbaabb" },
    { "0002", "bbccbbcc" },
    { "0003", "ccddccdd" },
    { "0004", "ddeeddee" },
    { "0005", "eeffeeff" },
    {      0,          0 }
  };

  fastmap_blob_t blobs[] = {
    { "0001", "foo",    strlen("foo")    },
    { "0002", "bar",    strlen("bar")    },
    { "0003", "baz",    strlen("baz")    },
    { "0004", "quux",   strlen("quux")   },
    { "0005", "quorum", strlen("quorum") },
    {      0,        0,                0 }
  };

  fastmap_pair_t *pair;
  fastmap_block_t *block;
  fastmap_blob_t *blob;
  fastmap_outhandle_t ohandle;

  handle_error( fastmap_attr_init(&attr) );
  handle_error( fastmap_attr_setelements(&attr, 5) );
  handle_error( fastmap_attr_setksize(&attr, 4) );
  handle_error( fastmap_attr_setformat(&attr, FASTMAP_PAIR) );

  handle_error( fastmap_outhandle_init(&ohandle, &attr, "./fastmap-pair.map") );

  for (pair = pairs; pair; pair++)
  {
    handle_error( fastmap_outhandle_put(&ohandle, pair) );
  }

  handle_error( fastmap_outhandle_destroy(&ohandle) );

  handle_error( fastmap_attr_destroy(&attr) );
  handle_error( fastmap_attr_init(&attr) );

  handle_error( fastmap_attr_setelements(&attr, 5) );
  handle_error( fastmap_attr_setksize(&attr, 4) );
  handle_error( fastmap_attr_setvsize(&attr, strlen(blocks[0].value)) );
  handle_error( fastmap_attr_setformat(&attr, FASTMAP_BLOCK) );

  handle_error( fastmap_outhandle_init(&ohandle, &attr, "./fastmap-block.map") );

  for (block = blocks; block; block++)
  {
    handle_error( fastmap_outhandle_put(&ohandle, block) );
  }

  handle_error( fastmap_outhandle_destroy(&ohandle) );

  handle_error( fastmap_attr_destroy(&attr) );
  handle_error( fastmap_attr_init(&attr) );

  handle_error( fastmap_attr_setelements(&attr, 5) );
  handle_error( fastmap_attr_setksize(&attr, 4) );
  handle_error( fastmap_attr_setformat(&attr, FASTMAP_BLOB) );

  handle_error( fastmap_outhandle_init(&ohandle, &attr, "./fastmap-blob.map") );

  handle_error( fastmap_outhandle_mput(&ohandle, blobs, 5) );

  handle_error( fastmap_outhandle_destroy(&ohandle) );

  handle_error( fastmap_attr_destroy(&attr) );

  return 0;
}`

## LIMITATIONS

This library is not portable to systems which do not implement mmap(3) (or simmilar).

The fastmap file format is not portable. To remedy this the utility `fastmap2json` is
provided to produce a portable form which can safely be relocated to another system.
There is also `json2fastmap` which converts the portable format back into a system
specific form.
