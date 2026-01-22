#include "trackmyfunds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void saveTransaction() {
    char filename[100];
    sprintf(filename, "%s_transactions.txt", currentUser);
    FILE *f = fopen(filename, "w");
    if (!f) return;

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

    transactions = malloc(sizeof(Transaction) * (transactionCount + 1));

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

    printf("Enter category: ");
    scanf("%s", t.category);
    toUpperStr(t.category);

    printf("Enter amount: ");
    scanf("%f", &t.amount);

    getCurrentTime(t.date);

    if (!strcmp(t.type, "INCOME")) totalIncome += t.amount;
    else totalExpenses += t.amount;

    transactions = realloc(transactions, sizeof(Transaction) * (transactionCount + 1));
    transactions[transactionCount++] = t;

    saveTransaction();
}

void viewBalance() {
    printf("\nIncome: %.2f\nExpenses: %.2f\nBalance: %.2f\n",
           totalIncome, totalExpenses, totalIncome - totalExpenses);
}

void viewTransactions() {
    printf("\n%-10s %-15s %-10s %-20s\n",
           "Type", "Category", "Amount", "Date");

    for (int i = 0; i < transactionCount; i++) {
        printf("%-10s %-15s %-10.2f %-20s\n",
               transactions[i].type,
               transactions[i].category,
               transactions[i].amount,
               transactions[i].date);
    }
}
