
#include "files.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int get_files_recursive(Files* files, const char* base_path);
bool is_directory(const char* path);


int get_files(Files** files_out, const char* base_path) {
    Files* files = malloc(sizeof(Files));
    files->files = nullptr;
    files->files_last = nullptr;
    files->seen = nullptr;
    files->seen_last = nullptr;
    files->count = 0;
    files->seen_count = 0;

    get_files_recursive(files, base_path);

    *files_out = files;
    return EXIT_SUCCESS;
}

int get_files_recursive(Files* files, const char* base_path) {
    struct dirent *entry;
    struct stat statbuf;
    char path[PATH_MAX];

    DIR* dir = opendir(base_path);
    if (!dir) {
        fprintf(stderr, "Cannot open directory: %s\n", base_path);
        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);

        if (lstat(path, &statbuf) == -1) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            get_files_recursive(files, path);
        } else {
            FilesNode* file = malloc(sizeof(FilesNode));
            file->path = strdup(path);
            file->next = nullptr;

            if (files->files == nullptr) {
                files->files = file;
                files->files_last = file;
            } else {
                files->files_last->next = file;
                files->files_last = file;
            }
            files->count++;
        }
    }

    closedir(dir);
    return EXIT_SUCCESS;
}

bool is_directory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return false;
    }
    return S_ISDIR(statbuf.st_mode);
}

inline void free_files(Files* files) {
    while (files->files != nullptr) {
        FilesNode* tmp = files->files;
        files->files = files->files->next;
        free(tmp->path);
        free(tmp);
    }
    while (files->seen != nullptr) {
        FilesNode* tmp = files->seen;
        files->seen = files->seen->next;
        free(tmp->path);
        free(tmp);
    }
    free(files);
}