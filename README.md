#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_LEN 100
#define USER_FILE "users.txt"

typedef struct {
    char type[10];       // "INCOME" or "EXPENSE"
    char category[20];   // e.g. "FOOD", "RENT"
    float amount;
    char date[30];       // Date and Time string
} Transaction;

Transaction *transactions = NULL;
int transactionCount = 0;
float totalIncome = 0, totalExpenses = 0;
char currentUser[50];

void getCurrentTime(char* buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 30, "%d-%m-%Y %I:%M:%S %p", t);  // Indian format
}

void toUpperStr(char *str) {
    for (int i = 0; str[i]; i++) str[i] = toupper(str[i]);
}

void saveTransaction() {
    char filename[100];
    sprintf(filename, "%s_transactions.txt", currentUser);
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Failed to save transactions");
        return;
    }

    fprintf(f, "%d %.2f %.2f\n", transactionCount, totalIncome, totalExpenses);
    for (int i = 0; i < transactionCount; i++) {
        fprintf(f, "%s %s %.2f %s\n",
            transactions[i].type,
            transactions[i].category,
            transactions[i].amount,
            transactions[i].date);
    }
    fclose(f);
}

void loadTransactions() {
    char filename[100];
    sprintf(filename, "%s_transactions.txt", currentUser);
    FILE *f = fopen(filename, "r");
    if (!f) return;

    fscanf(f, "%d %f %f\n", &transactionCount, &totalIncome, &totalExpenses);
    transactions = (Transaction*)malloc(sizeof(Transaction) * (transactionCount + 1));
    for (int i = 0; i < transactionCount; i++) {
        fscanf(f, "%s %s %f %[^\n]", 
            transactions[i].type,
            transactions[i].category,
            &transactions[i].amount,
            transactions[i].date);
    }
    fclose(f);
}

void addTransaction() {
    Transaction t;
    printf("Enter type (Income/Expense): ");
    scanf("%s", t.type);
    toUpperStr(t.type);

    if (strcmp(t.type, "INCOME") != 0 && strcmp(t.type, "EXPENSE") != 0) {
        printf("Invalid type!\n");
        return;
    }

    printf("Enter category (e.g., Food, Rent): ");
    scanf("%s", t.category);
    toUpperStr(t.category);

    printf("Enter amount: ");
    scanf("%f", &t.amount);

    getCurrentTime(t.date);

    if (strcmp(t.type, "INCOME") == 0) totalIncome += t.amount;
    else totalExpenses += t.amount;

    transactions = realloc(transactions, sizeof(Transaction) * (transactionCount + 1));
    transactions[transactionCount++] = t;

    printf("Transaction Added Successfully!\n");
    saveTransaction();
}

void viewBalance() {
    printf("\nTotal Income: %.2f\n", totalIncome);
    printf("Total Expenses: %.2f\n", totalExpenses);
    printf("Balance: %.2f\n", totalIncome - totalExpenses);
}

void viewTransactions() {
    printf("\n%-10s %-15s %-10s %-20s\n", "Type", "Category", "Amount", "Date");
    for (int i = 0; i < transactionCount; i++) {
        printf("%-10s %-15s %-10.2f %-20s\n",
            transactions[i].type,
            transactions[i].category,
            transactions[i].amount,
            transactions[i].date);
    }
}

void categoryReport() {
    printf("Available Categories:\n");
    for (int i = 0; i < transactionCount; i++) {
        printf("- %s\n", transactions[i].category);
    }

    char cat[20];
    float total = 0;
    printf("Enter category to report: ");
    scanf("%s", cat);
    toUpperStr(cat);

    for (int i = 0; i < transactionCount; i++) {
        if (strcmp(transactions[i].category, cat) == 0 &&
            strcmp(transactions[i].type, "EXPENSE") == 0) {
            total += transactions[i].amount;
        }
    }

    printf("Total spent on %s: %.2f\n", cat, total);
}

