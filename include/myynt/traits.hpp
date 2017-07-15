//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file traits.hpp
 * \brief Properties and relations between modules and messages.
 */
 
#ifndef YYMP__TRAITS_HPP
#define YYMP__TRAITS_HPP

#include <utility>		// std::declval
#include <type_traits> 	// std::void_t, std::false_type, std::true_type

namespace myynt {
	
    /** \brief Indicates that an emitter is a callback emitter. */
    struct callback;
    
	/** \brief Inherits from `bool` constants to indicate if a \a Module member function `myynt_Process` exists that takes values of type `Message&&`. */
	template< class Message, class Module, class = std::void_t<> >
	struct is_message_processable;
    
    template< class Message, class Module, class >
    struct is_message_processable : std::false_type { };
    
    template< class Message, class Module >
    struct is_message_processable<
        Message, 
        Module, 
        std::void_t<decltype(std::declval<Module>().myynt_Process(std::declval<Message>()))>
    > : std::true_type { };
}

#endif // YYMP__TRAITS_HPP