#ifndef TRACKMYFUNDS_H
#define TRACKMYFUNDS_H

#include <time.h>

/* ===== STRUCT ===== */
typedef struct {
    char type[10];
    char category[20];
    float amount;
    char date[30];
} Transaction;

/* ===== GLOBALS ===== */
extern Transaction *transactions;
extern int transactionCount;
extern float totalIncome, totalExpenses;
extern char currentUser[50];

/* ===== UTILS ===== */
void getCurrentTime(char* buffer);
void toUpperStr(char *str);

/* ===== AUTH ===== */
int login();
void registerUser();
void deleteUser();

/* ===== TRANSACTIONS ===== */
void addTransaction();
void saveTransaction();
void loadTransactions();
void viewTransactions();
void viewBalance();

/* ===== REPORTS ===== */
void categoryReport();
void showGraphicalReport();

#endif
