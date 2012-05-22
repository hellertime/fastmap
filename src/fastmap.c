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

/**
 * Write computed values
 *
 * @param[in] fd the descriptor to write
 * @param[in] fm the 'fastmap_t' object being written
 */
static int _write_header(int fd, const fastmap_t *fm)
{
	fastmap_attr_serialized_t sattr;
	uint64_t _64;
	uint32_t _32;
	int i;

	_32 = (uint32_t)htobe32((uint32_t)fm->blocksize);
	write(fd, &_32, sizeof(_32));

	_32 = (uint32_t)htobe32((uint32_t)fm->numlevels);
	write(fd, &_32, sizeof(_32));

	for (i = 0; i < fm->numlevels; i++)
	{
		_64 = (uint64_t)htobe64((uint64_t)fm->pagesperlevel[i]);
		write(fd, &_64, sizeof(_64));
	}

	_64 = (uint64_t)htobe64((uint64_t)fm->branchingfactor);
	write(fd, &_64, sizeof(_64));

	_64 = (uint64_t)htobe64((uint64_t)fm->bptrsize);
	write(fd, &_64, sizeof(_64));

	_64 = (uint64_t)htobe64((uint64_t)fm->keysperpage);
	write(fd, &_64, sizeof(_64));

	_64 = (uint64_t)htobe64((uint64_t)fm->leafnodes);
	write(fd, &_64, sizeof(_64))

	fastmap_attr_serialize(&sattr, &fm->attr);

	fastmap_attr_serialized_write(fm->fd, &sattr);

	/* TODO: ERRORS! */
	return 0;
}

/**
 * Read computed values
 *
 * @param[in]  fd the descriptor to read
 * @param[out] fm the 'fastmap_t' object which will hold the values read
 */
static int fastmap_read_header(int fd, fastmap_t *fm)
{
	fastmap_attr_serialized_t sattr;
	uint64_t _64;
	uint32_t _32;
	int i;

	/* TODO: Verfiy the blocksize, if it doesn't match the current st_blksize something is very wrong! */
	read(fd, &_32, sizeof(_32));
	fm->blocksize = (blksize_t)be32toh(_32);

	read(fd, &_32, sizeof(_32));
	fm->numlevels = (int)be32toh(_32);

	for (i = 0; i < fm->numlevels; i++)
	{
		read(fd, &_64, sizeof(_64));
		fm->pagesperlevel[i] = (size_t)be64toh(_64);
	}

	read(fd, &_64, sizeof(_64));
	fm->branchingfactor = (size_t)be64toh(_64);

	read(fd, &_64, sizeof(_64));
	fm->bptrsize = (size_t)be64toh(_64);

	read(fd, &_64, sizeof(_64));
	fm->keysperpage = (size_t)be64toh(_64);

	read(fd, &_64, sizeof(_64));
	fm->leafnodes = (size_t)be64toh(_64);

	fastmap_attr_serialized_read(fd, &sattr);

	fastmap_attr_deserialize(&fm->attr, &sattr);

	/* TODO: ERRORS! */
	return 0;
}


/**
 * Computes the keys per page value
 *
 * @param[in] fm 'fastmap_t' object parameterizing the keysperpage value
 *
 */
static ssize_t _compute_keysperpage(const fastmap_t *fm)
{
	switch (fm->attr->type)
	{
	case FASTMAP_TYPE_ATOM:
		return (fm->blocksize / fm->attr.ksize);
	case FASTMAP_TYPE_PAIR:
		return (fm->blocksize / (fm->attr.ksize * 2));
	case FASTMAP_TYPE_BLOCK:
	case FASTMAP_TYPE_BLOB:
		return (fm->blocksize / (fm->bptrsize));
	default:
		assert(0 && "Unknown fm->attr->type value");
	}
}

/**
 * Computes the branching factor of the map
 *
 * @param[in] fm 'fastmap_t' object parameterizing the branchingfactor
 */
static ssize_t _compute_branchingfactor(const fastmap_t *fm)
{
	return ((fm->keysperpage + fm->attr.ksize) / (fm->attr.ksize + fm->ptrsize));
}

int fastmap_create(fastmap_t *fm, const fastmap_attr_t *attr, const char *path)
{
	struct stat st;
	int error;
	fastmap_attr_serialized_t attr_serialized;

	fastmap_attr_copy(&fm->attr, attr);

	if (attr->mode != FASTMAP_WRITE_ONLY)
		return EINVAL;

	fm->fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
	if (fm->fd == -1)
		return errno;

	error = fstat(fm->fd, &st);
	if (error == -1)
		return errno;

	fm->blocksize = st.st_blksize;
	fm->keysperpage = _compute_keysperpage(fm);
	fm->leafnodes = (size_t)ceil((double)fm->attr->elements / (double)fm->keysperpage);
	fm->branchingfactor = _compute_branchingfactor(fm);

	fm->numlevels = 0;
	{
		size_t levelpages = fm->leafnodes;
		while (levelpages > 1)
		{
			levelpages = (size_t)ceil((double)levelpages / (double)fm->branchingfactor);
			fm->pagesperlevel[fm->numlevels++] = levelpages;
			if (fm->numlevels > FASTMAP_MAX_LEVELS)
				return -1; /* TODO: Return FASTMAP_ERROR_TOO_MANY_LEVELS */
		}
	}

	_write_header(fm->fd, &fm);

	/* TODO: Pre-allocate inner pages and map them into memory -- get ready for writing */

	return 0;
}

int fastmap_open(fastmap_t *fm, const char *path)
{
	int error;

	fm->fd = open(path, O_RDONLY);
	if (fm->fd == -1)
		return errno;

	error = _read_header(fm->fd, fm);
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
