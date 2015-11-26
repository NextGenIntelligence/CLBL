#include "test.h"
#include "CLBL/func.h"
#include "void_definitions.h"

using namespace clbl::tests;
using namespace clbl;

void void_tests() {
    void_struct void_object{};

    auto f = func(&void_object);
    TEST(f() == test_id::void_struct_op);

    auto g = func(&void_func);
    TEST(g() == test_id::void_func);

    auto h = func(&void_object, &decltype(void_object)::func);
    TEST(h() == test_id::void_struct_func);

    STATIC_TEST(f.return_type == g.return_type);
    STATIC_TEST(h.return_type == g.return_type);
    STATIC_TEST(f.return_type == hana::type_c<const char*>);

    STATIC_TEST(f.args_tuple == g.args_tuple);
    STATIC_TEST(h.args_tuple == g.args_tuple);
    STATIC_TEST(f.args_tuple == hana::make_tuple());

    STATIC_TEST(f.arity == g.arity);
    STATIC_TEST(h.arity == g.arity);
    STATIC_TEST(h.arity == 0_c);

    STATIC_TEST(!f.can_call(1));
    STATIC_TEST(!g.can_call(1));
    STATIC_TEST(!h.can_call(1));
    STATIC_TEST(f.can_call());
    STATIC_TEST(g.can_call());
    STATIC_TEST(h.can_call());

    STATIC_TEST(h.matches(g));
    STATIC_TEST(g.matches(f));
    STATIC_TEST(!g.matches(func(&int_char_func)));
    STATIC_TEST(!f.matches(func(&some_callable_object)));
}