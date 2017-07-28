//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file traits.hpp
 * \brief Properties and relations between modules and messages.
 */
 
#ifndef MYYNT__TRAITS_HPP
#define MYYNT__TRAITS_HPP

#include <utility>		// std::declval
#include <type_traits> 	// std::void_t, std::false_type, std::true_type, std::conditional

#include <yymp/typelist_fwd.hpp>    // group_suppress
#include <yymp/bind.hpp>            // yymp::bind
#include <yymp/group.hpp>           // yymp::group_by, yymp::group, yymp::get_group*

#include <myynt/detail/tags.hpp>

namespace myynt {
	
    /** \brief Indicates that an emitter is a callback emitter. */
    struct callback;
    
	/** \brief Inherits from `bool` constants to indicate if a \a Module member function `myynt_Process` exists that takes values of type `Message&&`. */
	template< class Message, class Module, class = std::void_t<> >
	struct is_message_processable;
    
    /** \brief Inherits from `bool` constants to indicate if a \a Module member function `myynt_RegisterManager` exists that takes values of type `Manager&&`. */
    template< class Manager, class Module, class = std::void_t<> >
    struct is_module_registerable;
    
    /** \brief Defines `type` as the tag category of \a Module under processing \a Message.
     *
     * `type` is one of:
     *  * \ref myynt::tags::first
     *  * \ref myynt::tags::intermediate
     *  * \ref myynt::tags::last
     */
    template< class Module, class Message >
    struct module_process_category;
    
    template< class Message, class Module, class >
    struct is_message_processable : std::false_type { };
    
    template< class Message, class Module >
    struct is_message_processable<
        Message, 
        Module, 
        std::void_t<decltype(std::declval<Module>().myynt_Process(std::declval<Message>()))>
    > : std::true_type { };
    
    template< class Manager, class Module, class >
    struct is_module_registerable : std::false_type { };
    
    template< class Manager, class Module >
    struct is_module_registerable<
        Manager,
        Module,
        std::void_t<decltype(std::declval<Module>().myynt_RegisterManager(std::declval<Manager>()))>
    > : std::true_type { };
    
    template< class Module, class Message >
    struct module_process_category {
        static constexpr bool is_in_first = tags::belongs_to_category<Module, Message, tags::first>::value;
        static constexpr bool is_in_last = tags::belongs_to_category<Module, Message, tags::last>::value;
        
        using type = typename std::conditional<
            is_in_first,
            tags::first,                // Module has a tag in Message::first
            typename std::conditional<
                is_in_last,
                tags::last,             // Module has a tag in Message::last
                yymp::group_suppress    // Module has neither a tag in first or last
            >::type
        >::type;
    };
    
    template< class Message, class ProcessCategoryGroup >
    struct make_tagged_subgroups {
        using message = Message;
        using category = typename yymp::get_group_key<ProcessCategoryGroup>::type;
        using modules = typename yymp::get_group_types<ProcessCategoryGroup>::type;
        using message_tags = typename tags::tags_of_category<message, category>::type;

        using type = typename yymp::group_by<
            yymp::bind<tags::get_first_representative<yymp::var, message_tags>>::template generic,
            modules
        >::type;
    };
    
    template< class Message, class ProcessCategoryGroup >
    struct reorder_process_group {
        using message = Message;
        using category = typename yymp::get_group_key<ProcessCategoryGroup>::type;
        using message_tags = typename tags::tags_of_category<message, category>::type;
        
        using tagged_subgroups = typename make_tagged_subgroups<Message, ProcessCategoryGroup>::type;
        using reordered_modules = typename yymp::expand<
            yymp::join,
            typename yymp::transform<
                yymp::bind<yymp::get_group<yymp::var, tagged_subgroups>>::template generic,
                message_tags
            >::type
        >::type;
        using type = yymp::group<category, reordered_modules>;
    };
    
}

#endif // MYYNT__TRAITS_HPP