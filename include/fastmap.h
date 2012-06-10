/**
 * @file   fastmap.h
 * @Author Chris Heller (hellertime@gmail.com)
 * @date   May, 2012
 * @brief  Fastmap key-value index file
 *
 */
#ifndef FASTMAP_H
#define FASTMAP_H 1

/** Valid formats */
typedef enum
{
	FASTMAP_ATOM,
	FASTMAP_PAIR,
	FASTMAP_BLOCK,
	FASTMAP_BLOB
} fastmap_format_t;

/** Opaque structure used to specify the parameters of a new fastmap */
struct fastmap_attr_t
{
	size_t records;
	size_t ksize;
	size_t vsize;
	fastmap_format_t format;
};

typedef struct fastmap_attr_t fastmap_attr_t;

/** A callback function used to compare records */
typedef int (*fastmap_cmpfunc)(const fastmap_attr_t *attr, const void *a, const void *b);

#define FASTMAP_MAXLEVELS 32 /* 32 levels allows for 10^64 records (assuming 8-byte keys and 8-byte pointers), plenty of space */

typedef struct fastmap_handle_t
{
	uint32_t magic;
	uint32_t version;
	struct {
		size_t firstoffset;
		size_t lastoffset;
		size_t pages;
	} perlevel[FASTMAP_MAXLEVELS];
	fastmap_attr_t attr;
	size_t keyspersearchpage;
	size_t valueptrsize;
	size_t recordsperleafpage;
	size_t leafpages;
	size_t leafpagerecordsize;
	size_t firstleafpageoffset;
	size_t firstvalueoffset;
	uint32_t pagesize;
	int numlevels;
	uint16_t flags;
} fastmap_handle_t;

/** Opaque structure used in writing a fastmap */
struct fastmap_outhandle_t
{
	fastmap_handle_t handle;
	struct {
		size_t keys;
		size_t currentoffset;
	} levelinfo[FASTMAP_MAXLEVELS];
	size_t records;
	size_t currentleafpageoffset;
	size_t currentvalueoffset;
	int fd;
};

typedef struct fastmap_outhandle_t fastmap_outhandle_t;

/** Opaque structure used in reading a fastmap */
struct fastmap_inhandle_t
{
	fastmap_handle_t handle;
	fastmap_cmpfunc cmp;
	void *mmapaddr;
	size_t mmaplen;
	int fd;
};

typedef struct fastmap_inhandle_t fastmap_inhandle_t;

/** Generic structure for passing keys in and out of a #FASTMAP_ATOM formatted fastmap */
typedef struct fastmap_atom_t
{
	void *key;	/**< address of the key data, the size of the key is specified by #fastmap_attr_setksize() */
} fastmap_atom_t;

/** Generic structure for passing keys and values in and out of a #FASTMAP_PAIR formatted fastmap */
typedef struct fastmap_pair_t
{
	void *key;	/**< address of the key data, the size of the key is specified by #fastmap_attr_setksize() */
	void *value;	/**< address of the value data, the size of the value is taken to be equal to the size of the key */
} fastmap_pair_t;

/** Generic structure for passing keys and values in and out of a #FASTMAP_BLOCK formatted fastmap */
typedef struct fastmap_block_t
{
	void *key;	/**< address of the key data, the size of the key is specified by #fastmap_attr_setksize() */
	void *value;	/**< address of the value data, the size of the value is specified by #fastmap_attr_setvsize() */
} fastmap_block_t;

/** Generic structure for passing keys and values in and out of a #FASTMAP_BLOB formatted fastmap */
typedef struct fastmap_blob_t
{
	void *key;	/**< address of the key data, the size of the key is specified by #fastmap_attr_setksize() */
	void *value;	/**< address of the value data */
	size_t vsize;	/**< sie of the value data */
} fastmap_blob_t;

/** Union of all record structures. Type is specified by #fastmap_attr_settype() */
typedef union
{
	fastmap_atom_t atom;
	fastmap_pair_t pair;
	fastmap_block_t block;
	fastmap_blob_t blob;
} fastmap_record_t;

