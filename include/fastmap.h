#ifndef FASTMAP_H
#define FASTMAP_H 1

/**
 * @file   fastmap.h
 * @Author Chris Heller (hellertime@gmail.com)
 * @date   May, 2012
 * @brief  Fastmap key-value index file
 *
 */

typedef struct fastmap fastmap_t;
typedef struct fastmap_attr fastmap_attr_t;
typedef struct fastmap_datumattr fastmap_datumattr_t;
typedef struct fastmap_datum fastmap_datum_t;

/**
 * Initialize fastmap attributes object
 *
 * Calling 'fastmap_attr_init' on a previously initialized 'fastmap_attr_t' is undefined
 *
 * @param[out] attr a newly initialized 'fastmap_attr_t'
 */
int fastmap_attr_init(fastmap_attr_t *attr);

/**
 * Uninitialize fastmap attributes object
 *
 * A 'fastmap_attr_t' can be re-initialized with 'fastmap_attr_init'
 *
 * @param[in] attr a previously initialized 'fastmap_attr_t'
 */
int fastmap_attr_destroy(fastmap_attr_t *attr);

/**
 * Copy the fields of 'fastmap_attr_t' 'src' to 'dst'
 *
 * @param[in]  src source 'fastmap_attr_t' to copy from
 * @param[out] dst destination 'fastmap_attr_t' to copy to
 */
int fastmap_attr_copy(fastmap_attr_t *dst, fastmap_attr_t *src);


/**
 * Initialize fastmap datum attributes object
 *
 * Calling 'fastmap_datumattr_init' on a previously initialized 'fastmap_datumattr_t' is undefined
 *
 * @param[out] datumattr a newly initialized 'fastmap_datumattr_t'
 */
int fastmap_datumattr_init(fastmap_datumattr_t *datumattr);

/**
 * Uninitialize fastmap datum attributes object
 *
 * A 'fastmap_datumattr_t' can be re-initialized with 'fastmap_datumattr_t'
 *
 * @param[in] datumattr a previously initialized 'fastmap_datumattr_t'
 */
int fastmap_datumattr_destroy(fastmap_datumattr_t *datumattr);

/**
 * Copy the fields of 'fastmap_datumattr_t' 'src' to 'dst'
 *
 * @param[in]  src source 'fastmap_datumattr_t' to copy from
 * @param[out] dst destination 'fastmap_datumattr_t' to copy to
 */
int fastmap_datumattr_copy(fastmap_datumattr_t *dst, fastmap_datumattr_t *src);

/**
 * Initialize fastmap datum object
 *
 * Calling 'fastmap_datum_init' on a previously initialized 'fastmap_datum_t' is undefined
 *
 * @param[in]  datumattr an initialized and configured 'fastmap_datumattr_t' to associate with the 'fastmap_datum_t'
 * @param[in]  key the fastmap datum key
 * @param[in]  value the fastmap datum value
 * @param[out] datum an initialized fastmap datum object
 */
int fastmap_datum_init(fastmap_datum_t *datum, fastmap_datumattr_t *datumattr, void *key, void *value);

/**
 * Uninitialize fastmap datum object
 *
 * A 'fastmap_datum_t' can be re-initialized with 'fastmap_datum_init'
 *
 * @param[in] datum a previously initialized 'fastmap_datum_t'
 */
int fastmap_datum_destroy(fastmap_datum_t *datum);

/**
 * Create a new writeable fastmap file or rewrite an existing one
 *
 * @param[in]  attr an initialized and configured 'fastmap_attr_t' to associate with the 'fastmap_t'
 * @param[in]  path 
 * @param[out] fm   a handle to the created fastmap
 */
int fastmap_create(fastmap_t *fm, const fastmap_attr_t *attr, const char *path);

/**
 * Open an existing fastmap for reading
 *
 * @param[in]  path
 * @param[out] fm   a handle to the opend fastmap
 */
int fastmap_open(fastmap_t *fm, const char *path);

/**
 * Close a fastmap handle
 *
 * @param[in] fm a 'fastmap_t' handle
 */
int fastmap_close(fastmap_t *fm);

/**
 * Retrieve datum from the fastmap
 *
 * @param[in]  fm
 * @param[out] datum the retrieved datum
 */
int fastmap_get(fastmap_t *fm, fastmap_datum_t *datum);

/**
 * Retrieve data from the fastmap
 *
 * @param[in]  fm
 * @param[out] data the retrieved data
 */
int fastmap_get_many(fastmap_t *fm, fastmap_datum_t *data[], size_t how_many);

/**
 * Store datum in the fastmap
 *
 * This must be done in sorted order, otherwise it is an error
 * The 'fastmap_datum_t' object passed to 'fastmap_put' is destroyed
 */
int fastmap_put(fastmap_t *fm, fastmap_datum_t *datum);

/**
 * Store data in the fastmap
 *
 * The data must be sorted
 * Each 'fastmap_datum_t' object passed to 'fastmap_put_many' is destroyed
 */
int fastmap_put_many(fastmap_t *fm, fastmap_datum_t *data[], size_t how_many);

#endif /* ! FASTMAP_H */
