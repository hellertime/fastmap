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
	fprintf(out, "Usage: dumpfastmap [OPTION]... INPUT\n");
	fprintf(out, "Write the description of the fastmap INPUT to standard output\n");
	fprintf(out, "\n");
	fprintf(out, "Mandatory arguments to long options are mandatory for short options too.\n");
	fprintf(out, "\n");
	fprintf(out, "  --help    display this help message\n");
	fprintf(out, "\n");
	fprintf(out, "When INPUT is -, read standard input.\n");
	fprintf(out, "Report bugs to " PACKAGE_BUGREPORT "\n");
	fflush(out);
}

static int help;

int main(int argc, char *argv[])
{
	fastmap_attr_t attr;
	fastmap_inhandle_t ihandle;
	int opt;
	char *pathname;

	while (1)
	{
		static struct option longopts[] = {
			{ "help", no_argument, &help, 1},
			{ 0, 0, 0, 0}
		};

		int option_index;
		if ((opt = getopt_long(argc, argv, "", longopts, &option_index)) == -1)
			break;

		switch (opt)
		{
			default:
				break;
		}
	}

	if (help == 1)
	{
		usage(stdout);
		exit(EXIT_SUCCESS);
	} 
	if (argc - optind != 1)
	{
		fprintf(stderr, "dumpfastmap: you must specify then INPUT\n");
		fprintf(stderr, "Try 'dumpfastmap --help' for more information.\n");
		exit(EXIT_FAILURE);
	}

	pathname = argv[optind];

	fastmap_inhandle_init(&ihandle, pathname);
	fastmap_inhandle_getattr(&ihandle, &attr);

	puts("{ \"fastmap\":");
	puts("  { \"handle\":");
	puts("     {");
	fprintf(stdout, "      \"numlevels\": %d,\n", ihandle.handle.numlevels);
	fprintf(stdout, "      \"pagesize\": %d,\n", ihandle.handle.pagesize);
	puts("      \"attr\":");
	puts("        {");
	fprintf(stdout, "        \"records\": %d,\n", ihandle.handle.attr.records);
	fprintf(stdout, "        \"ksize\": %d,\n", ihandle.handle.attr.ksize);
	if (ihandle.handle.attr.format == FASTMAP_BLOCK)
		fprintf(stdout, "        \"vsize\": %d,\n", ihandle.handle.attr.vsize);
	else
		fprintf(stdout, "        \"vsize\": N/A,\n");
	switch (ihandle.handle.attr.format)
	{
		case FASTMAP_ATOM:
			puts("        \"format\": \"atom\",");
			break;
		case FASTMAP_PAIR:
			puts("        \"format\": \"pair\",");
			break;
		case FASTMAP_BLOCK:
			if (ihandle.handle.flags & 0x02)
				puts("        \"format\": \"block (inline)\",");
			else
				puts("        \"format\": \"block\",");
			break;
		case FASTMAP_BLOB:
			puts("        \"format\": \"blob\",");
			break;
		default:
			puts("        \"format\": null,");
			break;
	}
	puts("        }");
	puts("      }");
	puts("  }");
	puts("}");
}