/** Status codes, range -13000 to -13199 */
#define FASTMAP_OK			0
#define FASTMAP_NOT_FOUND		-13199
#define FASTMAP_EXPECTATION_FAILED	-13198
#define FASTMAP_TOO_MANY_LEVELS		-13197
#define FASTMAP_TOO_MANY_RECORDS	-13196

/** Initialize a fastmap attribute structure.
 * This function sets a #fastmap_attr_t to a sane default state.
 * @param[out] attr An allocated #fastmap_attr_t to be initialized
 * @return A non-zero error value on failure and 0 on success
 */
int fastmap_attr_init(fastmap_attr_t *attr);

/** Destroy a previously initialized attribute structure.
 * The attribute structure must not be used again util passing it to #fastmap_attr_init()
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_attr_destroy(fastmap_attr_t *attr);

/** Set the size of keys in the map
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[in] size The size of keys
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalud parameter was specified</li>
 * </ul>
 */
int fastmap_attr_setksize(fastmap_attr_t *attr, const size_t size);

/** Get the size of keys in the map
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[out] size The size of keys
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_attr_getksize(fastmap_attr_t *attr, size_t *size);

/** Set the size of values in the map
 * This function has no effect unless the map format is #FASTMAP_BLOCK
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[in] size The size of values
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li>EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_attr_setvsize(fastmap_attr_t *attr, const size_t size);

/** Get the size of values in the map
 * The value returned by this function is undefined unless the map format is #FASTMAP_BLOCK
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[out] size The size of values
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li>EINVAL - An invalid parameter was specified</li>
 * </ul>
 */

/** Set the map format
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[in] format The map format
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li>EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_attr_setformat(fastmap_attr_t *attr, const fastmap_format_t format);

/** Get the map format
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[out] format The map format
 * @return A non-zero error value on failure and 0 on succes. Some possible errors are:
 * <ul>
 *   <li>EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_attr_getformat(fastmap_attr_t *attr, fastmap_format_t *format);

/** Set the number of records in the map
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[in] nrecords The number of records
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li>EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_attr_setrecords(fastmap_attr_t *attr, const size_t nrecords);

/** Get the number of records in the map
 * @param[in] attr A #fastmap_attr_t returned by #fastmap_attr_init()
 * @param[out] nrecords The number of records
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li>EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_attr_getrecords(fastmap_attr_t *attr, size_t *nrecords);

/** Create a fastmap write handle.
 * This function may allocate memory inside the passed in #fastmap_outhandle_t.
 * To release this memory and discard the handle, call #fastmap_outhandle_destroy().
 * @param[out] ohandle An allocated #fastmap_outhandle_t to be initialized
 * @return A non-zero error value on failure and 0 on success
 */
int fastmap_outhandle_init(fastmap_outhandle_t *ohandle, const fastmap_attr_t *attr, const char *pathname);

/** Close the underlying fastmap and release any allocated memory in the handle.
 * Calling this function before fully writing all expected records to the handle
 * will result in an error, and will discard the underlying fastmap being written.
 * The handle must not be used again after this call, except in a call to #fastmap_outhandle_init()
 * @param[in] ohandle A #fastmap_outhandle_t returned by #fastmap_outhandle_init()
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> #FASTMAP_EXPECTATION_FAILED - The expectation that all records would be written
 *                before calling this function has not been met.</li>
 *   <li> #FASTMAP_TOO_MANY_LEVELS - The number of branch node levels required to build a
 *                valid fastmap exceeds the value of #FASTMAP_MAXLEVELS.</li>
 * </ul>
 */
int fastmap_outhandle_destroy(fastmap_outhandle_t *ohandle);

/** Add an record into a fastmap.
 * This function stores key/value records in the map. It is required that keys added
 * by this function be added in sorted order.
 * @param[in] ohandle A #fastmap_outhandle_t returned by #fastmap_outhandle_init()
 * @param[in] record A #fastmap_record_t to add to the map
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalid parameter was specified</li>
 *   <li> #FASTMAP_TOO_MANY_ELEMENTS - This map already contains the maximum number of records it can hold</li>
 * </ul>
 */
