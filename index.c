#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "headers/constants.h"

void read_line(const char * line)
{
    for (int i = 0; i < strlen(line); i++)
    {
        printf("%c", line[i]);
    }
}

void process_file(const char *filepath)
{
    printf("\nProcessing file: %s\n\n", filepath);

    FILE    *file;

    char    *line       = NULL;
    size_t  line_size   = 0;
    ssize_t line_length;
    
    file = fopen(filepath, "r");

    if (!file)
    {
        perror("\nError opening file");
        
        return;
    }
    
    while ((line_length = getline(&line, &line_size, file)) != -1)
    {
        read_line(line);
    }

    printf("\n");
    
    free(line);

    fclose(file);
}

void traverse_dir(const char *dirpath)
{
    DIR           *dir;
    struct dirent *entry;
    struct stat   statbuf;
    char          filepath[MAX_PATH_LENGTH];

    dir = opendir(dirpath);

    if (!dir)
    {
        printf("\nError: failed to open directory %s\n", dirpath);
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue; // Skip hidden files
        }

        snprintf(filepath, MAX_PATH_LENGTH, "%s/%s", dirpath, entry->d_name);

        if (stat(filepath, &statbuf) == -1)
        {
            printf("\nError: failed to get file status for %s\n", filepath);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode))
        {
            traverse_dir(filepath); // If stat mode is a directory recurse into subdirectories
        }
        else if (S_ISREG(statbuf.st_mode))
        {
            char *ext = strrchr(entry->d_name, '.'); // Retrieves file extension

            if (ext && strcmp(ext, TARGET_EXTENSION) == 0)
            {
                process_file(filepath); // Process matching files
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s [directory path]\n", argv[0]);

        return EXIT_FAILURE;
    }

    traverse_dir(argv[1]);

    return EXIT_SUCCESS;
}