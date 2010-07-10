#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "writing.h"

// 1 on success, 0 on failure
int editor_write(char *tmp, char *buf)
{
	// construct vim command
	int len = 4 + strlen(tmp);
	char cmd[len];
	snprintf(cmd, len, "vim %s", tmp);

	// edit
	system(cmd);

	// read tmp file
	if (editor_read(tmp, &buf) > 0) {
		// delete tmp file
		snprintf(cmd, len, "rm %s", tmp);
		system(cmd);
		return 1;
	}
	return 0;
}

// Reads a file, returns number of bytes read, -1 or -2 on failure
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
		return -2;
	}
	fclose(f);
	*(buf + size) = 0;
	return size;
}
