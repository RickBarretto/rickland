// ============
// Result<T, E>
// ============
//
// :Author: RickBarretto
// :Year: 2026
// :License: MPL 2.0
// :Copyright: RickBarretto, 2026
// :Version: 1.0
//
// ``Result<T, E>`` is a generic result type.
// This provides a safe interface to deal with results in C,
// while being readable and avoiding namespace conflicts.
// This also simulates generics in C by using macros, 
// making our code more reusable.
//
//
// How to Use
// ----------
//
// Include this header file after defining the type T and the PRINT_T macro,
// as well as the type E and the PRINT_E macro.
// See each function documentation for usage details.
//
// A common way to import it would be:
//
//      typedef char* cstring;
//      #define T cstring
//      #define PRINT_T(value) printf("%s", value)
//      #define E cstring
//      #define PRINT_E(value) printf("%s", value)
//      #include "result.h"
//
// Notice that we need to declare a char* as cstring
// due to the macro evaluation rules.
// Each function we have is namespaced under Result<T, E>,
// i.e. ``Result(Value, Errors, unwrap)`` is the same as 
// ``Result_Value_Errors_unwrap``.
// If you pass a char* directly as T or E, the macro expansion will break.
// 
// And a common way to use it would be:
//
//      void handle_success(cstring message) {
//          printf("Success: %s\n", message);
//      }
//
//      void handle_failure(cstring error) {
//          printf("Failure: %s\n", error);
//      }
//
//      Result(Output, ExitCode) * result = 
//          Success(Output, ExitCode)("All good");
//      Result(Output, ExitCode, match)
//          (result, handle_success, handle_failure);
//
// Custom Enum Errors
// ------------------
//
// A good way to use Result<T, E> is to define a custom enum for errors.
// This way, you can have a more structured error handling in your code.
//
// Let's see an example of a shell executor:
//
//      #define T Output
//      #define PRINT_T(value) printf("%s", value)
//      #define E ExitCode
//      #define PRINT_E(value) printf("%s", ExitCode_as_string[value])
//      #include "result.h"
//
//      typedef enum {
//          SUCCESS = 0,
//          COMMAND_NOT_FOUND = 1,
//          PERMISSION_DENIED = 2,
//          UNKNOWN_ERROR = 3
//      } ExitCode;
//
//      const char* ExitCode_as_string[] = {
//          "Success",
//          "Command Not Found",
//          "Permission Denied",
//          "Unknown Error"
//      };
// 
//     Result(Output, ExitCode) * execute_command(str command) {
//         // ... command execution logic ...
//         if (command_not_found) {
//             return Fail(Output, ExitCode)(COMMAND_NOT_FOUND);
//         }
//         // On success
//         return Success(Output, ExitCode)(output);
//     }
//
//      Result(Output, ExitCode) * result = execute_command("ls -l");
//      Result(Output, ExitCode, debug)(result);
//

// =~=~=~=~=~=~=~=~ Imports ~=~=~=~=~=~=~=~=

#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// =~=~=~=~=~=~=~=~ Helper Macros ~=~=~=~=~=~=~=~=

#define _CAT(X, Y) X ## _ ## Y
#define CAT(X, Y) _CAT(X, Y)
#define _CAT3(X, Y, Z) X ## _ ## Y ## _ ## Z
#define CAT3(X, Y, Z) _CAT3(X, Y, Z)
#define _CAT4(W, X, Y, Z) W ## _ ## X ## _ ## Y ## _ ## Z
#define CAT4(W, X, Y, Z) _CAT4(W, X, Y, Z)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ensure(COND, VAL) if (not (COND)) return VAL

// =~=~=~=~=~=~=~=~ Implementation ~=~=~=~=~=~=~=~=

// T: Success type of the Result<T, E>
#ifndef T
#error "T is not defined"
#endif

// E: Error type of the Result<T, E>
#ifndef E
#error "E is not defined"
#endif

// PRINT_T: (T) -> void
// 
// PRINT_T is a macro that defines how to print an element of type T.
// You must refer to a print function or macro suitable for type T.
//
// Examples
// --------
//
// Using inline macro:
//
//      #define PRINT_T(value) printf("%s", value)
// 
// Using function pointer:
//
//      void print_str(str value) { printf("%s", value); }
//      #define PRINT_T printf_str
//
#ifndef PRINT_T
#error "PRINT_T is not defined"
#endif

