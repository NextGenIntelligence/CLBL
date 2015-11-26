#ifndef TEST_H
#define TEST_H

#include "callable.h"
#include "void_definitions.h"
#include "int_definitions.h"
#include "int_char_definitions.h"

namespace clbl { namespace tests {
        struct some_type {};
        auto some_callable_object = [](some_type) {};
        extern inline void some_function(some_type) {}
}}

#define STATIC_TEST BOOST_HANA_CONSTANT_CHECK
#define TEST BOOST_HANA_CHECK

#endif