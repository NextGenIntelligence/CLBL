#ifndef CLBL_FREE_FUNCTION_H
#define CLBL_FREE_FUNCTION_H

#include "callable.h"

namespace clbl {

    //dispatch failure case for free functions
    template<typename Failure>
    struct free_function {

        template<typename... T>
        free_function(T...) {}

        free_function() {}

        template<typename... T>
        auto operator()(T...) { static_assert(sizeof...(T) < 0, "Not a function."); }

        template<typename T = void>
        auto operator()(...) { static_assert(sizeof(T) < 0, "Not a function."); }
    };

    //specialization for free function types
    template<typename Return, typename... Args>
    struct free_function<Return(Args...)> : callable<free_function<Return(Args...)>, Return(Args...)> {

        free_function(Return(*f_ptr)(Args...)) : function_ptr(f_ptr) {}
        inline Return operator()(Args... a) const { return (*function_ptr)(a...); }

    private:
        Return(*function_ptr)(Args...);
    };

    template<typename Return, typename... Args>
    struct free_function<Return(Args..., ...)> : callable<free_function<Return(Args..., ...)>, Return(Args..., ...)> {

        free_function(Return(*p)(Args..., ...)) : function_ptr(p) {}
        inline Return operator()(Args... args, ...) const { return (*function_ptr)(args..., ...); }

    private:
        Return(*function_ptr)(Args..., ...);
    };
}

#endif