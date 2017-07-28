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
#include <yymp/bind.hpp>            // yymp::bind
#include <yymp/join.hpp>            // yymp::join
#include <yymp/filter.hpp>          // yymp::filter_duplicates
#include <yymp/setops.hpp>          // yymp::intersect
#include <yymp/traits.hpp>          // yymp::any_of

#include <type_traits>           

namespace myynt::tags {
    
    struct first;
    
    struct last;
    
    struct module;
    
    template< class T, class Category, class = std::void_t<> >
    struct tags_of_category {
        using type = yymp::typelist<>;
    };
    
    template< class T >
    struct tags_of_category<T, first, std::void_t<typename T::myynt_first>> {
        using type = typename yymp::filter_duplicates<
            typename yymp::retain_as_typelist<typename T::myynt_first>::type
        >::type;
    };
    
    template< class T, class Void >
    struct tags_of_category<T, module, Void> {
        using type = yymp::typelist<T>;
    };
    
    template< class T >
    struct tags_of_category<T, module, std::void_t<typename T::myynt_tags>> {
        using type = typename yymp::filter_duplicates<
            typename yymp::join<
                yymp::typelist<T>,
                typename yymp::retain_as_typelist<typename T::myynt_tags>::type
            >::type
        >::type;
    };
    
    template< class T >
    using tags_of = tags_of_category<T, module>;
    
    template< class T, class Tag >
    struct is_tag_of : yymp::any_of<Tag, typename tags_of<T>::type> { };
    
    template< class T >
    struct tags_of_category<T, last, std::void_t<typename T::myynt_last>> {
        using type = typename yymp::filter_duplicates<
            typename yymp::retain_as_typelist<typename T::myynt_last>::type
        >::type;
    };
    
    template< class S, class T, class Category >
    struct belongs_to_category
        : yymp::any_where<
            yymp::bind<is_tag_of<S, yymp::var>>::template generic,
            typename tags_of_category<T, Category>::type
        >::type { };
    
    template< class T, class Tags >
    struct get_first_representative {
        using representatives = typename yymp::filter<
            yymp::bind<is_tag_of<T, yymp::var>>::template generic, 
            Tags
        >::type;
        
        using type = typename yymp::first<representatives>::type;
    };
    
}

#endif // MYYNT__TAGS_HPP