void showGraphicalReport() {
    printf("\n[ Budget Usage - Expense as Percentage of Income ]\n");
    float percent = (totalIncome == 0) ? 0 : (totalExpenses / totalIncome) * 100;
    int blocks = (int)(percent / 2);  // scale to 50 chars
    if (blocks > 50) blocks = 50;

    printf("|");
    for (int i = 0; i < blocks; i++) printf("#");
    for (int i = blocks; i < 50; i++) printf(".");
    printf("| %.2f%% used\n", percent);

    // Show per-category comparative report
    printf("\n[ Category Comparison - Expense Amounts ]\n");

    // Collect unique categories
    char categories[100][20];
    float categorySums[100] = {0};
    int catCount = 0;

    for (int i = 0; i < transactionCount; i++) {
        if (strcmp(transactions[i].type, "EXPENSE") != 0) continue;

        int found = 0;
        for (int j = 0; j < catCount; j++) {
            if (strcmp(categories[j], transactions[i].category) == 0) {
                categorySums[j] += transactions[i].amount;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(categories[catCount], transactions[i].category);
            categorySums[catCount] = transactions[i].amount;
            catCount++;
        }
    }

    for (int i = 0; i < catCount; i++) {
        int b = (int)((categorySums[i] / totalExpenses) * 50);
        if (b > 50) b = 50;
        printf("%-15s |", categories[i]);
        for (int k = 0; k < b; k++) printf("#");
        for (int k = b; k < 50; k++) printf(".");
        printf("| %.2f\n", categorySums[i]);
    }
}

int login() {
    char username[50], password[50], fileUser[50], filePass[50];
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
        if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
            strcpy(currentUser, username);
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    printf("User not found!\n");
    return 0;
}

void registerUser() {
    char username[50], password[50];
    FILE *f = fopen(USER_FILE, "a");
    if (!f) {
        perror("Error saving data");
        return;
    }

    printf("Enter username: ");
    scanf("%s", username);
    printf("Create password: ");
    scanf("%s", password);
    fprintf(f, "%s %s\n", username, password);
    fclose(f);
    printf("Registration successful. Please login.\n");
}

void deleteUser() {
    char username[50], fileUser[50], filePass[50];
    FILE *f = fopen(USER_FILE, "r");
    if (!f) {
        printf("User database not found.\n");
        return;
    }

    printf("Enter username to delete: ");
    scanf("%s", username);

    // Check if the user exists
    int userFound = 0;
    while (fscanf(f, "%s %s", fileUser, filePass) != EOF) {
        if (strcmp(username, fileUser) == 0) {
            userFound = 1;
            break;
        }
    }
    fclose(f);

    if (!userFound) {
        printf("User not found!\n");
        return;
    }

    // Confirm deletion
    char confirm;
    printf("Are you sure you want to delete user '%s'? (y/n): ", username);
    scanf(" %c", &confirm);
    if (confirm != 'y' && confirm != 'Y') {
        printf("User deletion canceled.\n");
        return;
    }

    // Create a temporary file to store users except the one to be deleted
    FILE *tempFile = fopen("temp_users.txt", "w");
    f = fopen(USER_FILE, "r");
    if (!f || !tempFile) {
        printf("Error opening files.\n");
        return;
    }

    while (fscanf(f, "%s %s", fileUser, filePass) != EOF) {
        if (strcmp(username, fileUser) != 0) {
            fprintf(tempFile, "%s %s\n", fileUser, filePass);
        }
    }

    fclose(f);
    fclose(tempFile);

    // Replace the old user file with the new one
    remove(USER_FILE);
    rename("temp_users.txt", USER_FILE);

    printf("User '%s' deleted successfully.\n", username);
}

int main() {
    int choice, auth = 0;

    while (!auth) {
        printf("1. Register\n2. Login\n3. Delete User\nEnter choice: ");
        scanf("%d", &choice);
        if (choice == 1) registerUser();
        else if (choice == 2) auth = login();
        else if (choice == 3) deleteUser();
        else printf("Invalid choice.\n");
    }

    loadTransactions();

    while (1) {
        printf("\n==== Personal Finance Manager (%s) ====\n", currentUser);
        printf("1. Add Transaction\n");
        printf("2. View Balance\n");
        printf("3. View Transactions\n");
        printf("4. Category Report\n");
        printf("5. Graphical Report\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addTransaction(); break;
            case 2: viewBalance(); break;
            case 3: viewTransactions(); break;
            case 4: categoryReport(); break;
            case 5: showGraphicalReport(); break;
            case 6: saveTransaction(); printf("Goodbye!\n"); exit(0);
            default: printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}

