#ifdef HAVE_CONFIG_H
#include <fastmap_config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fastmap_types.h>
#include <fastmap.h>

int fastmap_create(fastmap_t *fm, const fastmap_attr_t *attr, const char *path)
{
	int error;
	fastmap_attr_serialized_t attr_serialized;

	fastmap_attr_copy(&fm->attr, attr);

	if (attr->mode != FASTMAP_WRITE_ONLY)
		return EINVAL;

	fm->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
	if (fm->fd == -1)
		return errno;

	error = fastmap_attr_serialize(&attr_serialized, &fm->attr);
	if (error != 0)
		return error;

	error = fastmap_attr_serialized_write(fm->fd, &attr_serialized);
	if (error == -1)
		return errno;

	/* TODO: Pre-allocate inner pages and map them into memory -- get ready for writing */

	return 0;
}

int fastmap_open(fastmap_t *fm, const char *path)
{
	int error;
	fastmap_attr_serialized_t attr_serialized;

	fm->fd = open(path, O_RDONLY);
	if (fm->fd == -1)
		return errno;

	error = fastmap_attr_serialized_read(fm->fd, &attr_serialized);
	if (error == -1)
		return errno;

	return fastmap_load(fm, path);
}

int fastmap_close(fastmap_t *fm)
{
	fastmap_unload(fm);
	close(fm->fd);
	memset(fm, 0, sizeof(struct fastmap));
	return 0;
}

int fastmap_get(fastmap_t *fm, fastmap_datum_t *datum)
{
	int error = fastmap_seek(fm, datum->key);
	return fastmap_read(fm, datum);
}

int fastmap_get_many(fastmap_t *fm, fastmap_datum_t *data[], size_t how_many)
{
	size_t i;

	for(i = 0; i < how_many; i++)
	{
		int error = fastmap_seek(fm, data[i]->key);
		fastmap_read(fm, data[i]);
	}

	return 0;
}

int fastmap_put(fastmap_t *fm, fastmap_datum_t *datum)
{
	return fastmap_write(fm, datum);
}

int fastmap_put_many(fastmap_t *fm, fastmap_datum_t *data[], size_t how_many)
{
	size_t i;

	for(i = 0; i < how_many; i++)
	{
		fastmap_put(fm, data[i]);
	}
}
