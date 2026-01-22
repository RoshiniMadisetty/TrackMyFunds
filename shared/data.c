#include "trackmyfunds.h"
#include <stdlib.h>

Transaction *transactions = NULL;
int transactionCount = 0;
float totalIncome = 0;
float totalExpenses = 0;
char currentUser[50];
