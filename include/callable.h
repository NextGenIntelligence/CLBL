#ifndef CLBL_CALLABLE_H
#define CLBL_CALLABLE_H

#include <boost/hana.hpp>
#include <type_traits>

namespace hana = boost::hana;
using namespace hana::literals;

namespace clbl {

    /*
    using the "func" function in this file, we can statically
    wrap functions and member functions/function objects into
    signature-deduced callable objects that conceptually *eliminates*
    the semantic barrier between member functions and free functions,
    without the overhead of the dynamic type erasure used in the
    <functional> library. Currently, the tests in the bottom half
    of the file are the best documentation available.

    There are three ways to call func:
    1. Pass a function pointer
    2. Pass a pointer to an object as the first argument, and pass a member function pointer as the second argument
    3. Pass a pointer to an object with an overloaded operator() (which includes lambdas)

    Things to note/remember:
    - std::shared_ptr works for objects. Note that (currently) pointers are copied rather than moved.
    - You still have to help the compiler peform overload resolution, by (for instance) static_casting the function pointer
    - Default arguments are not part of the function signature, so you cannot use them
    - be careful const correctness, because my error messages don't yet help you with that
    - this code is very new and still under development
    - templated functions not yet tested
    - volatile not yet supported
    - may add support for hana::partial (or similar) in the future
    */

    namespace detail {

        //operator() call checker used by can_call_impl below
        template<typename T, typename... Args>
        auto can_call_v = hana::is_valid([](auto&&... args) 
                -> decltype((std::declval<T>()).operator()(std::forward<Args>(args)...)) {}
            );

        template<typename CallableDerived>
        struct can_call_impl {
            
            /*
            Returns a true type when passing arguments to the CallableDerived operator()
            does not result in a compiler error, otherwise a returns a false type
            */

            template<typename... Args>
            inline constexpr auto can_call(Args&&... args) { 
                return can_call_v<CallableDerived, Args...>(std::forward<Args>(args)...); 
            }
        };

        template<typename CallableDerived>
        struct matches_impl {

            /*
            performs a strict comparison against another callable type. Returns a 
            true type if the return type and arguments match exactly 
            */

            template<typename OtherCallable>
            inline constexpr auto matches(OtherCallable c) { 
                return hana::bool_<std::is_same<typename OtherCallable::type, typename CallableDerived::type>::value>{};
            }
        };

        template<typename CallableDerived>
        struct as_impl {

            //convert a clbl callable object to a type-erased std::function object

            template<template<class> class TypeErasedFunctionTemplate>
            inline auto as() {
                return TypeErasedFunctionTemplate<typename CallableDerived::type>{*static_cast<CallableDerived*>(this)};
            };
        };
    }

    //dispatch failure case
    template<typename, typename Bad>
    struct callable { static_assert(sizeof(Bad) < 0, "Not a valid callable type."); };

    //callable is the end result of a func(...) call
    template<typename Derived, typename Return, typename... Args>
    struct callable<Derived, Return(Args...)>
        : detail::can_call_impl<Derived>,
          detail::matches_impl<Derived>,
          detail::as_impl<Derived> {

        using type = Return(Args...);
        using args_t = hana::tuple<Args...>;
        using return_t = Return;

        static constexpr auto args_tuple = hana::tuple_t<Args...>;
        static constexpr auto return_type = hana::type_c<Return>;
        static constexpr auto arity = hana::int_c<sizeof...(Args)>;
    };

    template<typename Derived, typename Return, typename... Args>
    struct callable<Derived, Return(Args..., ...)>
        : detail::can_call_impl<Derived>,
        detail::matches_impl<Derived>,
        detail::as_impl<Derived> {

        using type = Return(Args..., ...);
        using args_t = decltype(hana::make_tuple());
        using return_t = Return;

        static constexpr auto args_tuple = hana::make_tuple();
        static constexpr auto return_type = hana::type_c<Return>;
        static constexpr auto arity = 0_c;
    };
}

#endif