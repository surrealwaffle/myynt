#ifndef MYYNT__PACKAGE_HPP
#define MYYNT__PACKAGE_HPP

#include <tuple>
#include <type_traits>

#include <yymp/typelist_fwd.hpp>
#include <yymp/join.hpp>

namespace myynt {
    
    template<class... Ts>
    struct package {
        std::tuple<Ts...> pack;
        
        package()               = default;
        package(package const&) = default;
        package(package&&)      = default;
        
        package& operator=(package const&)  = default;
        package& operator=(package&&)       = default;
        
        template<
            class... Args,
            class = typename std::enable_if<
                sizeof...(Args) >= 1 &&
                std::is_constructible<decltype(pack), Args...>::value
            >::type
        > constexpr package(Args&&... args)
            noexcept(std::is_nothrow_constructible<decltype(pack), Args...>::value)
            : pack(std::forward<Args>(args)...) { }
    };
    
    template< class T, class = typename std::decay<T>::type >
    struct is_package : std::false_type { };
    
    template< class T, class... Ts >
    struct is_package<T, package<Ts...>> : std::true_type { };
    
    template<class... Ts>
    package(Ts&&...) -> package<Ts...>;
    
    struct package_unpacker {
        constexpr std::tuple<> operator()() const noexcept {
            return {};
        }
        
        template<class T> 
        constexpr auto operator()(T&& t) const noexcept 
            -> typename std::enable_if<!is_package<T>::value, std::tuple<T&&>>::type { 
            return std::forward_as_tuple(std::forward<T>(t)); 
        }
        
        template< class... Ts >
        constexpr auto operator()(package<Ts...> const& p) const noexcept 
            -> decltype(std::apply(*this, p.pack)) {
            return std::apply(*this, p.pack);
        }
        
        template< class... Ts >
        constexpr auto operator()(package<Ts...>&& p) const noexcept
            -> decltype(std::apply(*this, std::move(p.pack))) {
            return std::apply(*this, std::move(p.pack));
        }
        
        template< class A, class B, class... Ts >
        constexpr auto operator()(A&& a, B&& b, Ts&&... ts) const noexcept {
            return std::tuple_cat(
                this->operator()(std::forward<A>(a)),
                this->operator()(std::forward<B>(b)),
                this->operator()(std::forward<Ts>(ts))...);
        }
    };
    
    template< class... Ts >
    constexpr auto unpack_packages(Ts&&... ts) noexcept {
        return package_unpacker{}(std::forward<Ts>(ts)...);
    }
    
    template< class... Ts >
    struct unpack_modules;
    
    template< class T >
    struct unpack_modules<T> {
        using type = yymp::typelist<typename std::decay<T>::type>;
    };
    
    template< class... Ts >
    struct unpack_modules<package<Ts...>> {
        using type = typename yymp::join<
            typename unpack_modules<typename std::decay<Ts>::type>::type...
        >::type;
    };
    
    template< class... Ts >
    struct unpack_modules {
        using type = typename yymp::join<
            typename unpack_modules<Ts>::type...
        >::type;
    };
    
}

#endif // MYYNT__PACKAGE_HPP