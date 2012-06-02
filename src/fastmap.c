#ifdef HAVE_CONFIG_H #include <fastmap_config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

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
	struct {
		size_t firstoffset;
		size_t pages;
	} perlevel[FASTMAP_MAXLEVELS];
	fastmap_attr_t attr;
	size_t branchingfactor;
	size_t bptrsize;
	size_t elementsperleafnode;
	size_t leafnodes;
	size_t leafnodeitemsize;
	size_t firstvalueoffset;
	uint32_t pagesize;
	int numlevels;
	uint16_t flags;
};

#define FASTMAP_INVALID_MAP	0x01
#define FASTMAP_INLINE_BLOCK	0x02

struct fastmap_outhandle_t
{
	fastmap_handle_t handle;
	struct {
		size_t currentkey;
		size_t currentoffset;
	} levelinfo[FASTMAP_MAXLEVELS];
	size_t currentelement;
	size_t currentleafoffset;
	size_t currentvalueoffset;
	int fd;
};

struct fastmap_inhandle_t
{
	fastmap_handle_t handle;
	fastmap_cmpfunc cmp;
	void *mmapaddr;
	size_t mmaplen;
	int fd;
};

static int fastmap_cmpfunc_memcmp(const fastmap_attr_t *attr, const void *a, const void *b);

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
	int rc = FASTMAP_OK;

	if (ohandle == NULL)
		return EINVAL;

	memset(ohandle, 0, sizeof(*ohandle));
	memcpy(ohandle->handle->attr, attr, sizeof(*attr));
	ohandle->fd = -1;

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

#define IS_MULTIPLE(x,y) (x == ((x / y) * y))

	if (!IS_MULTIPLE(ohandle->handle->pagesize, ohandle->handle->attr->ksize))
	{
		rc = EINVAL;
		goto fail;
	}

	switch (ohandle->handle->attr->format)
	{
	case FASTMAP_ATOM:
		ohandle->handle->leafnodeitemsize = ohandle->handle->attr->ksize;
		break;
	case FASTMAP_PAIR:
		ohandle->handle->leafnodeitemsize = ohandle->handle->attr->ksize * 2;
		break;
	case FASTMAP_BLOCK:
		if (IS_MULTIPLE(ohandle->handle->pagesize, ohandle->handle->attr->ksize + ohandle->handle->attr->vsize))
		{
			ohandle->handle->leafnodeitemsize = ohandle->handle->attr->ksize + ohandle->handle->attr->vsize;
			ohandle->handle->flags |= FASTMAP_INLINE_BLOCKS;
		}
		else
		{
			ohandle->handle->leafnodeitemsize = ohandle->handle->attr->ksize;
		}
		break;
	case FASTMAP_BLOB:
		ohandle->handle->leafnodeitemsize = ohandle->handle->attr->ksize + ohandle->handle->bptrsize;
		break;
	default:
		rc = EINVAL;
		goto fail;
	}

#undef IS_MULTIPLE

	ohandle->handle->elementsperleafnode = ohandle->handle->pagesize / ohandle->handle->leafnodeitemsize;
	ohandle->handle->leafnodes = (size_t)ceil((double)ohandle->handle->attr->elements / (double)ohandle->handle->elementsperleafnode);
	ohandle->handle->branchingfactor = ((ohandle->handle->pagesize + ohandle->handle->attr->ksize) / ohandle->handle->attr->ksize);

	ohandle->handle->numlevels = 0;
	{
		size_t leafnodesfirstoffset = ohandle->handle->pagesize;
		size_t pagesperlevel = ohandle->handle->leafnodes;
		int i;
		while (pagesperlevel > 1)
		{
			pagesperlevel = (size_t)ceil((double)pagesperlevel / (double)ohandle->handle->branchingfactor);
			ohandle->handle->perlevel[ohandle->handle->numlevels++].pages = pagesperlevel;
			leafnodesfirstoffset += pagesperlevel * ohandle->handle->pagesize;
			if (ohandle->handle->numlevels > FASTMAP_MAXLEVELS)
			{
				rc = FASTMAP_TOO_MANY_LEVELS;
				goto fail;
			}
		}

		ohandle->handle->leafnodesfirstoffset = leafnodesfirstoffset;

		for (i = ohandle->handle->numlevels; i > 0; i--)
		{
			leafnodesfirstoffset -= ohandle->handle->perlevel[i-1].pages * ohandle->handle->pagesize;
			ohandle->handle->perlevel[i-1].firstoffset = leafnodesfirstoffset;
		}
	}

	ohandle->currentleafoffset = ohandle->handle->leafnodesfirstoffset;

	if (ohandle->handle->attr->format == FASTMAP_BLOB || (ohandle->handle->attr->format == FASTMAP_BLOCK && !(ohandle->handle->flags & FASTMAP_INLINE_BLOCKS)))
	{
		ohandle->handle->firstvalueoffset = ohandle->handle->leafnodesfirstoffet + (ohandle->handle->pagesize * ohandle->handle->leafnodes);
		ohandle->currentvalueoffset = ohandle->handle->firstvalueoffset;
	}

	ohandle->handle->flags |= FASTMAP_INVALID_MAP;
	write(ohandle->fd, &(ohandle->handle), sizeof(ohandle->handle));
	lseek(ohandle->fd, ohandle->handle->pagesize, SEEK_SET);

	goto success;
