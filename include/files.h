
#ifndef FILES_H
#define FILES_H

typedef struct File_ {
    char *path;
} FilesNode;

typedef struct Files_ {
    FilesNode* files;
    FilesNode* seen;
    unsigned int count;
    unsigned int unseen_count;
    unsigned int seen_count;
} Files;

int get_files(Files** files, const char* path);

#endif
