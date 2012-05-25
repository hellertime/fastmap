#ifdef HAVE_CONFIG_H #include <fastmap_config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fastmap.h>

struct fastmap_attr_t
{
	size_t elements;
	size_t ksize;
	size_t vsize;
	fastmap_format_t format;
};

#define FASTMAP_MAXLEVELS 32 /* 32 levels allows for 10^64 elements (assuming 8-byte keys and 8-byte pointers), plenty of space */

struct fastmap_handle_t
{
	uint32_t magic;
	uint32_t version;
	size_t pagesperlevel[FASTMAP_MAXLEVELS];
	fastmap_attr_t attr;
	size_t branchingfactor;
	size_t bptrsize;
	size_t itemsperleafnode;
	size_t leafnodes;
	size_t leafnodeitemsize;
	uint32_t pagesize;
	int numlevels;
	uint16_t flags;
};

#define FASTMAP_INLINE_BLOCK	0x01

struct fastmap_outhandle_t
{
	fastmap_handle_t handle;
	struct levelinfo
	{
		size_t currentpage;
		size_t nkeys;
	}[FASTMAP_MAXLEVELS];
	int fd;
};


int fastmap_attr_init(fastmap_attr_t *attr)
{
	return fastmap_attr_destroy(attr);
}

int fastmap_attr_destroy(fastmap_attr_t *attr)
{
	if (!attr)
		return EINVAL;

	memset(attr, 0, sizeof(*attr));
	return FASTMAP_OK;
}

int fastmap_attr_setelements(fastmap_attr_t *attr, const size_t nelements)
{
	attr->elements = nelements;
	return FASTMAP_OK;
}

int fastmap_attr_getelements(fastmap_attr_t *attr, size_t *nelements)
{
	*nelements = attr->elements;
	return FASTMAP_OK;
}

int fastmap_attr_setksize(fastmap_attr_t *attr, const size_t size)
{
	attr->ksize = size;
	return FASTMAP_OK;
}

int fastmap_attr_getksize(fastmap_attr_t *attr, size_t *size)
{
	*size = attr->ksize;
	return FASTMAP_OK;
}

int fastmap_attr_setvsize(fastmap_attr_t *attr, const size_t size)
{
	attr->vsize = size;
	return FASTMAP_OK;
}

int fastmap_attr_getvsize(fastmap_attr_t *attr, size_t *size)
{
	*size = attr->vsize;
	return FASTMAP_OK;
}

int fastmap_attr_setformat(fastmap_attr_t *attr, const fastmap_format_t format)
{
	attr->format = format;
	return FASTMAP_OK;
}

int fastmap_attr_getformat(fastmap_attr_t *attr, fastmap_format_t *format)
{
	*format = attr->format;
	return FASTMAP_OK;
}

int fastmap_outhandle_init(fastmap_outhandle_t *ohandle, const fastmap_attr_t *attr, const char *pathname)
{
	struct stat st;
	int rc = 0;

	if (!ohandle)
		return EINVAL;

	memset(ohandle, 0, sizeof(*ohandle));
	memcpy(ohandle->attr, attr, sizeof(*attr));

	ohandle->fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC);
	if (ohandle->fd == -1)
	{
		rc = errno;
		goto fail;
	}

	if (fstat(ohandle->fd, &st) == -1)
	{
		rc = errno;
		goto fail;
	}

	ohandle->handle->bptrsize = sizeof(st.st_size);
	ohandle->handle->pagesize = (uint32_t)st.st_blksize;

	switch (ohandle->attr->format)
	{
	case FASTMAP_ATOM:
		ohandle->handle->leafnodeitemsize = ohandle->attr->ksize;
		break;
	case FASTMAP_PAIR:
		ohandle->handle->leafnodeitemsize = ohandle->attr->ksize * 2;
		break;
	case FASTMAP_BLOCK:
		if (((ohandle->attr->vsize + (ohandle->handle->pagesize - 1)) & ~(ohandle->handle->pagesize - 1)) > ohandle->attr->ksize)
		{
			ohandle->handle->leafnodeitemsize = ohandle->attr->ksize;
			ohandle->handle->flags |= FASTMAP_INLINE_BLOCKS;
		}
		else
			ohandle->handle->leafnodeitemsize = ohandle->attr->ksize + ohandle->attr->vsize;

		break;
	case FASTMAP_BLOB:
		ohandle->handle->leafnodeitemsize = ohandle->attr->ksize + ohandle->attr->bptrsize;
		break;
	default:
		rc = EINVAL;
		goto fail;
	}

	ohandle->handle->itemsperleafnode = ohandle->handle->pagesize / ohandle->handle->leafnodeitemsize;
	ohandle->handle->leafnodes = (size_t)ceil((double)ohandle->attr->elements / (double)ohandle->handle->itemsperleafnode);

	/* TODO: branching factor, can the size of a search pointer be calculated based on the number of leaf nodes */
	ohandle->handle->branchingfactor = 

fail:
	close(ohandle->fd);
success:
	return rc;
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
