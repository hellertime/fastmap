#ifdef HAVE_CONFIG_H
#include <fastmap_config.h>
#endif

#include <string.h>
#include <unistd.h>

#include <fastmap_types.h>
#include <fastmap.h>

int fastmap_attr_init(fastmap_attr_t *attr)
{
	int error = fastmap_attr_destroy(attr);
	attr->mode = FASTMAP_READ_ONLY;
	return error;	
}

int fastmap_attr_destroy(fastmap_attr_t *attr)
{
	memset(attr, 0, sizeof(*attr));
	return 0;
}

int fastmap_attr_copy(fastmap_attr_t *dst, const fastmap_attr_t *src)
{
	memcpy(&dst, &src, sizeof(*src));
	return 0;
}

int fastmap_attr_setelements(fastmap_attr_t *attr, size_t elements)
{
	attr->elements = elements;
	return 0;
}

int fastmap_attr_getelements(fastmap_attr_t *attr, size_t *elements)
{
	*elements = attr->elements;
	return 0;
}

int fastmap_attr_setfixedvsize(fastmap_attr_t *attr, size_t fixedvsize)
{
	attr->fixedvsize = fixedvsize;
	return 0;
}

int fastmap_attr_getfixedvsize(fastmap_attr_t *attr, size_t *fixedvsize)
{
	*fixedvsize = attr->fixedvsize;
	return 0;
}

int fastmap_attr_setksize(fastmap_attr_t *attr, size_t ksize)
{
	attr->ksize = ksize;
	return 0;
}

int fastmap_attr_getksize(fastmap_attr_t *attr, size_t *ksize)
{
	*ksize = attr->ksize;
	return 0;
}

int fastmap_attr_setmode(fastmap_attr_t *attr, fastmap_mode_t mode)
{
	attr->mode = mode;
	return 0;
}

int fastmap_attr_getmode(fastmap_attr_t *attr, fastmap_mode_t *mode)
{
	*mode = attr->mode;
	return 0;
}

int fastmap_attr_settype(fastmap_attr_t *attr, fastmap_type_t type)
{
	attr->type = type;
	return 0;
}

int fastmap_attr_gettype(fastmap_attr_t *attr, fastmap_type_t *type)
{
	*type = attr->type;
	return 0;
}

int fastmap_attr_serialize(fastmap_attr_serialized_t *sattr, const fastmap_attr_t *attr)
{
	uint64_t _64;
	uint8_t _8;
	uint64_t serialized_fixedvsize = htobe64((uint64_t)attr->fixedvsize);
	uint64_t serialized_ksize = htobe64((uint64_t)attr->ksize);
	uint64_t serialized_leafnodes
	uint8_t serialized_mode = attr->mode;
	uint8_t serialized_type = attr->type;

	_64 = htobe64((uint64_t)attr->fixedvsize);
	memcpy((sattr->buffer + FASTMAP_ATTR_SERIALIZED_FIXEDVSIZE_OFFSET), &_64, FASTMAP_ATTR_SERIALIZED_FIXEDVSIZE_BYTES);

	_64 = htobe64((uint64_t)attr->ksize);
	memcpy((sattr->buffer + FASTMAP_ATTR_SERIALIZED_KSIZE_OFFSET), &_64, FASTMAP_ATTR_SERIALIZED_KSIZE_BYTES);

	_64 = htobe64((uint64_t)attr->leafnode);
	memcpy((sattr->buffer + FASTMAP_ATTR_SERIALIZED_LEAFNODES_OFFSET), &_64, FASTMAP_ATTR_SERIALIZED_LEAFNODES_BYTES);
	
	_8 = (uint8_t)attr->mode;
	memcpy((sattr->buffer + FASTMAP_ATTR_SERIALIZED_MODE_OFFSET), &_8, FASTMAP_ATTR_SERIALIZED_MODE_BYTES);

	_8 = (uint8_t)attr->type;
	memcpy((sattr->buffer + FASTMAP_ATTR_SERIALIZED_TYPE_OFFSET), &_8, FASTMAP_ATTR_SERIALIZED_TYPE_BYTES);

	return 0;
}

int fastmap_attr_deserialize(fastmap_attr_t *attr, const fastmap_attr_serialized_t *sattr)
{
	uint64_t _64;
	uint8_t _8;

	memcpy(&_64, (sattr->buffer + FASTMAP_ATTR_SERIALIZED_FIXEDVSIZE_OFFSET), FASTMAP_ATTR_SERIALIZED_FIXEDVSIZE_BYTES);
	attr->fixedvsize = (size_t)be64toh(_64);

	memcpy(&_64, (sattr->buffer + FASTMAP_ATTR_SERIALIZED_KSIZE_OFFSET), FASTMAP_ATTR_SERIALIZED_KSIZE_BYTES);
	attr->ksize = (size_t)be64toh(_64);

	memcpy(&_64, (sattr->buffer + FASTMAP_ATTR_SERIALIZED_LEAFNODES), FASTMAP_ATTR_SERIALIZED_LEAFNODS_BYTES);
	attr->leafnodes = (size_t)be64toh(_64);

	memcpy(&_8, (sattr->buffer + FASTMAP_ATTR_SERIALIZED_MODE_OFFSET), FASTMAP_ATTR_SERIALIZED_MODE_BYTES);
	attr->mode = (fastmap_mode_t)_8;

	memcpy(&_8, (sattr->buffer + FASTMAP_ATTR_SERIALIZED_TYPE_OFFSET), FASTMAP_ATTR_SERIALIZED_TYPE_BYTES);
	attr->type = (fastmap_type_t)_8;

	return 0;
}

ssize_t fastmap_attr_serialized_write(int fd, const fastmap_attr_serialized_t *sattr)
{
	/* TODO: Write header and checksum */
	return write(fd, sattr->buffer, FASTMAP_ATTR_SERIALIZED_BYTES);
}

ssize_t fastmap_attr_serialized_read(int fd, fastmap_attr_serialized_t *sattr)
{
	/* TODO: Read header and verify checksum */
	return read(fd, sattr->buffer, FASTMAP_ATTR_SERIALIZED_BYTES);
}
