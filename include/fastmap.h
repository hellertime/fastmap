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
typedef struct fastmap_attr_serialized fastmap_attr_serialized_t;
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
int fastmap_attr_copy(fastmap_attr_t *dst, const fastmap_attr_t *src);

/**
 * Set the 'elements' attribute
 *
 * @param[in] attr 'fastmap_attr_t' object being updated
 * @param[in] elements updated 'elements' attribute
 */
int fastmap_attr_setelements(fastmap_attr_t *attr, size_t elements);

/**
 * Get the 'elements' attribute
 *
 * @param[in]  attr 'fastmap_attr_t' object being queried
 * @param[out] elements will be set to the current value
 */
int fastmap_attr_getelements(fastmap_attr_t *attr, size_t *elements); 

/**
 * Set the 'fixedvsize' attribute
 *
 * @param[in] attr 'fastmap_attr_t' object being updated
 * @param[in] fixedvsize updated 'fixedvsize' attribute
 */
int fastmap_attr_setfixedvsize(fastmap_attr_t *attr, size_t fixedvsize);

/**
 * Get the 'fixedvsize' attribute
 *
 * @param[in]  attr 'fastmap_attr_t' object being queried
 * @param[out] fixedvsize will be set to the current value
 */
int fastmap_attr_getfixedvsize(fastmap_attr_t *attr, size_t *fixedvsize); 

/**
 * Set the 'ksize' attribute
 *
 * @param[in] attr 'fastmap_attr_t' object being updated
 * @param[in] ksize updated 'ksize' attribute
 */
int fastmap_attr_setksize(fastmap_attr_t *attr, size_t ksize);

/**
 * Get the 'ksize' attribute
 *
 * @param[in]  attr 'fastmap_attr_t' object being queried
 * @param[out] ksize will be set to the current value
 */
int fastmap_attr_getksize(fastmap_attr_t *attr, size_t *ksize); 

/**
 * Set the 'mode' attribute
 *
 * @param[in] attr 'fastmap_attr_t' object being updated
 * @param[in] mode updated 'mode' attribute
 */
int fastmap_attr_setmode(fastmap_attr_t *attr, fastmap_mode_t mode);

/**
 * Get the 'mode' attribute
 *
 * @param[in]  attr 'fastmap_attr_t' object being queried
 * @param[out] mode will be set to the current value
 */
int fastmap_attr_getmode(fastmap_attr_t *attr, fastmap_mode_t *mode); 

/**
 * Set the 'type' attribute
 *
 * @param[in] attr 'fastmap_attr_t' object being updated
 * @param[in] type updated 'type' attribute
 */
int fastmap_attr_settype(fastmap_attr_t *attr, fastmap_type_t type);

/**
 * Get the 'type' attribute
 *
 * @param[in]  attr 'fastmap_attr_t' object being queried
 * @param[out] type will be set to the current value
 */
int fastmap_attr_gettype(fastmap_attr_t *attr, fastmap_type_t *type); 

/**
 * Serialized a 'fastmap_attr_t' object
 *
 * @param[in]  attr 'fastmap_attr_t' object to serialize
 * @param[out] sattr 'fastmap_attr_serialized_t' object being serialized into
 */
int fastmap_attr_serialize(fastmap_attr_serialized_t *sattr, const fastmap_attr_t *attr);

/**
 * Deserialize a 'fastmap_attr_serialized_t' object
 *
 * @param[in]  sattr 'fastmap_attr_serialized_t' object to deserialize
 * @param[out] attr 'fastmap_attr_t' object to populate
 */
int fastmap_attr_deserialize(fastmap_attr_t *attr, const fastmap_attr_serialized_t *sattr);

/**
 * Read a 'fastmap_attr_serialized_t' object from a file descriptor
 *
 * @param[in]  fd the descriptor being read from
 * @param[out] sattr the 'fastmap_attr_serialized_t' object to populate
 */
ssize_t fastmap_attr_serialized_read(int fd, fastmap_attr_serialized_t *sattr);

/**
 * Write a 'fastmap_attr_serialized_t' object to a file descriptor
 *
 * @param[in]  sattr the 'fastmap_attr_serialized_t' object to write
 * @param[out] fd the descriptor being written to
 */
ssize_t fastmap_attr_serialized_write(int, const fastmap_attr_serialized_t *sattr);

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
int fastmap_datumattr_copy(fastmap_datumattr_t *dst, const fastmap_datumattr_t *src);

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

/** Create a fastmap write handle.
 * This function may allocate memory inside the passed in #fastmap_outhandle_t.
 * To release this memory and discard the handle, call #fastmap_outhandle_destroy().
 * @param[out] ohandle An allocated #fastmap_ohandle_t to be initialized
 * @return A non-zero error value on failure and 0 on success
 */
int fastmap_outhandle_init(fastmap_outhandle_t *ohandle, const fastmap_attr_t *attr, const char *pathname);

/** Close the underlying fastmap and release any allocated memory in the handle.
 * Calling this function before fully writing all expected elements to the handle
 * will result in an error, and will discard the underlying fastmap being written.
 * The handle must not be used again after this call, except in a call to #fastmap_outhandle_init()
 * @param[in] ohandle A #fastmap_outhandle_t returned by #fastmap_outhandle_init()
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> #FASTMAP_EXPECTATION_FAILED - The expectation that all elements would be written
 *                before calling this function has not been met.</li>
 * </ul>
 */
int fastmap_outhandle_destroy(fastmap_outhandle_t *ohandle);

/** Add an element into a fastmap.
 * This function stores key/value elements in the map. It is required that keys added
 * by this function be added in sorted order.
 * @param[in] ohandle A #fastmap_outhandle_t returned by #fastmap_outhandle_init()
 * @param[in] element A #fastmap_element_t to add to the map
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> #FASTMAP_EXPECTATION_FAILED - The expectation that elements would be written
 *                in sorted order has not been met.</li>
 *   <li> EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_outhandle_put(fastmap_outhandle_t *ohandle, const fastmap_element_t *element);

/** Add multiple elements at once into a fastmap.
 * This function behaves just like calling #fastmap_outhandle_put() multiple times.
 * @param[in] ohandle A #fastmap_outhandle_t returned by #fastmap_outhandle_init()
 * @param[in] elements An array of #fastmap_element_t objects to add to the map
 * @param[in] numelements The size of the 'elements' array
 * @return A non-zero error value on failure and 0 on sucess. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalid parameter was specified</li>
 * </li>
 */
int fastmap_outhandle_mput(fastmap_outhandle_t *ohandle, const fastmap_element_t *elements[], size_t numelements);
#endif /* ! FASTMAP_H */
