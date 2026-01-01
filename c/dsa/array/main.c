#include <stdio.h>

typedef char* str;
#define T str
#define PRINT_T(value) printf("%s", value)
#include "array.h"

int main() {

    Array(str) * names = Array(str, new)(5);

    Array(str, set)(names, 0, "Alice");
    Array(str, set)(names, 1, "Bob");
    Array(str, set)(names, 2, "Charlie");
    Array(str, set)(names, 3, "Diana");
    Array(str, set)(names, 4, "Eve");

    Array(str, debug)(names);

    Array(str, delete)(names);
    return 0;

}