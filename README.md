#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_PASS 50
#define USER_FILE "users.txt"

typedef struct {
    char type[10];
    char category[20];
    float amount;
    char date[30];
} Transaction;

Transaction transactions[1000];
int transactionCount = 0;
float totalIncome = 0, totalExpenses = 0;
char currentUser[MAX_NAME];
int isLoggedIn = 0;

void getCurrentTime(char* buffer) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, 30, "%d-%m-%Y %I:%M:%S %p", t);
}

void toUpperStr(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

void saveToFile() {
    char filename[MAX_NAME + 15];
    sprintf(filename, "%s_data.txt", currentUser);
    FILE *f = fopen(filename, "w");
    if (!f) return;
    for (int i = 0; i < transactionCount; i++) {
        fprintf(f, "%s %s %.2f %s\n",
                transactions[i].type,
                transactions[i].category,
                transactions[i].amount,
                transactions[i].date);
    }
    fclose(f);
    printf("Data saved successfully.\n");
}

void loadFromFile() {
    char filename[MAX_NAME + 15];
    sprintf(filename, "%s_data.txt", currentUser);
    FILE *f = fopen(filename, "r");
    if (!f) return;
    transactionCount = 0;
    totalIncome = 0;
    totalExpenses = 0;
    while (fscanf(f, "%s %s %f %[^
]",
                  transactions[transactionCount].type,
                  transactions[transactionCount].category,
                  &transactions[transactionCount].amount,
                  transactions[transactionCount].date) == 4) {
        if (strcmp(transactions[transactionCount].type, "INCOME") == 0)
            totalIncome += transactions[transactionCount].amount;
        else if (strcmp(transactions[transactionCount].type, "EXPENSE") == 0)
            totalExpenses += transactions[transactionCount].amount;
        transactionCount++;
    }
    fclose(f);
}

int userExists(const char* username) {
    FILE* f = fopen(USER_FILE, "r");
    if (!f) return 0;
    char uname[MAX_NAME], pword[MAX_PASS];
    while (fscanf(f, "%s %s", uname, pword) != EOF) {
        if (strcmp(uname, username) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int registerUser() {
    char username[MAX_NAME], password[MAX_PASS];
    printf("Enter username: ");
    scanf("%s", username);
    if (userExists(username)) {
        printf("User already exists.\n");
        return 0;
    }
    printf("Create password: ");
    scanf("%s", password);
    FILE* f = fopen(USER_FILE, "a");
    if (!f) {
        printf("Error saving data.\n");
        return 0;
    }
    fprintf(f, "%s %s\n", username, password);
    fclose(f);
    printf("Registration successful. Please login.\n");
    return 1;
}

int loginUser() {
    char username[MAX_NAME], password[MAX_PASS];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    FILE* f = fopen(USER_FILE, "r");
    if (!f) {
        printf("User file not found.\n");
        return 0;
    }
    char uname[MAX_NAME], pword[MAX_PASS];
    while (fscanf(f, "%s %s", uname, pword) != EOF) {
        if (strcmp(username, uname) == 0 && strcmp(password, pword) == 0) {
            strcpy(currentUser, username);
            fclose(f);
            loadFromFile();
            isLoggedIn = 1;
            return 1;
        }
    }
    fclose(f);
    printf("Invalid username or password.\n");
    return 0;
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
    printf("Enter category: ");
    scanf("%s", t.category);
    toUpperStr(t.category);
    printf("Enter amount: ");
    scanf("%f", &t.amount);
    getCurrentTime(t.date);
    if (strcmp(t.type, "INCOME") == 0) totalIncome += t.amount;
    else totalExpenses += t.amount;
    transactions[transactionCount++] = t;
    printf("Transaction added successfully!\n");
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
    char category[20];
    printf("Enter category to report: ");
    scanf("%s", category);
    toUpperStr(category);
    float total = 0;
    for (int i = 0; i < transactionCount; i++) {
        if (strcmp(transactions[i].category, category) == 0 && strcmp(transactions[i].type, "EXPENSE") == 0) {
            total += transactions[i].amount;
        }
    }
    printf("Total spent on %s: %.2f\n", category, total);
}

void showGraphicalReport() {
    printf("\n[ Category Comparison - Expense Amounts ]\n");
    for (int i = 0; i < transactionCount; i++) {
        if (strcmp(transactions[i].type, "EXPENSE") == 0) {
            float amount = transactions[i].amount;
            printf("%-15s |", transactions[i].category);
            int bars = (int)(amount);
            if (bars > 50) bars = 50;
            for (int j = 0; j < bars; j++) printf("#");
            for (int j = bars; j < 50; j++) printf(".");
            printf("| %.2f\n", amount);
        }
    }
}

void deleteUser() {
    if (!isLoggedIn) {
        printf("No user logged in to delete.\n");
        return;
    }
    printf("Are you sure you want to delete your account '%s'? (yes/no): ", currentUser);
    char confirm[10];
    scanf("%s", confirm);
    if (strcmp(confirm, "yes") != 0) {
        printf("Account deletion cancelled.\n");
        return;
    }

    char filename[MAX_NAME + 15];
    sprintf(filename, "%s_data.txt", currentUser);
    remove(filename);

    FILE* f = fopen(USER_FILE, "r");
    FILE* temp = fopen("temp.txt", "w");
    if (!f || !temp) {
        printf("Error opening files.\n");
        return;
    }
    char uname[MAX_NAME], pword[MAX_PASS];
    while (fscanf(f, "%s %s", uname, pword) != EOF) {
        if (strcmp(uname, currentUser) != 0) {
            fprintf(temp, "%s %s\n", uname, pword);
        }
    }
    fclose(f);
    fclose(temp);
    remove(USER_FILE);
    rename("temp.txt", USER_FILE);

    printf("User '%s' and their data have been deleted.\n", currentUser);
    isLoggedIn = 0;
    strcpy(currentUser, "");
    transactionCount = 0;
    totalIncome = totalExpenses = 0;
}

int main() {
    int choice;
    while (1) {
        printf("\n1. Register\n2. Login\nEnter choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }
        if (choice == 1) registerUser();
        else if (choice == 2 && loginUser()) break;
        else printf("Try again.\n");
    }

    while (isLoggedIn) {
        printf("\n==== Personal Finance Manager (%s) ====\n", currentUser);
        printf("1. Add Transaction\n");
        printf("2. View Balance\n");
        printf("3. View Transactions\n");
        printf("4. Category Report\n");
        printf("5. Graphical Report\n");
        printf("6. Save & Exit\n");
        printf("7. Delete Account\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        switch (choice) {
            case 1: addTransaction(); break;
            case 2: viewBalance(); break;
            case 3: viewTransactions(); break;
            case 4: categoryReport(); break;
            case 5: showGraphicalReport(); break;
            case 6: saveToFile(); printf("Goodbye!\n"); return 0;
            case 7: deleteUser(); break;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}
