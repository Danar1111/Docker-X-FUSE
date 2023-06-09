# Docker-X-FUSE

`download & unzip`
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>

#define URL "https://drive.google.com/u/0/uc?id=18YCFdG658SALaboVJUHQIqeamcfNY39a&export=download&confirm=yes"
#define FILE_NAME "rahasia.zip"

int main() {
    pid_t pid = fork();

    if (pid == 0) {
        execl("/usr/bin/wget", "/usr/bin/wget", URL, "-O", FILE_NAME, NULL);
        exit(0);

    } else if (pid > 0) {
        wait(NULL);
        system("unzip rahasia.zip");
        remove(FILE_NAME);
        
    } else {
        printf("Fork failed\n");
        return 1;
    }

    return 0;
}
```

`register & login`
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50

void registerUser();
bool isUsernameTaken(const char* username);
bool saveCredentials(const char* username, const char* hashedPassword);

void loginUser();
bool verifyCredentials(const char* username, const char* password);

void hashPassword(const char* password, char* hashedPassword);

int main() {
    int choice;

    while (1) {
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                loginUser();
                break;
            case 3:
                printf("Exiting program...\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}

void registerUser() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char hashedPassword[MAX_PASSWORD_LENGTH + 1]; // +1 for null terminator

    printf("Enter username: ");
    scanf("%s", username);

    if (isUsernameTaken(username)) {
        printf("Username already exists! Please choose a different username.\n");
        return;
    }

    printf("Enter password: ");
    scanf("%s", password);

    hashPassword(password, hashedPassword);

    if (saveCredentials(username, hashedPassword)) {
        printf("Registration successful!\n");
    } else {
        printf("Failed to save credentials. Please try again.\n");
    }
}

bool isUsernameTaken(const char* username) {
    FILE* file = fopen("credentials.txt", "r");
    char line[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2]; // +2 for ';' and '\n'

    if (file == NULL) {
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char savedUsername[MAX_USERNAME_LENGTH + 1]; // +1 for null terminator
        sscanf(line, "%[^;]", savedUsername);

        if (strcmp(username, savedUsername) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

void hashPassword(const char* password, char* hashedPassword) {
    char command[MAX_PASSWORD_LENGTH + 20]; // Command to execute
    char result[33]; // MD5 hash result (32 characters) + null terminator

    sprintf(command, "echo -n %s | md5sum | awk '{ print $1 }'", password);

    FILE* pipe = popen(command, "r");
    fgets(result, sizeof(result), pipe);
    pclose(pipe);

    strcpy(hashedPassword, result);
}

bool saveCredentials(const char* username, const char* hashedPassword) {
    FILE* file = fopen("credentials.txt", "a");

    if (file == NULL) {
        return false;
    }

    fprintf(file, "%s;%s\n", username, hashedPassword);
    fclose(file);

    return true;
}

void loginUser() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char hashedPassword[MAX_PASSWORD_LENGTH + 1]; // +1 for null terminator

    printf("Enter username: ");
    scanf("%s", username);

    printf("Enter password: ");
    scanf("%s", password);

    hashPassword(password, hashedPassword);

    if (verifyCredentials(username, hashedPassword)) {
        printf("Login successful!\n");
    } else {
        printf("Invalid username or password. Please try again.\n");
    }
}

bool verifyCredentials(const char* username, const char* password) {
    FILE* file = fopen("credentials.txt", "r");
    char line[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2]; // +2 for ';' and '\n'

    if (file == NULL) {
        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char savedUsername[MAX_USERNAME_LENGTH + 1]; // +1 for null terminator
        char savedHashedPassword[MAX_PASSWORD_LENGTH + 1]; // +1 for null terminator
        sscanf(line, "%[^;];%s", savedUsername, savedHashedPassword);

        if (strcmp(username, savedUsername) == 0 && strcmp(password, savedHashedPassword) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}
```

`list tree & extension`
```c
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
```