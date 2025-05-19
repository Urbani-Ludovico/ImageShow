
#ifndef FILES_H
#define FILES_H

typedef struct File_ {
    char* path;
    char* title;
    struct File_* next;
} FilesNode;

typedef struct Files_ {
    FilesNode* files;
    unsigned int count;
} Files;

int get_files();
void free_files();
void shuffle_files();

#endif
