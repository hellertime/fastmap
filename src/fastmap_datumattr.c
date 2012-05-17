#ifdef HAVE_CONFIG_H
#include <fastmap_config.h>
#endif

#include <fastmap_types.h>
#include <fastmap.h>

int fastmap_datumattr_init(fastmap_datumattr_t *datumattr)
{
	int error = fastmap_datumattr_destroy(datumattr);
	return error;
}

int fastmap_datumattr_destroy(fastmap_datumattr_t *datumattr)
{
	memset(datumattr, 0, sizeof(struct fastmap_datumattr));
}
