#ifdef HAVE_CONFIG_H
#include <fastmap_config.h>
#endif

#include <string.h>

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
