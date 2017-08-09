//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MYYNT__COMPLETE_HPP
#define MYYNT__COMPLETE_HPP

#include <tuple>        // std::tuple
#include <utility>      // std::forward, std::index_sequence
#include <type_traits>  // std::is_constructible, std::is_nothrow_constructible, std::enable_if

namespace myynt {
    
    template< template<class...> class ModuleTemplate, class... CArgs >
    struct complete_type;
    
    template< class Manager, class TypeToComplete >
    struct make_complete;
    
    template< template<class...> class ModuleTemplate, class... CArgs >
    struct complete_type {
        using container = std::tuple<CArgs...>;
        
        complete_type() = delete;
        complete_type(complete_type const&) = delete;
        complete_type(complete_type&&) = delete;
        
        complete_type& operator=(complete_type const&) = delete;
        complete_type& operator=(complete_type&&) = delete;
        
        constexpr complete_type(CArgs&&... args) noexcept(std::is_nothrow_constructible<container, CArgs...>::value)
            : constructor_arguments(std::forward<CArgs>(args)...) { }
        
        template<class Manager, std::size_t... I>
        constexpr ModuleTemplate<Manager> move_construct_to(std::index_sequence<I...>) 
            noexcept(std::is_nothrow_constructible<ModuleTemplate<Manager>, CArgs...>::value) {
            return ModuleTemplate<Manager>{std::forward<CArgs>(std::get<I>(constructor_arguments))...};
        }
        
        template<
            class Manager,
            typename = typename std::enable_if<std::is_constructible<ModuleTemplate<Manager>, CArgs...>::value>::type
        > constexpr operator ModuleTemplate<Manager>() 
            noexcept(std::is_nothrow_constructible<ModuleTemplate<Manager>, CArgs...>::value) {
            return move_construct_to<Manager>(std::make_index_sequence<sizeof...(CArgs)>());
        }
        
        container constructor_arguments;
    };
    
    template< template<class...> class ModuleTemplate, class... CArgs >
    constexpr complete_type<ModuleTemplate, CArgs...> complete(CArgs&&... constructor_args)
        noexcept(std::is_nothrow_constructible<complete_type<ModuleTemplate, CArgs...>, CArgs...>::value) {
        return {std::forward<CArgs>(constructor_args)...};
    }
    
    template< class Manager, class T >
    struct make_complete {
        using type = T;
    };
    
    template< class Manager, template<class...> class ModuleTemplate, class... CArgs >
    struct make_complete<Manager, complete_type<ModuleTemplate, CArgs...>> {
        using type = ModuleTemplate<Manager>;
    };
    
}

#endif // MYYNT__COMPLETE_HPP