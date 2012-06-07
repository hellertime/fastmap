#ifdef HAVE_CONFIG_H 
#include <fastmap_config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include <fastmap.h>

#define FASTMAP_INVALID_MAP	0x01
#define FASTMAP_INLINE_BLOCK	0x02

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

int fastmap_attr_setrecords(fastmap_attr_t *attr, const size_t nrecords)
{
	attr->records = nrecords;
	return FASTMAP_OK;
}

int fastmap_attr_getrecords(fastmap_attr_t *attr, size_t *nrecords)
{
	*nrecords = attr->records;
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

	if (ohandle == NULL || attr == NULL)
		return EINVAL;

	memset(ohandle, 0, sizeof(*ohandle));
	memcpy(&ohandle->handle.attr, attr, sizeof(*attr));
	ohandle->fd = -1;

	ohandle->fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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

	ohandle->handle.valueptrsize = sizeof(ohandle->currentvalueoffset);
	ohandle->handle.pagesize = (uint32_t)st.st_blksize;

	/* TODO: want to change logic on key eval, no multiple restriction, inline blocks (key + val) * n should fit > 95% of a page, or go non-inline */
#define IS_MULTIPLE(x,y) (x == ((x / y) * y))

	if (!IS_MULTIPLE(ohandle->handle.pagesize, ohandle->handle.attr.ksize))
	{
		rc = EINVAL;
		goto fail;
	}

	switch (ohandle->handle.attr.format)
	{
	case FASTMAP_ATOM:
		ohandle->handle.leafpagerecordsize = ohandle->handle.attr.ksize;
		break;
	case FASTMAP_PAIR:
		ohandle->handle.leafpagerecordsize = ohandle->handle.attr.ksize * 2;
		break;
	case FASTMAP_BLOCK:
		if (IS_MULTIPLE(ohandle->handle.pagesize, ohandle->handle.attr.ksize + ohandle->handle.attr.vsize))
		{
			ohandle->handle.leafpagerecordsize = ohandle->handle.attr.ksize + ohandle->handle.attr.vsize;
			ohandle->handle.flags |= FASTMAP_INLINE_BLOCK;
		}
		else
		{
			ohandle->handle.leafpagerecordsize = ohandle->handle.attr.ksize;
		}
		break;
	case FASTMAP_BLOB:
		ohandle->handle.leafpagerecordsize = ohandle->handle.attr.ksize + ohandle->handle.valueptrsize;
		break;
	default:
		rc = EINVAL;
		goto fail;
	}

#undef IS_MULTIPLE

	ohandle->handle.recordsperleafpage = ohandle->handle.pagesize / ohandle->handle.leafpagerecordsize;
	ohandle->handle.leafpages = (size_t)ceil((double)ohandle->handle.attr.records / (double)ohandle->handle.recordsperleafpage);
	ohandle->handle.branchingfactor = ((ohandle->handle.pagesize + ohandle->handle.attr.ksize) / ohandle->handle.attr.ksize);

	ohandle->handle.numlevels = 0;
	{
		size_t firstleafpageoffset = ohandle->handle.pagesize;
		size_t pagesperlevel = ohandle->handle.leafpages;
		int i;
		while (pagesperlevel > 1)
		{
			pagesperlevel = (size_t)ceil((double)pagesperlevel / (double)ohandle->handle.branchingfactor);
			ohandle->handle.perlevel[ohandle->handle.numlevels++].pages = pagesperlevel;
			firstleafpageoffset += pagesperlevel * ohandle->handle.pagesize;
			if (ohandle->handle.numlevels > FASTMAP_MAXLEVELS)
			{
				rc = FASTMAP_TOO_MANY_LEVELS;
				goto fail;
			}
		}

		ohandle->handle.firstleafpageoffset = firstleafpageoffset;

		for (i = 0; i < ohandle->handle.numlevels; i++)
		{
			firstleafpageoffset -= ohandle->handle.perlevel[i].pages * ohandle->handle.pagesize;
			ohandle->handle.perlevel[i].firstoffset = firstleafpageoffset;
			ohandle->levelinfo[i].currentoffset = firstleafpageoffset;
		}
	}

	ohandle->currentleafpageoffset = ohandle->handle.firstleafpageoffset;

	if (ohandle->handle.attr.format == FASTMAP_BLOB || (ohandle->handle.attr.format == FASTMAP_BLOCK && !(ohandle->handle.flags & FASTMAP_INLINE_BLOCK)))
	{
		ohandle->handle.firstvalueoffset = ohandle->handle.firstleafpageoffset + (ohandle->handle.pagesize * ohandle->handle.leafpages);
		ohandle->currentvalueoffset = ohandle->handle.firstvalueoffset;
	}

	ohandle->handle.flags |= FASTMAP_INVALID_MAP;
	write(ohandle->fd, &(ohandle->handle), sizeof(ohandle->handle));
	lseek(ohandle->fd, ohandle->handle.pagesize, SEEK_SET);

	goto success;
fail:
	if (ohandle->fd != -1 && close(ohandle->fd) == -1)
		rc = errno;
success:
	return rc;
}

