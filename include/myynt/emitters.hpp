//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file emitters.hpp
 * \brief Mixins providing carry-up message passing.
 */

#ifndef MYYNT__EMITTERS_HPP
#define MYYNT__EMITTERS_HPP

#include <type_traits>  // std::enable_if, std::remove_reference
#include <memory>       // std::addressof

#include <yymp/typelist_fwd.hpp>    // yymp::typelist
#include <yymp/filter.hpp>          // yymp::filter_duplicates
#include <yymp/expand.hpp>          // yymp::expand
#include <yymp/traits.hpp>          // yymp::any_of

#include <myynt/traits.hpp> // myynt::callback

#include <myynt/detail/callback_emitter.hpp> // myynt::callback_emitter

namespace myynt {
    
    /** \brief See the specializations of this class.
     *
     * Both specializations provide public member function (possibly template) `myynt_Emit`.
     * Calls made to the function (or instances of the template) will pass a message up to the emitter's registered manager.
     *
     * Emitters are default constructed in an unregistered state and registered with the manager after the construction of its submodules.
     * Because `myynt_Emit` expects that the manager is registered, emitting a message during construction of a module using an emitter
     * is not recommended.
     *
     * These calls behave as if calling `myynt_Emit` on the \ref myynt::manager "manager", possibly with some overhead.
     */
    template< class... Ts >
    class emitter;
    
    /** \brief Registers \a manager with the emitter that \a module inherits from.
     *
     * If \a module does not use an emitter, no operation is performed.
     */
    template< class Manager, class Module >
    constexpr void myynt_RegisterManagerWithEmitter(Manager& manager, Module& module) noexcept;
    
    /** \brief An emitter that stores a typed pointer to the parent manager in order to emit. */
    template< class Manager >
    class emitter<Manager> {
    public:
        constexpr emitter() noexcept : manager(nullptr) { }
        
        emitter(emitter const&) = default;
        emitter(emitter&&) = default;
        emitter& operator=(emitter const&) = default;
        emitter& operator=(emitter&&) = default;
        
        /** \brief Sends \a message up to the registered parent manager, by lvalue reference, as if by calling `myynt_Emit` on the manager.
         *
         * If this function is passed an lvalue, then this function returns an lvalue reference.
         * Otherwise, this function attempts to return \a message by move.
         *
         * If a manager has not registered with this emitter, then the behaviour of this function is undefined.
         *
         * \param[in] message The message to emit.
         * \return `std::forward<Message>(message)`
         */
        template< class Message >
        constexpr Message myynt_Emit(Message&& message) {
            return manager->myynt_Emit(std::forward<Message>(message));
        }
        
    private:
        Manager *manager; /**< The registered manager. */
        
        friend constexpr void myynt_RegisterManagerWithEmitter(Manager& manager, emitter& e) noexcept {
            e->manager = std::addressof(manager);
        }
    };
    
    /** \brief An emitter that stores and uses callbacks in order to emit messages up to the registered manager.
     *
     * \a Messages must not include any reference types.
     * Duplicates in \a Messages is permitted and will not cause multiple emits to occur per call to `myynt_Emit`.  
     *
     * The value type of the message emitted must match to at least one among \a Messages, including cv-qualifiers.
     * This is a design decision in order to maintain consistent functionality with the other emitter specializations.
     */
    template< class... Messages >
    class emitter<callback, Messages...> {
        static_assert((!std::is_reference<Messages>::value && ...), "callback emitter must not be templated on reference types");
        
        
        using messages = typename yymp::filter_duplicates< yymp::typelist<Messages...> >::type;
        using callback_impl = typename yymp::expand< detail::callback_emitter, messages >::type;
        
    public:
        emitter() = default;
        emitter(emitter const&) = default;
        emitter(emitter&&) = default;
        emitter& operator=(emitter const&) = default;
        emitter& operator=(emitter&&) = default;
        
        /** \brief Sends \a message up to the registered parent manager, by lvalue reference, as if by calling `myynt_Emit` on the manager.
         *
         * If \a Message is a reference, the type referred to by \a Message must be in \a Messages.
         * Otherwise, \a Message must be in \a Messages.
         *
         * If this function is passed an lvalue, then this function returns an lvalue reference.
         * Otherwise, this function attempts to return \a message by move.
         *
         * If a manager has not registered with this emitter, then the behaviour of this function is undefined.
         *
         * \param[in] message The message to emit up to the manager.
         * \return `std::forward<Message>(message)`
         */
        template< class Message >
        auto myynt_Emit(Message&& message)
            -> typename std::enable_if<
                ::yymp::any_of< typename std::remove_reference<Message>::type, messages >::value,
                Message
            >::type {
            return callbacks.myynt_Emit(std::forward<Message>(message));
        }
        
    private:
        callback_impl callbacks; /**< Storage for the callbacks. */
        
        template< class Manager >
        friend void myynt_RegisterManagerWithEmitter(Manager& manager, emitter& e) noexcept {
            e.callbacks.myynt_RegisterManager(manager);
        }
    };
    
    // default fallback if Module has no emitters
    template< class Manager, class Module >
    constexpr void myynt_RegisterManagerWithEmitter(Manager&, Module&) noexcept {
        // DO NOTHING
    }
}

#endif // MYYNT__EMITTERS_HPP