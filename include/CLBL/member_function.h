#ifndef CLBL_MEMBER_FUNCTION_H
#define CLBL_MEMBER_FUNCTION_H

#include <type_traits>

namespace clbl {

    //dispatch failure case for member functions
    template<typename, typename, typename, typename Failure>
    struct member_function {

        member_function() {}

        template<typename... T>
        member_function(T...) {}

        template<typename... T>
        void operator()(T...) {
            static_assert(sizeof...(T) < 0, "Not a member function.");
        }

        template<typename... T>
        void operator()(T..., ...) {
            static_assert(sizeof...(T) < 0, "Not a member function.");
        }
    };

    template<
        typename UnderlyingType, 
        typename TPtr, 
        typename MemberFunctionPointerType, 
        typename Return, 
        typename... Args
    >
    struct member_function<
        UnderlyingType, 
        TPtr, 
        MemberFunctionPointerType, 
        Return(UnderlyingType::*)(Args...)
    >
    //CRTP to callable
    : callable<
        member_function<UnderlyingType, TPtr, MemberFunctionPointerType, Return(UnderlyingType::*)(Args...)>, 
        Return(Args...)
    > {

        member_function(TPtr o_ptr, MemberFunctionPointerType f_ptr)
            : object_ptr(o_ptr),
            member_function_ptr(f_ptr)
        {}

        /*
        Using SFINAE to provide the proper operator() signature object. We assume here that 
        the MemberFunctionPointerType is compatible with the pointed-to object. If it isn't,
        and the compiler attempts to instantiate operator(), there will be a compile error.
        This does not (currently) account for ref-qualifiers
        */

        template<typename T = UnderlyingType, typename = std::enable_if_t<
            !std::is_const<T>::value && !std::is_volatile<T>::value>>
        inline Return operator()(Args... a) {
            /*
            Did a compiler error bring you here? Make sure you're 
            not calling a non-const function on a const object
            */
            return ((*object_ptr).*member_function_ptr)(a...);
        }

        template<typename T = UnderlyingType, typename = std::enable_if_t<
            std::is_const<T>::value && !std::is_volatile<T>::value> >
        inline Return operator()(Args... a) const {
            return ((*object_ptr).*member_function_ptr)(a...);
        }

        template<typename T = UnderlyingType, typename = std::enable_if_t<
            !std::is_const<T>::value && std::is_volatile<T>::value> >
        inline Return operator()(Args... a) volatile {
            return ((*object_ptr).*member_function_ptr)(a...);
        }

        template<typename T = UnderlyingType, typename = std::enable_if_t<
            std::is_const<T>::value && std::is_volatile<T>::value> >
        inline Return operator()(Args... a) const volatile {
            return ((*object_ptr).*member_function_ptr)(a...);
        }

    private:
        TPtr object_ptr;
        MemberFunctionPointerType member_function_ptr;
    };
}

#endif