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

struct fastmap
{
	struct fastmap_attr attr;
	int fd;
};

#endif /* ! FASTMAP_TYPES_H */
