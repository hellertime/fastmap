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
	memset(attr, 0, sizeof(struct fastmap_attr));
	return 0;
}

int fastmap_attr_copy(fastmap_attr_t *dst, const fastmap_attr_t *src)
{
	dst->ksize = src->ksize;
	dst->mode = src->mode;
	return 0;
}

int fastmap_attr_serialize(fastmap_attr_serialized_t *sattr, const fastmap_attr_t *attr)
{
	uint64_t serialized_ksize = htobe64((uint64_t)attr->ksize);
	uint8_t serialized_mode = attr->mode;

	memcpy((sattr->buffer + FASTMAP_ATTR_SERIALIZED_KSIZE_OFFSET), &serialized_ksize, FASTMAP_ATTR_SERIALIZED_KSIZE_BYTES);
	memcpy((sattr->buffer + FASTMAP_ATTR_SERIALIZED_MODE_OFFSET), &serialized_mode, FASTMAP_ATTR_SERIALIZED_MODE_BYTES);

	return 0;
}

int fastmap_attr_deserialize(fastmap_attr_t *attr, const fastmap_attr_serialized_t *sattr)
{
	uint64_t deserialized_ksize;
	uint8_t deserialized_mode;

	memcpy(&deserialized_ksize, (sattr->buffer + FASTMAP_ATTR_SERIALIZED_KSIZE_OFFSET), FASTMAP_ATTR_SERIALIZED_KSIZE_BYTES);
	memcpy(&deserialized_mode, (sattr->buffer + FASTMAP_ATTR_SERIALIZED_MODE_OFFSET), FASTMAP_ATTR_SERIALIZED_MODE_BYTES);

	attr->ksize = be64toh(deserialized_ksize);
	attr->mode = deserialized_mode;

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
