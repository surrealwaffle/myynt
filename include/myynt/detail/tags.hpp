//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file tags.hpp
 * \brief Contains tag categories and tag list retrieval.
 */

#ifndef MYYNT__TAGS_HPP
#define MYYNT__TAGS_HPP

#include <yymp/typelist_fwd.hpp>    // yymp::typelist, yymp::retain_as_typelist, yymp::is_not_empty
#include <yymp/setops.hpp>          // yymp::intersect

#include <type_traits>

namespace myynt::tags {
    
    struct first;
    
    struct intermediate;
    
    struct last;
    
    template< class T, class Category, class = std::void_t<> >
    struct tags_of_category {
        using type = yymp::typelist<>;
    };
    
    template< class T >
    struct tags_of_category<T, first, std::void_t<typename T::myynt_first>> {
        using type = typename yymp::retain_as_typelist<typename T::myynt_first>::type;
    };
    
    template< class T >
    struct tags_of_category<T, intermediate, std::void_t<typename T::myynt_tags>> {
        using type = typename yymp::retain_as_typelist<typename T::myynt_tags>::type;
    };
    
    template< class T >
    using tags_of = tags_of_category<T, intermediate>;
    
    template< class T >
    struct tags_of_category<T, last, std::void_t<typename T::myynt_last>> {
        using type = typename yymp::retain_as_typelist<typename T::myynt_last>::type;
    };
    
    template< class S, class T, class Category >
    struct belongs_to_category
        : yymp::is_not_empty<
            typename yymp::intersection<
                typename tags_of<S>::type,
                typename tags_of_category<T, Category>::type
            >::type
        > { };
}

#endif // MYYNT__TAGS_HPP