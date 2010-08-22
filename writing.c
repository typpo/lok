#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "writing.h"

// 1 on success, 0 on no edit, <0 on failure
int editor_do(char *tmppath, char *defaulttext, char **buf)
{
	// write to tmp
	FILE *f = fopen(tmppath, "w");
	if (!f) {
		printf("Couldn't write file %s\n", tmppath);
		return -1;
	}
	fprintf(f, "%s", defaulttext);
	fclose(f);

	// construct vim command
	int len = 5 + strlen(tmppath);
	char cmd[len];
	snprintf(cmd, len, "vim %s", tmppath);

	// do edit
	system(cmd);

	// read tmppath file
	long size = editor_read(tmppath, buf);
	printf("%ld bytes edited.\n", size);
	if (size < 1) {
		return -1;
	}
	// delete tmppath file
	remove(tmppath);

	// diff
	return strcmp(defaulttext, *buf) != 0;
}

// Reads a file, returns number of bytes read, <0 on failure.
long editor_read(char *path, char **buf)
{
	long size = -1;

	FILE *f = fopen(path, "rb");
	if (!f) {
		perror("Error opening file");
		goto cleanup;
	}
	// get size
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (size < 1) {
		goto cleanup;
	}
	// allocate
	*buf = (char *) malloc(sizeof(char) * (size + 1));
	if (!buf) {
		perror("malloc failed");
		size = -1;
		goto cleanup;
	}
	// load file
	rewind(f);
	if (size != fread(*buf, sizeof(char), size, f)) {
		printf("Expected size of file didn't match.\n");
		free(*buf);
		size = -1;
		goto cleanup;
	}
	// null terminate
	(*buf)[size] = 0;

      cleanup:
	fclose(f);
	return size;
}
