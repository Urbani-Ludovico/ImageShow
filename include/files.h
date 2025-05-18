
#ifndef FILES_H
#define FILES_H

typedef struct File_ {
    char* path;
    struct File_* next;
} FilesNode;

typedef struct Files_ {
    FilesNode* files;
    FilesNode* seen;
    FilesNode* seen_last;
    unsigned int count;
    unsigned int seen_count;
} Files;

int get_files(Files** files_out);
void free_files(Files* files);

#endif
