#ifdef HAVE_CONFIG_H
#include <fastmap_config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fastmap.h>

static void usage(FILE *out)
{
	fprintf(out, "Usage: tofastmap [OPTION]... NUMRECORDS INPUT OUTPUT\n");
	fprintf(out, "Create a fastmap file OUTPUT from INPUT\n");
	fprintf(out, "\n");
	fprintf(out, "Mandatory arguments to long options are mandatory for short options too.\n");
	fprintf(out, "\n");
	fprintf(out, "  -I, --input-format={csv}        specify the format of INPUT (default: csv)\n");
	fprintf(out, "  -O, --output-format={atom,pair,block,blob}\n");
	fprintf(out, "                                  specify the format of OUTPUT (default blob)\n");
	fprintf(out, "\n");
	fprintf(out, "When INPUT is -, read standard input.\n");
	fprintf(out, "Report bugs to " PACKAGE_BUGREPORT "\n");
	fflush(out);
}

#define MAX_FIELD_SIZE 1024000

#define DEFAULT_INPUT_FORMAT INPUT_CSV
#define DEFAULT_OUTPUT_FORMAT FASTMAP_BLOB

struct inputformat
{
	char *name;
	int format;
};

struct outputformat
{
	char *name;
	fastmap_format_t format;
};

int fromcsv(fastmap_attr_t *attr, int infd, const char *pathname)
{
	char buffer[4096], key[4096], value[4096];
	fastmap_record_t record;
	fastmap_outhandle_t ohandle;	
	FILE *f;
	char *token;
	size_t ksize = 0, vsize = 0, lineno;
	int rc = 0;
	fastmap_format_t format;

	fastmap_attr_getformat(attr, &format);

	f = fdopen(infd, "r");
	while (fgets(buffer, sizeof(buffer), f))
	{
		lineno++;
		token = strtok(buffer, ",");

		if (token == NULL)
		{
			fprintf(stderr, "tofastmap[fromcsv]: input line is out of spec:\n");
			fprintf(stderr, "line %d: %s\n", lineno, buffer);
			rc = -1;
			goto leave;
		}

		memcpy(key, token, strlen(token));

		token = buffer + strlen(token) + 1;
		while(*token && *token == ' ')
			token++;

		switch (format)
		{
		case FASTMAP_ATOM:
			record.atom.key = key;
			break;
		case FASTMAP_PAIR:
			if (strlen(token) != ksize)
			{
				fprintf(stderr, "tofastmap[fromcsv]: 'pair' format requires keys and values be the same size\n");
				fprintf(stderr, "line %d: %s\n", lineno, buffer);
				rc = -1;
				goto leave;
			}
			else
			{
				fastmap_attr_setvsize(attr, strlen(token));
			}
			memcpy(value, token, strlen(token));
			record.pair.key = key;
			record.pair.value = value;
			break;
		case FASTMAP_BLOCK:
			if (vsize != 0 && strlen(token) != vsize)
			{
				fprintf(stderr, "tofastmap[fromcsv]: 'block' format requires all values to be the same size\n");
				fprintf(stderr, "line: %d: %s\n", lineno, buffer);
				rc = -1;
				goto leave;
			}
			memcpy(value, token, strlen(token));
			record.block.key = key;
			record.block.value = value;
			break;
		case FASTMAP_BLOB:
			memcpy(value, token, strlen(token));
			record.blob.key = key;
			record.blob.value = value;
			record.blob.vsize = strlen(token);
			break;
		}

		if (ksize == 0)
		{
			ksize = strlen(key);
			fastmap_attr_setksize(attr, ksize);
			fastmap_outhandle_init(&ohandle, attr, pathname);
		}

		fastmap_outhandle_put(&ohandle, &record);
	}

leave:
	fastmap_outhandle_destroy(&ohandle);
	return rc;
}

static int help;

int main(int argc, char *argv[])
{
	enum {
		INPUT_CSV
	};
	struct inputformat inputformats[] = {
		{ "csv", INPUT_CSV }
	};
	struct outputformat outputformats[] = {
		{ "atom", FASTMAP_ATOM },
		{ "pair", FASTMAP_PAIR },
		{ "block", FASTMAP_BLOCK },
		{ "blob", FASTMAP_BLOB }
	};
	char *inputformat = NULL;
	char *outputformat = NULL;
	fastmap_attr_t attr;
	char *inputpathname, *outputpathname;
	size_t nrecords;
	int iformat;
	int i, opt, input, rc;
	fastmap_format_t oformat;

	while (1)
	{
		static struct option longopts[] = {
			{ "input-format", required_argument, NULL, 'I' },
			{ "output-format", required_argument, NULL, 'O' },
			{ "help", no_argument, &help, 1},
			{ 0, 0, 0, 0}
		};

		int option_index;
		if ((opt = getopt_long(argc, argv, "I:O:", longopts, &option_index)) == -1)
			break;

		switch (opt)
		{
			case 'I':
				inputformat = optarg;
				break;
			case 'O':
				outputformat = optarg;
				break;
			default:
				break;
		}
	}

	if (help == 1)
	{
		usage(stdout);
		exit(EXIT_SUCCESS);
	}

	if (argc - optind != 3)
	{
		fprintf(stderr, "tofastmap: you must specify the number of RECORDS and both an INPUT and OUTPUT\n");
		fprintf(stderr, "Try 'tofastmap --help' for more information.\n");
		exit(EXIT_FAILURE);
	}

	nrecords = (size_t)(atol((const char*)(argv[optind])));

	if (inputformat == NULL)
	{
		iformat = DEFAULT_INPUT_FORMAT;
	}
	else
	{
		int validformat = 0;
		for (i = 0; i < (sizeof(inputformats) / sizeof(inputformats[0])); i++)
		{
			if (strncmp(inputformat, inputformats[i].name, strlen(inputformats[i].name)) == 0)
			{
				iformat = inputformats[i].format;
				validformat = 1;
				break;
			}
		}

		if (!validformat)
		{
			fprintf(stderr, "tofastmap: invalid input format '%s'\n", inputformat);
			fprintf(stderr, "Try 'tofastmap --help' for more information.\n");
			exit(EXIT_FAILURE);
		}
	}

	if (outputformat == NULL)
	{
		oformat = DEFAULT_OUTPUT_FORMAT;
	}
	else
	{
		int validformat = 0;
		for (i = 0; i < (sizeof(outputformats) / sizeof(outputformats[0])); i++)
		{
			if (strncmp(outputformat, outputformats[i].name, strlen(outputformats[i].name)) == 0)
			{
				oformat = outputformats[i].format;
				validformat = 1;
				break;
			}
		}

		if (!validformat)
		{
			fprintf(stderr, "tofastmap: invalid output format '%s'\n", outputformat);
			fprintf(stderr, "Try 'tofastmap --help' for more information.\n");
			exit(EXIT_FAILURE);
		}
	}

	fastmap_attr_init(&attr);
	fastmap_attr_setrecords(&attr, nrecords);
	fastmap_attr_setformat(&attr, oformat);

	inputpathname = (char*)(argv[optind + 1]);
	outputpathname = (char*)(argv[optind + 2]);

	if (strcmp(inputpathname, "-") == 0)
	{
		input = STDIN_FILENO;
	}
	else
	{
		input = open(inputpathname, O_RDONLY);
		if (input == -1)
		{
			perror("tofastmap");
			exit(EXIT_FAILURE);
		}	
	}

	switch (iformat)
	{
	case INPUT_CSV:
		rc = fromcsv(&attr, input, outputpathname);
		break;
	}

	close(input);
	return rc ? EXIT_FAILURE : EXIT_SUCCESS;
}
