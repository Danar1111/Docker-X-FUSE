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