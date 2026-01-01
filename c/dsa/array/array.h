// ========
// Array<T>
// ========
// :Author: RickBarretto
// :Year: 2026
// :License: MPL 2.0
// :Copyright: RickBarretto, 2026
// :Version: 1.0
//
// ``Array<T>`` is generic array. 
// This provides a safe interface to deal with arrays in C,
// while being readable and avoiding namespace conflicts.
// This also simulates generics in C by using macros, 
// making our code more reusable.
//
// How to Use
// ----------
// Include this header file after defining the type T and the PRINT_T macro.
// See each function documentation for usage details.
//
// A common way to import it would be:
//
//      typedef char* cstring;
//      #define T cstring                          // defining the inner type
//      #define PRINT_T(value) printf("%s", value) // defining the print macro
//      #include "array.h"                         // including the DS
//
// Notice that we need to declare a char* as cstring
// due to the macro evaluation rules.
// Each function we have is namespaced under Array<type>,
// i.e. ``Array(cstring, create)`` is the same as ``Array_cstring_create``.
// If you pass a char* directly as T, the macro expansion will break.
// 
// And a common way to use it would be:
//      Array(cstring) * arr = Array(cstring, create)(10);
//      Array(cstring, set)(arr, 0, "Hello");
//      Array(cstring, println)(arr);
//      cstring * first = Array(cstring, get)(arr, 0);
//

#include <iso646.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// T
// T is a macro that defines the type of elements stored in the array.
#ifndef T
#error "T is not defined"
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

#define MODULE Array

#define Self CAT(MODULE, T)

#define _CAT(X, Y) X ## _ ## Y
#define CAT(X, Y) _CAT(X, Y)
#define _CAT3(X, Y, Z) X ## _ ## Y ## _ ## Z
#define CAT3(X, Y, Z) _CAT3(X, Y, Z)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define fn(NAME) CAT(Self, NAME)

#define _ARRAY_SELECT_MACRO(_1, _2, NAME, ...) NAME
#define Array(...) _ARRAY_SELECT_MACRO(__VA_ARGS__, Array2, Array1)(__VA_ARGS__)
#define Array1(T) CAT(MODULE, T)
#define Array2(T, FUNC) CAT3(MODULE, T, FUNC)

#define ensure(COND, VAL) if (not (COND)) return VAL

typedef struct {
    T* data;
    size_t _size;
} Self;


// Array >> new(size: size_t) -> *Array<T>
//
// Creates a new array of given size.
// 
// Parameters
// ----------
// size : size_t 
//     The number of elements in the array.
//
// Returns
// -------
// *Array<T>: A pointer to the newly created array.
Self *fn(new)(size_t size) {
    Self * array = malloc(sizeof(Self));
    array->data = calloc(size, sizeof(T));
    array->_size = size;
    return array;
}

// Array >> delete(array: *Array<T>) -> bool
//
// Safely deletes the array, including double free protection.
//
// Parameters
// ----------
// array : *Array<T>
//     The array to be deleted.
//
// Returns
// -------
// bool: Returns true on success.
//
bool fn(delete)(Self* array) {
    ensure(array, false);

    free(array->data);
    free(array);
    return true;
}

// Array >> get(array: *Array<T>, index: size_t) -> *T
//
// Safely gets a pointer to the element at the specified index.
//
// Parameters
// ----------
// array : *Array<T>
//     The array from which to get the element.
// index : size_t
//     The index of the element to retrieve.
//
// Returns
// -------
// *T: A pointer to the element at the specified index.
//     If the index is out of bounds, returns NULL.
//
T * fn(get)(Self* array, size_t index) {
    ensure(array, NULL);
    ensure(index < array->_size, NULL);

    return &array->data[index];
}

// Array >> set(array: *Array<T>, index: size_t, value: T) -> bool
//
// Safely sets the element at the specified index to the given value.
//
// Parameters
// ----------
// array : *Array<T>
//     The array in which to set the element.
// index : size_t
//     The index of the element to set.
// value : T
//     The value to set at the specified index.
//
// Returns
// -------
// bool: Returns true on success, false if the index is out of bounds.
//
bool fn(set)(Self* array, size_t index, T value) {
    ensure(array, false);
    ensure(index < array->_size, false);

    array->data[index] = value;
    return true;
}

// Array >> replace(array: *Array<T>, index: size_t, value: T) -> *T
//
// Safely replaces the element at the specified index with the given value
// and returns a pointer to the old value.
//
// Parameters
// ----------
// array : *Array<T>
//     The array in which to replace the element.
// index : size_t
//     The index of the element to replace.
// value : T
//     The new value to set at the specified index.
//
// Returns
// -------
// *T: A pointer to the old value at the specified index.
//     If the index is out of bounds, returns NULL.
//
T * fn(replace)(Self * array, size_t index, T value) {
    ensure(array, NULL);
    ensure(index < array->_size, NULL);

    T * old_value = fn(get)(array, index);
    fn(set)(array, index, value);

    return old_value;
}

// Array >> size(array: *Array<T>) -> size_t
//
// Returns the size of the array.
//
// Parameters
// ----------
// array : *Array<T>
//     The array whose size is to be retrieved.
//
// Returns
// -------
// size_t: The number of elements in the array.
//
size_t fn(size)(Self * array) {
    ensure(array, 0);
    return array->_size;
}

// Array >> print(array: *Array<T>, print: (T) -> void) -> void
// 
// Prints the array on terminal.
//
// Parameters
// ----------
// array : *Array<T>
//     The array to be printed.
//
void fn(print)(Self * array) {
    ensure(array,);

    printf("[");
    for (size_t i = 0; i < array->_size; i++) {
        PRINT_T(array->data[i]);
        if (i < array->_size - 1) printf(", ");
    }
    printf("]");
}

// Array >> println(array: *Array<T>, print: (T) -> void) -> void
//
// Prints the array on terminal followed by a newline.
//
// Parameters
// ----------
// array : *Array<T>
//     The array to be printed.
//
void fn(println)(Self * array) {
    fn(print)(array);
    printf("\n");
}

// Array >> debug(array: *Array<T>) -> void
//
// Prints the debug representation of the array.
//
// Parameters
// ----------
// array : *Array<T>
//     The array to be debugged.
//
void fn(debug)(Self * array) {
    if (not array) {
        printf("Array<%s> { NULL }\n", TOSTRING(T));
        return;
    }

    printf("Array<%s> {\n}", TOSTRING(T));
    printf("  size: %zu,\n", array->_size);
    printf("  data: "); fn(println)(array);
    printf("}\n");

}