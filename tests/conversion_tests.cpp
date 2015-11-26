#include "test.h"
#include "func.h"
#include "int_char_definitions.h"

#include<functional>
#include<iostream>

using namespace clbl::tests;
using namespace clbl;

auto int_char_std_function_result(std::function<const char*(int, char)> f) {
    return f(1, 'c');
}

void call_std_function(std::function<void()> f) {
    f();
}

void conversion_tests() {

    struct mutable_struct {
        int value;
        mutable_struct(int i) : value(i) {}

        void increment() {
            ++value;
        }
    };
    {
        //testing object reference preservation when converted to std::function
        auto mutable_object = mutable_struct{ 0 };
        auto f = func(&mutable_object, &mutable_struct::increment);
        auto stdf = f.as<std::function>();

        f();
        stdf();
        call_std_function(stdf);

        TEST(mutable_object.value == 3);
    }
    {
        //testing cv-correctness of clbl callables converted to std::function
        auto overloaded_object = overloaded_int_char_struct{};

        auto normal = force<const char*(int, char)>::func(&overloaded_object);
        auto c = force<const char*(int, char) const>::func(&overloaded_object);
        auto v = force<const char*(int, char) volatile>::func(&overloaded_object);
        auto cv = force<const char*(int, char) const volatile>::func(&overloaded_object);

        auto stdn = normal.as<std::function>();
        auto stdc = c.as<std::function>();
        auto stdv = v.as<std::function>();
        auto stdcv = cv.as<std::function>();

        TEST(stdn(1, 'c') == test_id::overloaded_int_char_struct_op);
        TEST(stdc(1, 'c') == test_id::overloaded_int_char_struct_op_c);
        TEST(stdv(1, 'c') == test_id::overloaded_int_char_struct_op_v);
        TEST(stdcv(1, 'c') == test_id::overloaded_int_char_struct_op_cv);

        TEST(int_char_std_function_result(stdn) == test_id::overloaded_int_char_struct_op);
        TEST(int_char_std_function_result(stdc) == test_id::overloaded_int_char_struct_op_c);
        TEST(int_char_std_function_result(stdv) == test_id::overloaded_int_char_struct_op_v);
        TEST(int_char_std_function_result(stdcv) == test_id::overloaded_int_char_struct_op_cv);
    }
    {
        //testing cv-correctness of nested clbl callbles converted to std::function
        auto overloaded_object = overloaded_int_char_struct{};

        auto normal = force<const char*(int, char)>::func(&overloaded_object);
        auto c = force<const char*(int, char) const>::func(&overloaded_object);
        auto v = force<const char*(int, char) volatile>::func(&overloaded_object);
        auto cv = force<const char*(int, char) const volatile>::func(&overloaded_object);

        auto nested_normal = force<const char*(int, char)>::func(&normal);
        auto nested_c = force<const char*(int, char)>::func(&c);
        auto nested_v = force<const char*(int, char)>::func(&v);
        auto nested_cv = force<const char*(int, char)>::func(&cv);

        auto stdn = nested_normal.as<std::function>();
        auto stdc = nested_c.as<std::function>();
        auto stdv = nested_v.as<std::function>();
        auto stdcv = nested_cv.as<std::function>();

        TEST(stdn(1, 'c') == test_id::overloaded_int_char_struct_op);
        TEST(stdc(1, 'c') == test_id::overloaded_int_char_struct_op_c);
        TEST(stdv(1, 'c') == test_id::overloaded_int_char_struct_op_v);
        TEST(stdcv(1, 'c') == test_id::overloaded_int_char_struct_op_cv);

        TEST(int_char_std_function_result(stdn) == test_id::overloaded_int_char_struct_op);
        TEST(int_char_std_function_result(stdc) == test_id::overloaded_int_char_struct_op_c);
        TEST(int_char_std_function_result(stdv) == test_id::overloaded_int_char_struct_op_v);
        TEST(int_char_std_function_result(stdcv) == test_id::overloaded_int_char_struct_op_cv);
    }
}