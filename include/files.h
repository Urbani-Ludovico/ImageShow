
#ifndef FILES_H
#define FILES_H

typedef struct File_ {
    char* path;
    char* title;
    struct File_* next;
} FilesNode;

typedef struct Files_ {
    FilesNode* files;
    FilesNode* seen;
    FilesNode* seen_last;
    unsigned int count;
    unsigned int seen_count;
} Files;

int get_files();
void free_files();

#endif
