fastmap(3) -- library routines for fastmap files
================================================

## SYNOPSIS

### Build and Install

```sh
./configure && make && make install
```

### Using

```c
#include <fastmap.h>
```

### Creating a fastmap

```c
fastmap_attr_t attr;
fastmap_outhandle_t out;

fastmap_atom_t atoms[] = { 1, 2, 3, ..., 999999 };

fastmap_attr_init(&attr);
fastmap_attr_setformat(&attr, FASTMAP_ATOM);
fastmap_attr_setrecords(&attr, sizeof(atoms) / sizeof(atoms[0]));
fastmap_attr_setksize(&attr, sizeof(atoms[0]));

fastmap_outhandle_init(&out, &attr, "/a/fastmap");

for (int i = 0; i < sizeof(atoms) / sizeof(atoms[0]); i++)
  fastmap_outhandle_put(&out, (fastmap_record_t*)&atoms[i]);

fastmap_outhandle_destory(&out);
```

### Querying a fastmap

```c
fastmap_atom_t atom;
fastmap_inhandle_t in;

fastmap_inhandle_init(&in, "/a/fastmap");

atom.key = 16631;
assert(fastmap_inhandle_get(&in, &atom) == FASTMAP_OK);

fastmap_inhandle_destroy(&in);
```

## DESCRIPTION

A fastmap file represents an immutable key-value map. 

When creating a fastmap, records must be inserted in sorted order.

One must decided, a priori, on certain parameters which affect how the
fastmap will be created:

  * The number of records the fastmap will contain.

  * The size of a key.

  * The fastmap format (see [FORMATS][FORMATS]).

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

[FORMATS]:

  * `FASTMAP_ATOM` this is a degenerate key-value format, where the key is the value.

  * `FASTMAP_PAIR` this is a fixed size key-value format, where the key size and
    the value size are the same, and all keys and values have the same size.

  * `FASTMAP_BLOCK` this format has a fixed size key, and a fixed size value, but the
    size of the value is not necessarily the same size as the key. If this format is
    selected, on must also choose the size of a value in addition to the size of a key.

  * `FASTMAP_BLOB` this format has a fixed size key, but the value sizes are variable.

## FILE LAYOUT

A fastmap has a simple structure consisting of a header, search page levels, a leaf page
 level, and finally (if needed) the pages containing values.

Visually it looks something like this:

```
+-------------------------- +
| HEADER                    |
+---------------------------+
| SEARCH PAGE LEVEL (n - 1) |
+---------------------------+
| ...                       |
+---------------------------+
| SEARCH PAGE LEVEL (0)     |
+---------------------------+
| LEAF PAGE LEVEL           |
+---------------------------+
| [VALUE PAGES] (if needed) |
+---------------------------+
```

If the fastmap has a format of `FASTMAP_ATOM` or `FASTMAP_PAIR` the value pages will be
omitted.

If the fastmap has a format of `FASTMAP_BLOCK`, and the size of a key plus the size of a
value are a multiple of the page size, the value pages will also be omitted.

## LIMITATIONS

Keys currently must be a multiple of the page size, this will be relaxed in a later release.

The page size is currently fixed to the size of a page on the filesystem on which the fastmap
is created. This will become adjustable in the future.

This library is not portable to systems which do not implement mmap(3) (or simmilar).

The fastmap file format is not portable. To remedy this the utility `fastmap2json` is
provided to produce a portable form which can safely be relocated to another system.
There is also `json2fastmap` which converts the portable format back into a system
specific form.
