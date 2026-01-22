#include "trackmyfunds.h"
#include <stdio.h>
#include <string.h>

#define USER_FILE "users.txt"

int login() {
    char username[50], password[50];
    char fileUser[50], filePass[50];

    FILE *f = fopen(USER_FILE, "r");
    if (!f) {
        printf("User database not found.\n");
        return 0;
    }

    printf("Enter username: ");
    scanf("%s", username);

    printf("Enter password: ");
    scanf("%s", password);

    while (fscanf(f, "%s %s", fileUser, filePass) != EOF) {
        if (strcmp(username, fileUser) == 0 &&
            strcmp(password, filePass) == 0) {
            strcpy(currentUser, username);
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    printf("Invalid credentials!\n");
    return 0;
}

void registerUser() {
    char username[50], password[50];
    FILE *f = fopen(USER_FILE, "a");

    if (!f) {
        perror("Error opening user file");
        return;
    }

    printf("Enter username: ");
    scanf("%s", username);

    printf("Create password: ");
    scanf("%s", password);

    fprintf(f, "%s %s\n", username, password);
    fclose(f);

    printf("Registration successful.\n");
}

void deleteUser() {
    char username[50];
    char fileUser[50], filePass[50];
    int found = 0;

    FILE *f = fopen(USER_FILE, "r");
    if (!f) {
        printf("User database not found.\n");
        return;
    }

    printf("Enter username to delete: ");
    scanf("%s", username);

    FILE *temp = fopen("temp_users.txt", "w");
    if (!temp) {
        fclose(f);
        return;
    }

    while (fscanf(f, "%s %s", fileUser, filePass) != EOF) {
        if (strcmp(username, fileUser) != 0) {
            fprintf(temp, "%s %s\n", fileUser, filePass);
        } else {
            found = 1;
        }
    }

    fclose(f);
    fclose(temp);

    if (!found) {
        printf("User not found.\n");
        remove("temp_users.txt");
        return;
    }

    remove(USER_FILE);
    rename("temp_users.txt", USER_FILE);

    printf("User deleted successfully.\n");
}
