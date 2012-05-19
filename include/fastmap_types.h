#ifndef FASTMAP_TYPES_H
#define FASTMAP_TYPES_H 1

/**
 * @file   fastmap_types.h
 * @Author Chris Heller (hellertime@gmail.com)
 * @date   May, 2012
 * @brief  Fastmap key-value index file
 *
 */

#include <stdlib.h>

struct fastmap_datumattr
{
	size_t vsize;
};

struct fastmap_datum
{
	struct fastmap_datumattr attr;
	void *key;
	void *value;
};

typedef enum
{
	FASTMAP_WRITE_ONLY,
	FASTMAP_READ_ONLY
} fastmap_mode_t;

struct fastmap_attr
{
	size_t ksize;
	fastmap_mode_t mode;
};

#define FASTMAP_ATTR_SERIALIZED_KSIZE_OFFSET 0
#define FASTMAP_ATTR_SERIALIZED_KSIZE_BYTES 8
#define FASTMAP_ATTR_SERIALIZED_MODE_OFFSET 8
#define FASTMAP_ATTR_SERIALIZED_MODE_BYTES 1
#define FASTMAP_ATTR_SERIALIZED_BYTES 9 /* _KSIZE_BYTES + _MODE_BYTES */

struct fastmap_attr_serialized
{
	char buffer[FASTMAP_ATTR_SERIALIZED_BYTES];
};

struct fastmap
{
	struct fastmap_attr attr;
	int fd;
};

#endif /* ! FASTMAP_TYPES_H */
