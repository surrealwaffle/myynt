#ifndef MYYNT__PACKAGE_HPP
#define MYYNT__PACKAGE_HPP

#include <tuple>

#include <yymp/typelist_fwd.hpp>
#include <yymp/join.hpp>

namespace myynt {
    
    template<class... Ts>
    struct package {
        std::tuple<Ts...> pack;
        
        package() = default;
        package(package const&) = default;
        package(package&&) = default;
        
        package& operator=(package const&) = default;
        package& operator=(package&&) = default;
        
        constexpr package(Ts&&... ts) : pack(std::forward<Ts>(ts)...) { }
    };
    
    struct package_unpacker {
        
        template< class T >
        constexpr std::tuple<T&&> operator()(T&& t) const noexcept { 
            return std::forward_as_tuple(std::forward<T>(t)); 
        }
        
        template< class... Ts >
        constexpr auto operator()(package<Ts...> const& p) const noexcept {
            return std::apply(*this, p.pack);
        }
        
        template< class... Ts >
        constexpr auto operator()(package<Ts...>&& p) const noexcept {
            return std::apply(*this, std::move(p.pack));
        }
        
        template< class... Ts >
        constexpr auto operator()(Ts&&... ts) const noexcept {
            return std::tuple_cat(this->operator()(std::forward<Ts>(ts))...);
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
        using type = yymp::typelist<T>;
    };
    
    template< class... Ts >
    struct unpack_modules<package<Ts...>> {
        using type = typename yymp::join<
            typename unpack_module<Ts>::type...
        >::type;
    };
    
    template< class... Ts >
    struct unpack_modules {
        using type = typename yymp::join<
            typename unpack_module<Ts>::type...
        >::type;
    };
    
}

#endif // MYYNT__PACKAGE_HPP