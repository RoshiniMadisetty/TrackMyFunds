#include "trackmyfunds.h"
#include <ctype.h>
#include <time.h>

void getCurrentTime(char* buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 30, "%d-%m-%Y %I:%M:%S %p", t);
}

void toUpperStr(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = toupper(str[i]);
}
