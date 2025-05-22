
#ifndef FILES_H
#define FILES_H
#include "window.h"

typedef struct File_ {
    char* path;
    char* title;
} File;

int get_files();
void free_files();

void shuffle_window_files(const WindowData* window_data);

#endif
