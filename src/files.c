
#include "files.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

Files files = {nullptr, nullptr, nullptr, 0, 0};

const char* supported_exts[] = {".png", ".jpg", ".jpeg", ".gif", ".bmp", ".tiff", ".tif", ".ico", ".svg", ".webp"};
constexpr int supported_exts_count = 10;

extern char* source_path;

int get_files_recursive(const char* base_path);
bool is_directory(const char* path);


int get_files() {
    if (source_path == NULL) {
        fprintf(stderr, "No source path provided\n");
        return EXIT_FAILURE;
    }

    get_files_recursive(source_path);

    if (files.count == 0) {
        fprintf(stderr, "No files found\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int get_files_recursive(const char* base_path) {
    struct dirent* entry;
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
            get_files_recursive(path);
        } else {
            const char* last_dot = strrchr(path, '.');
            if (last_dot != NULL) {
                char ext[PATH_MAX];
                strcpy(ext, last_dot);
                for (unsigned int i = 0; i < strlen(ext); i++) {
                    ext[i] = tolower(ext[i]);
                }

                bool found = false;
                for (int i = 0; i < supported_exts_count; i++) {
                    if (strcmp(last_dot, supported_exts[i]) == 0) {
                        found = true;
                        break;
                    }
                }

                if (found) {
                    FilesNode* file = malloc(sizeof(FilesNode));
                    file->path = strdup(path);
                    file->next = file;

                    if (files.files == nullptr) {
                        files.files = file;
                    } else {
                        file->next = files.files->next;
                        files.files->next = file;
                    }
                    files.count++;
                }
            }
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


inline void free_files() {
    if (files.files != nullptr) {
        FilesNode* file = files.files;
        files.files = file->next;
        file->next = nullptr;
    }
    while (files.files != nullptr) {
        FilesNode* tmp = files.files;
        files.files = files.files->next;
        free(tmp->path);
        free(tmp);
    }
    if (files.seen != nullptr) {
        FilesNode* file = files.seen;
        files.seen = file->next;
        file->next = nullptr;
    }
    while (files.seen != nullptr) {
        FilesNode* tmp = files.seen;
        files.seen = files.seen->next;
        free(tmp->path);
        free(tmp);
    }
}
