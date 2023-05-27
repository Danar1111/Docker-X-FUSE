#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

typedef struct {
    char extension[10];
    int count;
    int isFolder;
} ExtensionCount;

void updateExtensionCount(ExtensionCount *extensionCounts, int *numExtensions, const char *extension, int isFolder) {
    for (int i = 0; i < *numExtensions; i++) {
        if (strcmp(extensionCounts[i].extension, extension) == 0 && extensionCounts[i].isFolder == isFolder) {
            extensionCounts[i].count++;
            return;
        }
    }

    strcpy(extensionCounts[*numExtensions].extension, extension);
    extensionCounts[*numExtensions].count = 1;
    extensionCounts[*numExtensions].isFolder = isFolder;
    (*numExtensions)++;
}

void scanDirectory(const char *path, ExtensionCount *extensionCounts, int *numExtensions, int *numFolders) {
    DIR *dir;
    struct dirent *entry;
    char filePath[512];

    if (!(dir = opendir(path))) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            (*numFolders)++;
            updateExtensionCount(extensionCounts, numExtensions, "folder", 1);
            scanDirectory(filePath, extensionCounts, numExtensions, numFolders);
        } else {
            const char *extension = strrchr(entry->d_name, '.');
            if (extension) {
                updateExtensionCount(extensionCounts, numExtensions, extension + 1, 0);
            }
        }
    }

    closedir(dir);
}

void writeExtensionCountToFile(const ExtensionCount *extensionCounts, int numExtensions, int numFolders) {
    FILE *file = fopen("extension.txt", "w");
    if (file == NULL) {
        printf("Gagal membuka file extension.txt.\n");
        return;
    }

    fprintf(file, "Folder = %d\n", numFolders);

    for (int i = 0; i < numExtensions; i++) {
        if (extensionCounts[i].isFolder) {
            continue;
        }
        fprintf(file, "%s = %d\n", extensionCounts[i].extension, extensionCounts[i].count);
    }

    fclose(file);
}

int main() {
    system("tree rahasia > result.txt");
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char destination[] = "/rahasia/";
    const char *path = strcat(cwd, destination);
    ExtensionCount extensionCounts[100];
    int numExtensions = 0;
    int numFolders = 0;

    scanDirectory(path, extensionCounts, &numExtensions, &numFolders);
    writeExtensionCountToFile(extensionCounts, numExtensions, numFolders);

    printf("Pemindaian selesai. Hasil disimpan dalam file extension.txt.\n");

    return 0;
}
