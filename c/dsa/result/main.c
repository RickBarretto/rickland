
typedef char* str;
#define T str
#define E str
#define PRINT_T(value) printf("%s", value)
#define PRINT_E(value) printf("%s", value)
#include "result.h"


void print_success(str message) {
    printf("Success: %s\n", message);
}

void print_fail(str error) {
    printf("Failure: %s\n", error);
}

typedef str Output;

typedef enum {
    SUCCESS = 0,
    COMMAND_NOT_FOUND = 1,
    PERMISSION_DENIED = 2,
    UNKNOWN_ERROR = 3
} ExitCode;

char* ExitCode_as_string[] = {
    "Success",
    "Command Not Found",
    "Permission Denied",
    "Unknown Error"
};

#define T Output
#define E ExitCode
#define PRINT_T(value) printf("%s", value)
#define PRINT_E(value) printf("%s", ExitCode_as_string[value])
#include "result.h"

int main() {

    Result(str, str) * valid = Success(str, str)("Operation succeeded");
    Result(str, str) * invalid = Fail(str, str)("Operation failed");
    Result(Output, ExitCode) * not_found = 
        Fail(Output, ExitCode)(COMMAND_NOT_FOUND);

    Result(str, str, match)(valid, print_success, print_fail);
    Result(str, str, match)(invalid, print_success, print_fail);

    Result(str, str, debug)(valid);
    Result(str, str, debug)(invalid);
    Result(Output, ExitCode, debug)(not_found);

    Result(str, str, delete)(valid);
    Result(str, str, delete)(invalid);
    Result(Output, ExitCode, delete)(not_found);

    return 0;

}