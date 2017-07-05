//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file metaprogramming.hpp
 * \brief Metaprogramming trickery.
 */

#ifndef YYMP__METAPROGRAMMING__METAPROGRAMMING_HPP
#define YYMP__METAPROGRAMMING__METAPROGRAMMING_HPP

namespace myynt::metaprogramming {
	
	/** \brief A type used to force certain overloads to be inspected first.
	 *
	 * See \ref traits.hpp for some examples.
	 */
	template< class... >
	struct convey { };
	
}

#endif // YYMP__METAPROGRAMMING__METAPROGRAMMING_HPP