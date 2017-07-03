//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef YYMP__TYPELIST_FWD_HPP
#define YYMP__TYPELIST_FWD_HPP

#include <type_traits>

namespace yymp {

/**
 * \brief Represents a compile-time sequence of types.
 *
 * \tparam Ts The parameter pack representing the sequence.
 */
template< class... Ts >
struct typelist {
    using type = typelist;
};

/** \brief A compile-time bool constant whose `value` is \c true if and only if \a T is a \ref yymp::typelist "typelist". */
template< class T >
struct is_typelist;

/** \brief Defines `type` as `T` if `T` is a \ref yymp::typelist "typelist", otherwise it is defined as `typelist<T>`. */
template< class T >
struct retain_as_typelist;

/** \brief A compile-time constant whose `value` is the length of the type sequence \a Ts. */
template< class TypeList >
struct size;

/** \brief A compile-time bool constant whose `value` is \c true if and only if \a TypeList is the empty \ref yymp::typelist "typelist". */
template< class TypeList >
struct is_empty;

/** \brief A compile-time bool constant whose `value` is \c true if and only if \a TypeList is not the empty \ref yymp::typelist "typelist". */
template< class TypeList >
struct is_not_empty;

/* ***************************
 * IMPLEMENTATION
 */

template< class T >
struct is_typelist : std::false_type { };

template< class... Ts >
struct is_typelist< typelist< Ts... > > : std::true_type { };

template< class T >
struct retain_as_typelist {
    using type = typelist< T >;
};

template< class... Ts >
struct retain_as_typelist< typelist< Ts... > > { 
    using type = typelist< Ts... >;
};

template< class... Ts >
struct size< typelist< Ts... > > : std::integral_constant< decltype(sizeof...(Ts)), sizeof...(Ts) > { };

template< class... Ts >
struct is_empty< typelist< Ts... > > : std::false_type { };

template< >
struct is_empty< typelist< > > : std::true_type { };

template< class... Ts >
struct is_not_empty< typelist< Ts... > > : std::true_type { };

template< >
struct is_not_empty< typelist< > > : std::false_type { };

}

#endif // YYMP__TYPELIST_FWD_HPP