fail:
	if (ohandle->fd != -1 && close(ohandle->fd) == -1)
		rc = errno;
success:
	return rc;
}

int fastmap_outhandle_put(fastmap_outhandle_t *ohandle, const fastmap_element_t *element)
{
	if ((ohandle->currentelement + 1) > ohandle->attr->elements)
		return EINVAL;

	/* TODO: Write element key, and value (possibly including the value pointer) */
	lseek(ohandle->fd, ohandle->currentleafoffset, SEEK_SET);
	write(ohandle->fd, element->key, ohandle->handle->attr->ksize);
	ohandle->currentleafoffset += ohandle->handle->attr->ksize;

	switch (ohandle->handle->attr->format)
	{
	case FASTMAP_PAIR:
		write(ohandle->fd, element->value, ohandle->handle->attr->ksize);
		ohandle->currentleafoffset += ohandle->handle->attr->ksize;
		break;
	case FASTMAP_BLOB:
		lseek(ohandle->fd, ohandle->currentvalueoffset, SEEK_SET);
		write(ohandle->fd, &(element->vsize), sizeof(element->vsize));
		write(ohandle->fd, element->value, element->vsize);
		ohandle->currentvalueoffset += sizeof(element->vsize) + element->vsize;
		break;
	case FASTMAP_BLOCK:
		if (ohandle->handle->flags & FASTMAP_INLINE_BLOCKS)
		{
			write(ohandle->fd, element->value, ohandle->handle->attr->vsize);
			ohandle->currentleafoffset += ohandle->handle->attr->vsize;
		}
		else
		{
			lseek(ohandle->fd, ohandle->currentvalueoffset, SEEK_SET);
			write(ohandle->fd, element->value, ohandle->handle->attr->vsize);
			ohandle->currentvalueoffset += ohandle->handle->attr->vsize;
		}
	}

	ohandle->currentelement++;

	if ((ohandle->currentelement % ohandle->handle->elementsperleafnode) == 0)
	{
		size_t i;

#define ALIGN_TO_NEXT_PAGE(v,p) ((v + (p - 1)) & ~(p - 1))

		ohandle->currentleafoffset = ALIGN_TO_NEXT_PAGE(ohandle->currentleafoffset, ohandle->handle->pagesize);

		for (i = 0; i < ohandle->handle->numlevels; i++)
		{
			lseek(ohandle->fd, ohandle->levelinfo[i].currentoffset, SEEK_SET);
			write(ohandle->fd, element->key, ohandle->handle->attr->ksize);
			ohandle->levelinfo[i].currentoffset += ohandle->handle->attr->ksize;
			ohandle->levelinfo[i].currentkey++;

			if (ohandle->levelinfo[i].currentkey % (ohandle->handle->branchingfactor - 1) != 0)
				break;

			ohandle->levelinfo[i].currentoffset = ALIGN_TO_NEXT_PAGE(ohandle->levelinfo[i].currentoffset, ohandle->handle->pagesize);
		}

#undef ALIGN_TO_NEXT_PAGE

	}

	return FASTMAP_OK;
}

