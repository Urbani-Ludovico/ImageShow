
#ifndef FILES_H
#define FILES_H

typedef struct File_ {
    char* path;
    char* title;
} File;

int get_files();
void free_files();

#endif
