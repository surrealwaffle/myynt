//          Copyright Thomas Goehner 2018
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MYYNT__FUNCTIONS_HPP
#define MYYNT__FUNCTIONS_HPP

#include <type_traits> // std::is_invocable_v, std::is_nothrow_invocable_v
#include <utility>     // std::index_sequence
#include <tuple>       // std::get(std::tuple)

namespace myynt {

/** Invokes \a callable over \a args by lvalue-reference, if possible, otherwise performs no operation. */
template<class Callable, class... Args>
constexpr void emit(Callable&& callable, Args&&... args)
    noexcept(std::is_nothrow_invocable_v<Callable&, Args&...>) {
    if constexpr (std::is_invocable_v<Callable&, Args&...>)
        (void)callable(args...);
}

/** Calls \ref myynt::emit on each callable indexed by \a I with lvalue-references of \a args. */
template<class Container, std::size_t... I, class... Args>
constexpr void emit_to_container(Container&& container, std::index_sequence<I...>, Args&&... args)
    noexcept((noexcept((void)emit(std::get<I>(container), args...)) && ...)) {
    (..., (void)::myynt::emit(std::get<I>(container), args...));
}

}

#endif // MYYNT__FUNCTIONS_HPP
