//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file emitters.hpp
 * \brief Mixins providing carry-up message passing.
 */

#ifndef MYYNT__EMITTERS_HPP
#define MYYNT__EMITTERS_HPP

#include <type_traits>  // std::enable_if
#include <memory>       // std::addressof

#include <yymp/typelist_fwd.hpp>    // yymp::typelist
#include <yymp/filter.hpp>          // yymp::filter_duplicates
#include <yymp/expand.hpp>          // yymp::expand
#include <yymp/traits.hpp>          // yymp::any_of

#include <myynt/traits.hpp> // myynt::callback

#include <myynt/detail/callback_emitter.hpp> // myynt::callback_emitter

namespace myynt {
    
    template< class Module, class Manager >
    constexpr void myynt_RegisterManagerWithEmitter(Module&, Manager&) noexcept {
        // DO NOTHING
    }
    
    template< class... Ts >
    class emitter;
    
    template< class Manager >
    class emitter<Manager> {
    public:
        constexpr emitter() noexcept : manager(nullptr) { }
        
        emitter(emitter const&) = default;
        emitter(emitter&&) = default;
        emitter& operator=(emitter const&) = default;
        emitter& operator=(emitter&&) = default;
        
        template< class Message >
        constexpr Message myynt_Emit(Message&& message) {
            return manager->myynt_Emit(std::forward<Message>(message));
        }
        
    private:
        Manager *manager;
        
        friend constexpr void myynt_RegisterManagerWithEmitter(emitter& e, Manager& manager) noexcept {
            e->manager = std::addressof(manager);
        }
    };
        
    template< class... Messages >
    class emitter<callback, Messages...> {
        using messages = typename ::yymp::filter_duplicates< ::yymp::typelist<Messages...> >::type;
        using callback_impl = typename ::yymp::expand< detail::callback_emitter, messages >::type;
    public:
        emitter() = default;
        emitter(emitter const&) = default;
        emitter(emitter&&) = default;
        emitter& operator=(emitter const&) = default;
        emitter& operator=(emitter&&) = default;
        
        template< class Message >
        auto myynt_Emit(Message&& message)
            -> typename std::enable_if<::yymp::any_of<Message, messages>::value, Message>::type {
            return callbacks.myynt_Emit(std::forward<Message>(message));
        }
        
    private:
        callback_impl callbacks;
        
        template< class Manager >
        friend void myynt_RegisterManagerWithEmitter(emitter& e, Manager& manager) noexcept {
            e.callbacks.myynt_RegisterManager(manager);
        }
    };
}

#endif // MYYNT__EMITTERS_HPP