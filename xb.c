#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
	FILE *src, *dst;
	int c;
	unsigned char b;
	int i;
	int skip;

	argc--;
	argv++;
	if (!argc)
		goto usage;
	src = fopen(*argv, "rb");
	if (!src) {
		perror(*argv);
		return 1;
	}
	argc--;
	argv++;
	dst = fopen(argc ? *argv : "a.out", "wb");
	if (!dst) {
		perror(argc ? *argv : "a.out");
		return 1;
	}

	b = skip = 0;
	i = 1;
	while ((c = fgetc(src)) != EOF) {
		if (c == '\n') {
			skip = 0;
			continue;
		}
		if (skip)
			continue;
		if (c == ';') {
			skip = 1;
			continue;
		}
		if (c == ' ' || c == '\t')
			continue;
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'a' && c <= 'f')
			c = c - 'a' + 10;
		else {
			fprintf(stderr, "bad char '%c'\n", c);
			return 1;
		}
		b = b | ((char)c << (4 * i));
		if (i)
			i--;
		else {
			if (fwrite(&b, 1, 1, dst) != 1) {
				perror("fwrite");
				return 1;
			}
			i = 1;
			b = 0;
		}
	}
	if (!i) {
		fprintf(stderr, "trailing nibble '%hhx'\n", b >> 4);
		return 1;
	}
	return 0;
usage:
	fprintf(stderr, "usage: xb SRC [DST]\n");
	return 1;
}