int fastmap_outhandle_put(fastmap_outhandle_t *ohandle, const fastmap_record_t *record);

/** Add multiple records at once into a fastmap.
 * This function behaves just like calling #fastmap_outhandle_put() multiple times.
 * @param[in] ohandle A #fastmap_outhandle_t returned by #fastmap_outhandle_init()
 * @param[in] records An array of #fastmap_record_t objects to add to the map
 * @param[in] nrecords The size of the 'records' array
 * @return A non-zero error value on failure and 0 on sucess. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalid parameter was specified</li>
 * </li>
 */
int fastmap_outhandle_mput(fastmap_outhandle_t *ohandle, const fastmap_record_t *records[], size_t nrecords);

/** Create a fastmap read handle.
 * This function may allocate memory inside the passed in #fastmap_inhandle_t.
 * To release this memory and discard the handle, call #fastmap_inhandle_destroy().
 * @param[out] ihandle An allocated #fastmap_inhandle_t to be initialized
 * @return A non-zero error value on failure and 0 on success
 */
int fastmap_inhandle_init(fastmap_inhandle_t *ihandle, const char *pathname);

/** Close the underlying fastmap and release any allocated memory in the handle.
 * The handle must not be used again after this call, except in a call to #fastmap_inhandle_init()
 * @params[in] ihandle A #fastmap_inhandle_t returned by #fastmap_inhandle_init()
 * @return A non-zero error value on failure and 0 on success
 */
int fastmap_inhandle_destroy(fastmap_inhandle_t *ihandle);

/** Locate an record in a fastmap.
 * This function locates a value by a key in the map, if the key exists. The 'record'
 * key field is used to search for the value, and is replaced by a pointer into the map.
 * This function does not allocate memory, instead returning pointers directly into the map. 
 * The retrieved data will be undefined if #fastmap_outhandle_get() is called again.
 * To retain the data, copy it out of the map using #fastmap_record_copy().
 * @param[in] ihandle A #fastmap_inhandle_t returned by #fastmap_inhandle_init()
 * @param[in,out] record A #fastmap_record_t used in the search and result
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalid parameter was specified</li>
 *   <li> #FASTMAP_NOT_FOUND - The key was not found in the map</li>
 * </ul>
 */
int fastmap_inhandle_get(fastmap_inhandle_t *ihandle, fastmap_record_t *record);

/** Locate multiple records at once from a fastmap.
 * This function behaved just like calling #fastmap_inhandle_get() multiple times.
 * Rathen than treating a missing key as an error, any input keys that were not 
 * found will be set to NULL.
 * @param[in] ihandle A #fastmap_inhandle_t returned by #fastmap_inhandle_init()
 * @param[in,out] records An array of #fastmap_record_t object used in the search and result
 * @param[in] nrecords The size of the 'records' array
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *  <li> EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_inhandle_mget(fastmap_inhandle_t *ihandle, fastmap_record_t *records[], size_t nrecords);

/** Get the attributes of the fastmap
 * @param[in] ihandle A #fastmap_inhandle_t returned by #fastmap_inhandle_init()
 * @param[out] attr A #fastmap_attr_t to be configured with the current attr
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li> EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_inhandle_getattr(fastmap_inhandle_t *ihandle, fastmap_attr_t *attr);

/** Set a custom comparison function
 * This function will be called when searching for a key in the map.
 * If no comparison function is specified, a byte-wise comparison is used, with shorter
 * items collating before longer ones.
 * @param[in] ihandle A #fastmap_inhandle_t returned by #fastmap_inhandle_init()
 * @param[in] cmp A #fastmap_cmpfunc function
 * @return A non-zero error value on failure and 0 on success. Some possible errors are:
 * <ul>
 *   <li>EINVAL - An invalid parameter was specified</li>
 * </ul>
 */
int fastmap_inhandle_setcmpfunc(fastmap_inhandle_t *ihandle, fastmap_cmpfunc cmp);

#endif /* ! FASTMAP_H */
