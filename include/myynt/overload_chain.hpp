//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MYYNT__SYSTEM_MANAGER_HPP
#define MYYNT__SYSTEM_MANAGER_HPP

#include <type_traits> // std::is_invocable_v, std::is_nothrow_constructible_v
#include <tuple>       // std::tuple

#include <myynt/functions.hpp>

namespace myynt {

/** A `FunctionObject` that invokes composing `Callable` objects on supplied arguments.
 * Only the `Callable` objects that can be invoked in this manner are invoked.
 * The order of these invocations occurs in the order supplied by \a Callables.
 */
template<class... Callables>
struct overload_chain {
    /** The type of the container storing the callable objects. */
    using callables_container = std::tuple<Callables...>;

    /** The sequence of indices into the #callables_container. */
    using index_sequence      = std::make_index_sequence<sizeof...(Callables)>;

    /** A constant that is `true` if and only if any object in #callables can be invoked over \a Args. */
    template<class... Args>
    static constexpr bool has_overload = (std::is_invocable_v<Callables&, Args...> || ...);

    /** A constant that is `true` if and only if any object in #callables can be invoked over \a Args with const-qualification. */
    template<class... Args>
    static constexpr bool has_const_overload = (std::is_invocable_v<Callables const&, Args...> || ...);

    overload_chain()                      = default; ///< DEFAULT
    overload_chain(overload_chain const&) = default; ///< DEFAULT
    overload_chain(overload_chain&&)      = default; ///< DEFAULT

    overload_chain& operator=(overload_chain const&) = default; ///< DEFAULT
    overload_chain& operator=(overload_chain&&)      = default; ///< DEFAULT

    /** Constructs the #callables directly. */
    constexpr overload_chain(Callables... callables)
        noexcept(std::is_nothrow_constructible_v<callables_container, Callables...>)
        : callables(std::move(callables)...) { }

    /** Invokes \a args by lvalue reference on all objects in #callables over which \a args can be invoked.
     * If no such callable exists among #callables, then this function performs no operation.
     */
    template<class... Args>
    constexpr void operator()(Args&&... args)
        noexcept(noexcept((void)::myynt::emit_to_container(std::declval<callables_container&>(), index_sequence{}, args...))) {
        if constexpr (has_overload<Args&...>)
            (void)::myynt::emit_to_container(callables, index_sequence{}, args...);
    }

    /** Invokes \a args by lvalue reference on all objects in #callables over which \a args can be invoked with const-qualification.
     * If no such callable exists among #callables, then this function performs no operation.
     */
    template<class... Args>
    constexpr void operator()(Args&&... args) const
        noexcept(noexcept((void)::myynt::emit_to_container(std::declval<callables_container const&>(), index_sequence{}, args...))) {
        if constexpr (has_const_overload<Args&...>)
            (void)::myynt::emit_to_container(callables, index_sequence{}, args...);
    }

    /** The container for the callable objects. */
    callables_container callables;
};

}

#endif // MYYNT__SYSTEM_MANAGER_HPP
