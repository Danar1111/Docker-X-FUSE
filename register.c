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