// PRINT_E: (E) -> void
// 
// PRINT_E is a macro that defines how to print an element of type E.
// See PRINT_T for more details.
//
#ifndef PRINT_E
#error "PRINT_E is not defined"
#endif

#define MODULE Result
#define Self CAT3(MODULE, T, E)
#define fn(NAME) CAT(Self, NAME)

#define _RESULT_SELECT_MACRO(_1, _2, _3, NAME, ...) NAME
#define Result(...) _RESULT_SELECT_MACRO(__VA_ARGS__, Result3, Result2, Result1)(__VA_ARGS__)
#define Result1(T) CAT(MODULE, T, T)
#define Result2(T, E) CAT3(MODULE, T, E)
#define Result3(T, E, FUNC) CAT(CAT3(MODULE, T, E), FUNC)

typedef struct {
    bool _is_ok;
    union {
        T _ok;
        E _err;
    };
} Self;

// Success(ok: type<T>, err: type<E>, value: T) -> *Result<T, E>
// 
// Creates a new success Result<T, E> with the given value.
// This is a shorthand for Result(T, E, success)(value).
//
// Parameters
// ----------
// value : T
//     The success value to be stored in the Result.
//
// Returns
// -------
// *Result<T, E>: A pointer to the newly created success Result.
//
#define Success(T, E) Result(T, E, success)

// Fail(err: type<E>, ok: type<T>, error: E) -> *Result<T, E>
//
// Creates a new failure Result<T, E> with the given error.
// This is a shorthand for Result(T, E, fail)(error).
//
// Parameters
// ----------
// error : E
//     The error value to be stored in the Result.
//
// Returns
// -------
// *Result<T, E>: A pointer to the newly created failure Result.
//
#define Fail(T, E) Result(T, E, fail)

// Result >> success(value: T) -> *Result<T, E>
// 
// Creates a new success Result<T, E> with the given value.
//
// Parameters
// ----------
// value : T
//     The success value to be stored in the Result.
//
// Returns
// -------
// *Result<T, E>: A pointer to the newly created success Result.
//
// See Also
// --------
// Success(T, E, value): Shorthand macro for this function.
//
Self * fn(success)(T value) {
    Self * result = (Self *) malloc(sizeof(Self));
    ensure(result, NULL);

    result->_is_ok = true;
    result->_ok = value;
    return result;
}

// Result >> fail(error: E) -> *Result<T, E>
//
// Creates a new failure Result<T, E> with the given error.
//
// Parameters
// ----------
// error : E
//     The error value to be stored in the Result.
//
// Returns
// -------
// *Result<T, E>: A pointer to the newly created failure Result.
//
// See Also
// --------
// Fail(T, E, error): Shorthand macro for this function.
// 
Self * fn(fail)(E error) {
    Self * result = (Self *) malloc(sizeof(Self));
    ensure(result, NULL);

    result->_is_ok = false;
    result->_err = error;
    return result;
}

// Result >> delete(result: *Result<T, E>) -> bool
//
// Safely deletes the Result, including double free protection.
//
// Parameters
// ----------
// result : *Result<T, E>
//     The Result to be deleted.
//
// Returns
// -------
// bool: Returns true on success.
//
bool fn(delete)(Self * result) {
    ensure(result, false);
    free(result);
}

// Result >> is_ok(result: *Result<T, E>) -> bool
// 
// Returns true if the Result is a success.
bool fn(is_ok)(Self * result) {
    ensure(result, false);
    return result->_is_ok;
}

// Result >> is_err(result: *Result<T, E>) -> bool
// 
// Returns true if the Result is an error.
bool fn(is_err)(Self * result) {
    ensure(result, false);
    return not result->_is_ok;
}

// Result >> value(result: *Result<T, E>) -> *T
//
// Returns a pointer to the success value if the Result is a success.
//
// Parameters
// ----------
// result : *Result<T, E>
//    The Result from which to get the success value.
// 
// Returns
// -------
// *T: A pointer to the success value.
//     If the Result is an error, returns NULL.
T * fn(value)(Self * result) {
    ensure(result, NULL);
    ensure(result->_is_ok, NULL);
    return &result->_ok;
}

