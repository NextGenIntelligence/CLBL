#ifndef INT_DEFINITIONS_H
#define INT_DEFINITIONS_H

namespace clbl { namespace tests {

    namespace test_id { constexpr auto int_func = "int_func(int i)"; }
    extern inline const char* int_func(int i) { return test_id::int_func; }

    namespace test_id { constexpr auto int_struct_func = "int_struct::func(int)"; }
    namespace test_id { constexpr auto int_struct_op = "int_struct::operator()(int)"; }
    struct int_struct {
        inline const char* func(int i) { return test_id::int_struct_func; }
        inline const char* operator()(int i) { return test_id::int_struct_op; }
    };

    namespace test_id { constexpr auto const_int_struct_func = "const_int_struct::func(int) const"; }
    namespace test_id { constexpr auto const_int_struct_op = "const_int_struct::operator()(int) const"; }
    struct const_int_struct {
        inline const char* func(int i) const { return test_id::const_int_struct_func; }
        inline const char* operator()(int i) const { return test_id::const_int_struct_op; }
    };
}}

#endif