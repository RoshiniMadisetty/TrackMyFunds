#include "trackmyfunds.h"
#include <stdio.h>
#include <string.h>

void categoryReport() {
    char cat[20];
    float total = 0;

    printf("Enter category: ");
    scanf("%s", cat);
    toUpperStr(cat);

    for (int i = 0; i < transactionCount; i++) {
        if (!strcmp(transactions[i].category, cat) &&
            !strcmp(transactions[i].type, "EXPENSE")) {
            total += transactions[i].amount;
        }
    }
    printf("Total spent on %s: %.2f\n", cat, total);
}

void showGraphicalReport() {
    float percent = (totalIncome == 0) ? 0 : (totalExpenses / totalIncome) * 100;
    int blocks = (int)(percent / 2);
    if (blocks > 50) blocks = 50;

    printf("\n|");
    for (int i = 0; i < blocks; i++) printf("#");
    for (int i = blocks; i < 50; i++) printf(".");
    printf("| %.2f%% used\n", percent);
}