// Result >> error(result: *Result<T, E>) -> *E
//
// Returns a pointer to the error value if the Result is an Fail.
//
// Parameters
// ----------
// result : *Result<T, E>
//    The Result from which to get the error value.
// 
// Returns
// -------
// *E: A pointer to the error value.
//     If the Result is not an error, returns NULL.
E * fn(error)(Self * result) {
    ensure(result, NULL);
    ensure(not result->_is_ok, NULL);
    return &result->_err;
}

// Result >> value_or(result: *Result<T, E>, fallback: *T) -> *T
//
// Returns the success value or a fallback if the Result is an error.
//
// Parameters
// ----------
// result : *Result<T, E>
//     The Result from which to get the success value.
// fallback : *T
//     The fallback value to return if the Result is an error.
//
T * fn(value_or)(Self * result, T * fallback) {
    ensure(result, NULL);
    return (result->_is_ok) ? &result->_ok : fallback;
}

// Result >> error_or(result: *Result<T, E>, fallback: *E) -> *E
//
// Returns the fail error or a fallback if the Result is not an error.
//
// Parameters
// ----------
// result : *Result<T, E>
//     The Result from which to get the success value.
// fallback : *E
//     The fallback value to return if the Result is not an error.
//
E * fn(error_or)(Self * result, E * fallback) {
    ensure(result, NULL);
    return (not result->_is_ok) ? &result->_err : fallback;
}

// Result >> value_or_else(result: *Result<T, E>, or_else: () -> *T) -> *T
//
// Returns the success value or a fallback if the Result is an error.
//
// Parameters
// ----------
// result : *Result<T, E>
//     The Result from which to get the success value.
// fallback : () -> *T
//     The fallback function to return if the Result is an error.
//
T * fn(value_or_else)(Self * result, T * (*or_else)()) {
    ensure(result, NULL);
    return (result->_is_ok) ? &result->_ok : or_else();
}

// Result >> error_or_else(result: *Result<T, E>, or_else: () -> *E) -> *E
//
// Returns the fail error or a fallback if the Result is not an error.
//
// Parameters
// ----------
// result : *Result<T, E>
//     The Result from which to get the success value.
// fallback : () -> *E
//     The fallback function to return if the Result is not an error.
//
E * fn(error_or_else)(Self * result, E * (*or_else)()) {
    ensure(result, NULL);
    return (not result->_is_ok) ? &result->_err : or_else();
}

// Result >> match(result: *Result<T, E>, then: (T) -> void, or_else: (E) -> void) -> void
//
// Matches the Result and calls the appropriate function.
//
// Parameters
// ----------
// result : *Result<T, E>
//     The Result to match.
// then : (T) -> void
//     The function to call if the Result is a success.
// or_else : (E) -> void
//    The function to call if the Result is an error.
//
void fn(match)(Self * result, void (*then)(T), void (*or_else)(E)) {
    ensure(result, );
    (result->_is_ok) ? then(result->_ok) : or_else(result->_err);
}

// Result >> print(result: *Result<T, E>) -> void
//
// Prints the Result on terminal.
//
void fn(print)(Self * result) {
    ensure(result, );

    printf("%s", (result->_is_ok) ? "Ok" : "Error");
    printf(" { ");
    (result->_is_ok) ? PRINT_T(result->_ok) : PRINT_E(result->_err);
    printf(" }");

}

// Result >> println(result: *Result<T, E>) -> void
// Prints the Result on terminal followed by a newline.
//
void fn(println)(Self * result) {
    ensure(result, );

    fn(print)(result);
    printf("\n");
}

// Result >> debug(result: *Result<T, E>) -> void
// 
// Prints a detailed debug representation of the Result.
//
void fn(debug)(Self * result) {
    ensure(result, );

    const char* variant = (result->_is_ok) ? "Ok" : "Error";

    printf("Result::%s<%s, %s> {\n", variant, TOSTRING(T), TOSTRING(E));
    printf("  is_ok: %s,\n", result->_is_ok ? "true" : "false");
    printf("  value: "); 
        (result->_is_ok)? PRINT_T(result->_ok) : PRINT_E(result->_err);
    printf("\n}\n");
}

#undef Self
#undef fn
#undef T
#undef E
#undef PRINT_T
#undef PRINT_E

