#ifdef HAVE_CONFIG_H
#include <fastmap_config.h>
#endif

#include <fastmap_types.h>
#include <fastmap.h>

int fastmap_datum_init(fastmap_datum_t *datum, fastmap_datumattr_t *datumattr, void *key, void *value)
{
	int error = fastmap_datum_destroy(datum);
	fastmap_datumattr_copy(datum->attr, datumattr);
	datum->key = key;
	datum->value = value;
	return error;
}

int fastmap_datum_destroy(fastmap_datum_t *datum)
{
	int error = fastmap_datumattr_destroy(datum->attr);
	memset(datum, 0, sizeof(struct fastmap_datum));
	return error;
}