int fastmap_inhandle_init(fastmap_inhandle_t *ihandle, const char *pathname)
{
	struct stat st;
	int rc = FASTMAP_OK;

	if (ihandle == NULL)
		return EINVAL;

	memset(ihandle, 0, sizeof(*ihandle));

	ihandle->fd = open(pathname, O_RDONLY);
	if (ihandle->fd == -1)
	{
		rc = errno;
		goto fail;
	}

	if (fstat(ihandle->fd, &st) == -1)
	{
		rc = errno;
		goto fail;
	}

	if (sizeof(st.st_size) > sizeof(size_t) && st.st_size > SIZE_MAX)
	{
		rc = ENOMEM;
		goto fail;
	}

	ihandle->mmaplen = (size_t)st.st_size;
	ihandle->mmapaddr = mmap(NULL, ihandle->mmaplen, PROT_READ, MAP_SHARED, ihandle->fd, 0);
	if (ihandle->mmapaddr == MAP_FAILED)
	{
		rc = errno;
		goto fail;
	}
	/* TODO: assert version and magic */
	memcpy(&ihandle->handle, ihandle->mmapaddr, sizeof(ihandle->handle));

	fastmap_inhandle_setcmpfunc(ihandle, fastmap_cmpfunc_memcmp);

	goto success;
fail:
	if (close(ihandle->fd) == -1 && errno != EBADF)
		rc = errno;
success:
	return rc;
}

static int fastmap_cmpfunc_memcmp(const fastmap_attr_t *attr, const void *a, const void *b)
{
	return memcmp(a, b, attr->ksize);
}

int fastmap_inhandle_setcmpfunc(fastmap_inhandle_t *ihandle, fastmap_cmpfunc *cmp)
{
	ihandle->cmp = cmp;
	return FASTMAP_OK;
}

int fastmap_inhandle_get(fastmap_inhandle_t *ihandle, fastmap_element_t *element)
{
	size_t currentkey;
	size_t elementindex;
	size_t offset;
	int currentlevel;

	currentlevel = ihandle->handle->numlevels;
	offset = ihandle->handle->pagesize;
	while (currentlevel > 0)
	{
		for (currentkey = 0; currentkey < ihandle->handle->branchingfactor; currentkey++)
		{
			int ord = ihandle->cmp(ihandle->handle->attr, element->atom->key, (char*)ihandle->mmapaddr + offset);
			if (ord > 0)
			{
				offset += ihandle->handle->attr->ksize;
			}
			else
			{
				if (currentlevel == 1)
				{
					offset = ihandle->handle->leafnodesoffset;
				}
				else
				{
					offset = ihandle->handle->perlevel[currentlevel - 1].firstoffset;
				}

				offset += ((ord < 0) ? currentkey : (currentkey + 1)) * ihandle->handle->pagesize;
				break;
			}
		}
		currentlevel--;
	}

	elementindex = ((offset - ihandle->handle->leafnodeoffset) / ohandle->handle->pagesize) / ohandle->handle->elementsperleafnode;

	for (currentkey = 0; currentkey < ihandle->handle->elementsperleafnode; currentkey++)
	{
		int ord = ihandle->cmp(ihandle->handle->attr, element->atom->key, (char*)ihandle->mmapaddr + offset);

		if (ord > 0)
		{
			offset += ihandle->handle->leafnodeitemsize;
			continue;
		}
		else if (ord == 0)
		{
			elementindex += currentkey;
			switch (ihandle->attr->format)
			{
			case FASTMAP_PAIR:
				element->pair->value = (void*)((char*)ihandle->mmapaddr + offset + ihandle->attr->ksize);
				break;
			case FASTMAP_BLOCK:
				if (ihandle->handle->flags & FASTMAP_INLINE_BLOCK)
					element->block->value = (void*)((char*)ihandle->mmapaddr + offset + ihandle->attr->ksize);
				else
					element->block->value = (void*)((char*)ihandle->mmapaddr + ihandle->handle->firstvalueoffset + (elementindex * ihandle->handle->attr->vsize);
				break;
			case FASTMAP_BLOB:
				memcpy(&offset, (char*)ihandle->mmapaddr + offset + ihandle->handle->attr->ksize, sizeof(offset));
				memcpy(&(element->blob->vsize), (char*)ihandle->mmapaddr + offset, sizeof(element->blob->vsize));
				element->pair->value = (void*)((char*)ihandle->mmapaddr + sizeof(element->blob->vsize));
			}
			return FASTMAP_OK;
		}
		else
		{
			break;
		}
	}

	return FASTMAP_NOT_FOUND;
}
