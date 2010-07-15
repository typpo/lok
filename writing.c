#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "writing.h"

// 1 on success, 0 on no edit, <0 on failure
int editor_do(char *tmppath, char *defaulttext, char *buf)
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
	if (editor_read(tmppath, &buf) > 0) {
		// delete tmppath file
        return remove(tmppath) == 0;
	}
	return 0;
}

// Reads a file, returns number of bytes read, <0 on failure.
int editor_read(char *path, char **buf)
{
	FILE *f = fopen(path, "r");
	if (!f) {
		return -1;
	}

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*buf = (char *) malloc(size + 1);
	if (size != fread(*buf, sizeof(char), size, f)) {
		free(*buf);
		return -1;
	}
	fclose(f);
	*(buf + size) = 0;
	return size;
}
