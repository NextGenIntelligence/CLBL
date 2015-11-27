#ifndef CLBL_FUNC_H
#define CLBL_FUNC_H

#include "callable.h"
#include "free_function.h"
#include "member_function.h"
#include "member_function_decay.h"

#include <type_traits>

namespace clbl {
    namespace detail {

        /*template<bool... Bools>
        struct dispatch {
            template<typename... Args>
            static constexpr auto result(Args... a) {
                constexpr auto bools = hana::make_tuple(hana::bool_<Bools>{}...);
                //todo: make this compile faster - may prefer to handroll specializations
                auto args = hana::make_tuple(a...);
                auto zipped = hana::zip(bools, args);
                auto result = hana::filter(zipped, [](auto element) {return element[0_c];});
                return result[0_c][1_c];
            }
        };*/

        //todo compare above dispatch implementation with this one
        template<bool, bool, bool, bool>
        struct dispatch {};

        template<bool R1, bool R2, bool R3>
        struct dispatch<true, R1, R2, R3> {
            template<typename Result, typename... Args>
            static constexpr auto result(Result r, Args...) {
                return r;
            }
        };

        template<bool R2, bool R3>
        struct dispatch<false, true, R2, R3> {
            template<typename T0, typename Result, typename... Args>
            static constexpr auto result(T0 t0, Result r, Args...) {
                return r;
            }
        };

        template<bool R3>
        struct dispatch<false, false, true, R3> {
            template<typename T0, typename T1, typename Result, typename T3>
            static constexpr auto result(T0 t0, T1 t1, Result r, T3 t3) {
                return r;
            }
        };

        template<>
        struct dispatch<false, false, false, true> {
            template<typename T0, typename T1, typename T2, typename Result>
            static constexpr auto result(T0 t0, T1 t1, T2 t2, Result r) {
                return r;
            }
        };

        struct null_function_object { void operator()() const volatile {} };

        auto has_call_operator_impl = hana::is_valid([](auto arg)->decltype(&decltype(arg)::operator()) {});

        template<typename T>
        constexpr bool has_call_operator = decltype(has_call_operator_impl(std::declval<T>()))::value;

        auto can_dereference_impl = hana::is_valid([](auto arg)->decltype(*arg) {});

        template<typename T>
        constexpr bool can_dereference = decltype(can_dereference_impl(std::declval<T>()))::value;

        template<typename TPtr, typename TMemberFnPtr>
        struct dispatch_context {

            static constexpr auto is_pointer_type = can_dereference<TPtr>;

            using underlying_type = std::conditional_t<
                                        is_pointer_type,
                                        std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<TPtr>())> >,
                                        std::remove_cv_t<std::remove_reference_t<TPtr> >
                                    >;
            
            static constexpr auto is_function_ptr = std::is_function<std::remove_pointer_t<TPtr> >::value;
            static constexpr auto is_member_function_ptr = std::is_member_function_pointer<TMemberFnPtr>::value;
            static constexpr auto is_function_object_ptr = !is_function_ptr
                                                        && !is_member_function_ptr
                                                        && detail::has_call_operator<underlying_type>
                                                        && is_pointer_type;

            static constexpr auto is_valid = is_function_ptr || is_member_function_ptr || is_function_object_ptr;
            static constexpr auto is_function_object = !is_pointer_type && detail::has_call_operator<underlying_type>;

            using function_object_type = std::conditional_t<is_function_object_ptr,
                                                            underlying_type,
                                                            detail::null_function_object>;

            using call_operator_type = decltype(&function_object_type::operator());
            static constexpr auto call_operator_ptr = &function_object_type::operator();
        };
    }

    struct invalid_arguments_pass_a_pointer_instead {};

    template<typename TPtr, typename TMemberFnPtr = nullptr_t>
    constexpr auto func(TPtr t, TMemberFnPtr ptr = nullptr) {

        using context = detail::dispatch_context<TPtr, TMemberFnPtr>;

        static_assert(context::is_valid,
            "clbl::func only accepts function pointers, raw object pointers, "
            "std::shared_ptr, and types with the same semantics as std::shared_ptr "
            "as the first argument. If the first argument to clbl::func is not a "
            "function pointer, then it must be an object pointer. clbl::func will "
            "create a callable object using the object's operator(), unless you "
            "supply a member function pointer as the second argument.");

        static_assert(context::is_valid && !context::is_function_object,
            "Don't try to pass an entire object to clbl::func (this includes "
            "lambdas). Pass a pointer instead. You are responsible for the "
            "lifetime of the object. ");

        using underlying_type = typename context::underlying_type;
        using function_object_type = typename context::function_object_type;
        using call_operator_type = typename context::call_operator_type;

        /*
        free_function_result, member_function_result, and function_object_result are all objects
        with an operator() defined. However, we use dispatch<...>::result() to only return
        the correct object according to the context determined by the arguments. The incorrect 
        objects have static_assert failures in their templated operator() bodies, which means 
        the user will never be passed an incorrect result due to a logic error manifesting inside 
        clbl::func (assuming the user tries to call the result).
        */

        auto free_function_result = free_function<std::remove_pointer_t<TPtr> >{ t };

        auto member_function_result = member_function<
                underlying_type,
                TPtr,
                TMemberFnPtr,
                member_function_decay<TMemberFnPtr>
            >{ t, ptr };

        auto function_object_result = member_function<
                function_object_type,
                TPtr,
                call_operator_type,
                member_function_decay<call_operator_type>
            >{ t, context::call_operator_ptr };

        return detail::dispatch<
            context::is_function_ptr,
            context::is_member_function_ptr,
            context::is_function_object_ptr,
            !context::is_valid
        >::result(
            free_function_result,
            member_function_result,
            function_object_result,
            invalid_arguments_pass_a_pointer_instead{}
        );
    }

    /*
    A real use case for "Abominable Function Types" mentioned in C++17 proposal P0172R0.
    allows a user to choose an operator() overload (including CV qualifiers!)
    */
    template<typename AbominableFunctionType>
    struct force {

        template<typename TPtr>
        static constexpr auto func(TPtr t) {
            using underlying_type = std::remove_cv_t <std::remove_reference_t<decltype(*std::declval<TPtr>())> >;
            using forced_overload_type = AbominableFunctionType underlying_type::*;
            return clbl::func(t, static_cast<forced_overload_type>(&underlying_type::operator()));
        }
    };
}

#endif