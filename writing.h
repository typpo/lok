#ifndef WRITING_H
#define WRITING_H
int editor_do(char *tmppath, char *defaulttext, char **buf);
long editor_read(char *path, char **buf);
#endif
