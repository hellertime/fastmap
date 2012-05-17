#ifndef FASTMAP_TYPES_H
#define FASTMAP_TYPES_H 1

struct fastmap_datumattr
{
	size_t ksize;
	size_t vsize;
};

struct fastmap_datum
{
	struct fastmap_datumattr attr;
	void *key;
	void *value;
};

typedef enum
{
	FASTMAP_WRITE_ONLY,
	FASTMAP_READ_ONLY
} fastmap_mode_t;

struct fastmap_attr
{
	fastmap_mode_t mode;
};

struct fastmap
{
	struct fastmap_attr attr;
	char *path;
	int fd;
};

#endif /* ! FASTMAP_TYPES_H */
