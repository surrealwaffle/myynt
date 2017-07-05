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
#include <type_traits> 	// std::false_type, std::true_type

#include <myynt/metaprogramming.hpp>

namespace myynt {
	
	/** \brief Sets \a value to \c true if and only if modules of type \a Module can process messages of type \a Message.
	 *
	 * This function looks for an overload that takes `Message&&`.
	 * Therefore overloads that take `Message&` will not be observed if `Message` is not a reference.
	 * One should be mindful of reference collapsing rules when using this function.
	 */
	template< class Message, class Module >
	struct is_message_processable {
	private:
		template< class Message_, class Module_ >
		static constexpr auto
		detect(metaprogramming::convey<Message_, Module_>) noexcept
			-> decltype(std::declval<Module_>().myynt_Process(std::declval<Message_>()), void(), std::true_type());
		
		template< class... FallbackOverload >
		static constexpr auto
		detect(FallbackOverload&&...) noexcept
			-> std::false_type;
	public:
		/** \brief \c true if and only if a \a Module can process values of type \a Message. */
		static constexpr bool value = decltype(detect(metaprogramming::convey<Message, Module>()))::value;
		
		constexpr operator bool() const noexcept { return value; }
		constexpr bool operator()() const noexcept { return value; }
	};
	
}

#endif // YYMP__TRAITS_HPP