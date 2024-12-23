#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct to hold a list of files for a specific extension
typedef struct {
    char *extension;
    char **files;
    size_t count;
    size_t capacity;
} FileGroup;

void add_file(FileGroup *group, const char *filename) {
    if (group == NULL || filename == NULL) {
        return;  // Or handle error appropriately
    }
    if (group->count == group->capacity) {
        group->capacity = group->capacity == 0 ? 10 : group->capacity * 2;
        char **new_files =
            realloc(group->files, group->capacity * sizeof(char *));
        if (new_files == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }
        group->files = new_files;
    }
    char *new_filename = strdup(filename);
    if (new_filename == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    group->files[group->count++] = new_filename;
}

int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int compare_groups(const void *a, const void *b) {
    const FileGroup *groupA = a;
    const FileGroup *groupB = b;
    return strcmp(groupA->extension, groupB->extension);
}

void free_file_group(const FileGroup *group) {
    if (group == NULL) return;
    for (size_t i = 0; i < group->count; i++) {
        free(group->files[i]);
    }
    free(group->files);
    free(group->extension);
}

char *get_file_extension(const char *filename) {
    if (filename == NULL) {
        char *noext = strdup("noext");
        if (noext == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return NULL;
        }
        return noext;
    }
    const char *dot = strrchr(filename, '.');
    char *result = dot ? strdup(dot + 1) : strdup("noext");
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    return result;
}

void list_files_by_extension(const char *directory, bool show_hidden) {
    if (directory == NULL) {
        fprintf(stderr, "Directory path is NULL\n");
        return;
    }

    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    FileGroup *groups = NULL;
    size_t group_count = 0, group_capacity = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name == NULL) {
            fprintf(stderr, "Directory entry name is NULL\n");
            continue;
        }

        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        if (entry->d_type == DT_DIR) {
            continue;
        }

        char *extension = get_file_extension(entry->d_name);
        if (extension == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            continue;
        }

        // Find or create a group for this extension
        FileGroup *group = NULL;
        for (size_t i = 0; i < group_count; i++) {
            if (strcmp(groups[i].extension, extension) == 0) {
                group = &groups[i];
                break;
            }
        }

        if (!group) {
            if (group_count == group_capacity) {
                group_capacity = group_capacity == 0 ? 10 : group_capacity * 2;
                FileGroup *new_groups =
                    realloc(groups, group_capacity * sizeof(FileGroup));
                if (new_groups == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    // Clean up and return
                    for (size_t i = 0; i < group_count; i++) {
                        free_file_group(&groups[i]);
                    }
                    free(groups);
                    free(extension);
                    closedir(dir);
                    return;
                }
                groups = new_groups;
            }
            groups[group_count].extension = extension;
            groups[group_count].files = NULL;
            groups[group_count].count = 0;
            groups[group_count].capacity = 0;
            group = &groups[group_count++];
        } else {
            free(extension);
        }

        if (entry->d_name == NULL) {
            fprintf(stderr, "Directory entry name is NULL\n");
            continue;
        }

        add_file(group, entry->d_name);
    }

    closedir(dir);

    if (groups == NULL) {
        // No files found or all were directories
        return;
    }

    // Sort groups and files within groups
    qsort(groups, group_count, sizeof(FileGroup), compare_groups);
    for (size_t i = 0; i < group_count; i++) {
        qsort(groups[i].files, groups[i].count, sizeof(char *),
              compare_strings);
    }

    // Print results
    for (size_t i = 0; i < group_count; i++) {
        printf("%s:\n", groups[i].extension);
        for (size_t j = 0; j < groups[i].count; j++) {
            printf("- %s\n", groups[i].files[j]);
        }
        printf("\n");
    }

    // Free memory
    for (size_t i = 0; i < group_count; i++) {
        free_file_group(&groups[i]);
    }
    free(groups);
}

int main(int argc, char *argv[]) {
    if (argv == NULL) {
        fprintf(stderr, "argv is NULL\n");
        return EXIT_FAILURE;
    }

    const char *directory = ".";
    bool show_hidden = false;

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-dir") == 0 && i + 1 < argc) {
            directory = argv[++i];
        } else if (strcmp(argv[i], "-hidden") == 0) {
            show_hidden = true;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    list_files_by_extension(directory, show_hidden);

    return EXIT_SUCCESS;
}
