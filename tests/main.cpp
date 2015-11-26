#include "test.h"
#include "CLBL/func.h"
#include "void_definitions.h"
#include "int_definitions.h"
#include "int_char_definitions.h"

#include <string>
#include <iostream>
#include <sstream>
#include <memory>

using namespace clbl::tests;
using namespace clbl;

void void_tests();
void int_tests();
void int_char_tests();

void const_void_tests();
void const_int_tests();
void const_int_char_tests();

void constable_void_tests();
void constable_int_tests();
void constable_int_char_tests();

void volatile_void_tests();
void volatile_int_char_tests();
void const_volatile_int_char_tests();

void shared_ptr_tests();
void reference_tests();
void overload_tests();
void conversion_tests();

int main() {

    void_tests();
    int_tests();
    int_char_tests();

    const_void_tests();
    const_int_tests();
    const_int_char_tests();

    constable_void_tests();
    constable_int_tests();
    constable_int_char_tests();

    volatile_void_tests();
    volatile_int_char_tests();
    const_volatile_int_char_tests();
    
    shared_ptr_tests();
    reference_tests();
    overload_tests();
    conversion_tests();
}