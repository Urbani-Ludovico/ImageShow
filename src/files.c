
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
extern char* add_title;

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
    DIR* dir = opendir(base_path);
    if (!dir) {
        fprintf(stderr, "Cannot open directory: %s\n", base_path);
        return EXIT_FAILURE;
    }

    // Iterate over folder entries
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Build absolute path
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);

        // Check if entry is a directory
        struct stat statbuf;
        if (lstat(path, &statbuf) == -1) {
            fprintf(stderr, "Cannot stat file: %s\n", path);
            return EXIT_FAILURE;
        }
        if (S_ISDIR(statbuf.st_mode)) {
            get_files_recursive(path);
            continue;
        }

        // Get last dot index from d_name
        int last_dot_index = -1;
        for (int i = (int)strlen(entry->d_name) - 1; i >= 0; i--) {
            if (entry->d_name[i] == '/') {
                break;
            }
            if (entry->d_name[i] == '.') {
                last_dot_index = i;
            }
        }
        if (last_dot_index > 0) { // Strict greater than 0 to exclude hidden files without extension
            // Check if ext is correct
            char ext[PATH_MAX];
            strcpy(ext, entry->d_name + last_dot_index);
            for (unsigned int i = 0; i < strlen(ext); i++) {
                ext[i] = (char)tolower(ext[i]);
            }
            bool ext_found = false;
            for (int i = 0; i < supported_exts_count; i++) {
                if (strcmp(ext, supported_exts[i]) == 0) {
                    ext_found = true;
                    break;
                }
            }
            if (!ext_found) {
                continue;
            }

            // Add file to chain
            FilesNode* file = malloc(sizeof(FilesNode));
            file->path = strdup(path);
            if (add_title != nullptr && strcmp(add_title, "filename") == 0) {
                file->title = strndup(entry->d_name, last_dot_index);
            } else {
                file->title = nullptr;
            }
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

void free_file(FilesNode* file) {
    if (file->path != nullptr) {
        free(file->path);
    }
    if (file->title != nullptr) {
        free(file->title);
    }
    free(file);
}

void free_files_chain(FilesNode* files) {
    FilesNode* current = files->next;
    files->next = nullptr;
    while (current != nullptr) {
        FilesNode* next = current->next;
        free_file(current);
        current = next;
    }
}

void free_files() {
    if (files.files != nullptr) {
        free_files_chain(files.files);
    }
    if (files.seen != nullptr) {
        free_files_chain(files.seen);
    }
}