int fastmap_outhandle_destroy(fastmap_outhandle_t *ohandle)
{
	int rc = FASTMAP_OK;

	if (ohandle == NULL || ohandle->fd == -1)
		return EINVAL;

	if (!(ohandle->handle.flags & FASTMAP_INVALID_MAP))
	{
		rc = EINVAL;
		goto success;
	}

	if (ohandle->currentrecord != ohandle->handle.attr.records)
	{
		rc = FASTMAP_EXPECTATION_FAILED;
		goto success;
	}

	ohandle->handle.flags &= ~FASTMAP_INVALID_MAP;
	lseek(ohandle->fd, 0L, SEEK_SET);
	write(ohandle->fd , &(ohandle->handle), sizeof(ohandle->handle));
	close(ohandle->fd);
	ohandle->fd = -1;
success:
	return rc;
}

int fastmap_outhandle_put(fastmap_outhandle_t *ohandle, const fastmap_record_t *record)
{
	if ((ohandle->currentrecord + 1) > ohandle->handle.attr.records)
		return FASTMAP_TOO_MANY_RECORDS;

	lseek(ohandle->fd, ohandle->currentleafpageoffset, SEEK_SET);
	write(ohandle->fd, record->atom.key, ohandle->handle.attr.ksize);
	ohandle->currentleafpageoffset += ohandle->handle.attr.ksize;

	switch (ohandle->handle.attr.format)
	{
	case FASTMAP_PAIR:
		write(ohandle->fd, record->pair.value, ohandle->handle.attr.ksize);
		ohandle->currentleafpageoffset += ohandle->handle.attr.ksize;
		break;
	case FASTMAP_BLOB:
		write(ohandle->fd, &(ohandle->currentvalueoffset), sizeof(ohandle->currentvalueoffset));
		ohandle->currentleafpageoffset += sizeof(ohandle->currentvalueoffset);
		lseek(ohandle->fd, ohandle->currentvalueoffset, SEEK_SET);
		write(ohandle->fd, &(record->blob.vsize), sizeof(record->blob.vsize));
		write(ohandle->fd, record->blob.value, record->blob.vsize);
		ohandle->currentvalueoffset += sizeof(record->blob.vsize) + record->blob.vsize;
		break;
	case FASTMAP_BLOCK:
		if (ohandle->handle.flags & FASTMAP_INLINE_BLOCK)
		{
			write(ohandle->fd, record->block.value, ohandle->handle.attr.vsize);
			ohandle->currentleafpageoffset += ohandle->handle.attr.vsize;
		}
		else
		{
			lseek(ohandle->fd, ohandle->currentvalueoffset, SEEK_SET);
			write(ohandle->fd, record->block.value, ohandle->handle.attr.vsize);
			ohandle->currentvalueoffset += ohandle->handle.attr.vsize;
		}
	case FASTMAP_ATOM:
		break;
	}

#define ALIGN_TO_NEXT_PAGE(v,p) ((v + (p - 1)) & ~(p - 1))

	if ((ohandle->currentrecord != 0) && (ohandle->currentrecord % ohandle->handle.recordsperleafpage) == 0)
	{
		int i;

		for (i = 0; i < ohandle->handle.numlevels; i++)
		{
			if (ohandle->levelinfo[i].currentkey % (ohandle->handle.branchingfactor - 1) == 0)
			{
				ohandle->levelinfo[i].currentoffset = ALIGN_TO_NEXT_PAGE(ohandle->levelinfo[i].currentoffset, ohandle->handle.pagesize);
			}
		
			lseek(ohandle->fd, ohandle->levelinfo[i].currentoffset, SEEK_SET);
			write(ohandle->fd, record->atom.key, ohandle->handle.attr.ksize);
			ohandle->levelinfo[i].currentoffset += ohandle->handle.attr.ksize;
			ohandle->levelinfo[i].currentkey++;

			if (ohandle->levelinfo[i].currentkey % (ohandle->handle.branchingfactor - 1) != 0)
				break;
		}
	}

	ohandle->currentrecord++;

	if ((ohandle->currentrecord % ohandle->handle.recordsperleafpage) == 0)
	{
		ohandle->currentleafpageoffset = ALIGN_TO_NEXT_PAGE(ohandle->currentleafpageoffset, ohandle->handle.pagesize);
	}

#undef ALIGN_TO_NEXT_PAGE

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

int fastmap_inhandle_destroy(fastmap_inhandle_t *ihandle)
{
	if (ihandle == NULL || ihandle->fd == -1)
		return EINVAL;

	if (ihandle->mmapaddr)
	{
		munmap(ihandle->mmapaddr, ihandle->mmaplen);
		ihandle->mmapaddr = NULL;
	}

	close(ihandle->fd);
	ihandle->fd = -1;
	return FASTMAP_OK;
}

static int fastmap_cmpfunc_memcmp(const fastmap_attr_t *attr, const void *a, const void *b)
{
	return memcmp(a, b, attr->ksize);
}

int fastmap_inhandle_getattr(fastmap_inhandle_t *ihandle, fastmap_attr_t *attr)
{
	memcpy(attr, &(ihandle->handle.attr), sizeof(*attr));
	return FASTMAP_OK;
}

int fastmap_inhandle_setcmpfunc(fastmap_inhandle_t *ihandle, fastmap_cmpfunc cmp)
{
	ihandle->cmp = cmp;
	return FASTMAP_OK;
}

int fastmap_inhandle_get(fastmap_inhandle_t *ihandle, fastmap_record_t *record)
{
	size_t currentkey;
	size_t recordindex;
	size_t offset;
	int currentlevel;

	currentlevel = ihandle->handle.numlevels;
	offset = ihandle->handle.pagesize;
	while (currentlevel > 0)
	{
		for (currentkey = 0; currentkey < ihandle->handle.branchingfactor; currentkey++)
		{
			int ord = ihandle->cmp(&ihandle->handle.attr, record->atom.key, (char*)ihandle->mmapaddr + offset);
			if (ord > 0)
			{
				offset += ihandle->handle.attr.ksize;
			}
			else
			{
				if (currentlevel == 1)
				{
					offset = ihandle->handle.firstleafpageoffset;
				}
				else
				{
					offset = ihandle->handle.perlevel[currentlevel - 1].firstoffset;
				}

				offset += ((ord < 0) ? currentkey : (currentkey + 1)) * ihandle->handle.pagesize;
				break;
			}
		}
		currentlevel--;
	}

	recordindex = ((offset - ihandle->handle.firstleafpageoffset) / ihandle->handle.pagesize) / ihandle->handle.recordsperleafpage;

	for (currentkey = 0; currentkey < ihandle->handle.recordsperleafpage; currentkey++)
	{
		int ord = ihandle->cmp(&ihandle->handle.attr, record->atom.key, (char*)ihandle->mmapaddr + offset);

		if (ord > 0)
		{
			offset += ihandle->handle.leafpagerecordsize;
			continue;
		}
		else if (ord == 0)
		{
			recordindex += currentkey;
			switch (ihandle->handle.attr.format)
			{
			case FASTMAP_PAIR:
				record->pair.value = (void*)((char*)ihandle->mmapaddr + offset + ihandle->handle.attr.ksize);
				break;
			case FASTMAP_BLOCK:
				if (ihandle->handle.flags & FASTMAP_INLINE_BLOCK)
					record->block.value = (void*)((char*)ihandle->mmapaddr + offset + ihandle->handle.attr.ksize);
				else
					record->block.value = (void*)((char*)ihandle->mmapaddr + ihandle->handle.firstvalueoffset + (recordindex * ihandle->handle.attr.vsize));
				break;
			case FASTMAP_BLOB:
				memcpy(&offset, (char*)ihandle->mmapaddr + offset + ihandle->handle.attr.ksize, sizeof(offset));
				memcpy(&(record->blob.vsize), (char*)ihandle->mmapaddr + offset, sizeof(record->blob.vsize));
				record->blob.value = (void*)((char*)ihandle->mmapaddr + offset + sizeof(record->blob.vsize));
			case FASTMAP_ATOM:
				break;
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
