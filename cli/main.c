#include <stdio.h>
#include "trackmyfunds.h"

int main() {
    int choice, auth = 0;

    while (!auth) {
        printf("1. Register\n2. Login\n3. Delete User\nChoice: ");
        scanf("%d", &choice);

        if (choice == 1) registerUser();
        else if (choice == 2) auth = login();
        else if (choice == 3) deleteUser();
    }

    loadTransactions();

    while (1) {
        printf("\n==== TrackMyFunds (%s) ====\n", currentUser);
        printf("1. Add Transaction\n2. View Balance\n3. View Transactions\n");
        printf("4. Category Report\n5. Graphical Report\n6. Exit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addTransaction(); break;
            case 2: viewBalance(); break;
            case 3: viewTransactions(); break;
            case 4: categoryReport(); break;
            case 5: showGraphicalReport(); break;
            case 6: saveTransaction(); return 0;
        }
    }
}
