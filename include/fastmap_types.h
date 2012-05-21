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

typedef enum
{
	FASTMAP_TYPE_ATOM,  /** ksize == vsize and the key is the value */
	FASTMAP_TYPE_PAIR,  /** ksize == vsize */
	FASTMAP_TYPE_BLOCK, /** ksize != vsize and all vsize are equal */
	FASTMAP_TYPE_BLOB   /** ksize != vsize and every vsize can be different */
} fastmap_type_t;

struct fastmap_attr
{
	size_t elements;
	size_t fixedvsize;
	size_t ksize;
	fastmap_mode_t mode;
	fastmap_type_t type;
};

#define FASTMAP_ATTR_SERIALIZED_ELEMENTS_OFFET		0
#define FASTMAP_ATTR_SERIALIZED_ELEMENTS_BYTES		8
#define FASTMAP_ATTR_SERIALIZED_FIXEDVSIZE_OFFSET	8
#define FASTMAP_ATTR_SERIALIZED_FIXEDVSIZE_BYTES	8
#define FASTMAP_ATTR_SERIALIZED_KSIZE_OFFSET		16
#define FASTMAP_ATTR_SERIALIZED_KSIZE_BYTES		8
#define FASTMAP_ATTR_SERIALIZED_MODE_OFFSET		24
#define FASTMAP_ATTR_SERIALIZED_MODE_BYTES		1
#define FASTMAP_ATTR_SERIALIZED_TYPE_OFFSET		25 
#define FASTMAP_ATTR_SERIALIZED_TYPE_BYTES		1
#define FASTMAP_ATTR_SERIALIZED_BYTES			26

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
