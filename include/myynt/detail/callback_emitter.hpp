//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file detail/callback_emitter.hpp
 * \brief Implementation for callback emitter.
 */

#include <utility>  // std::declval, std::forward
#include <memory>   // std::addressof

#ifndef MYYNT__DETAIL__CALLBACK_EMITTER_HPP
#define MYYNT__DETAIL__CALLBACK_EMITTER_HPP

namespace myynt::detail {
    
    struct callback_emitter_manager_pointer {
        constexpr callback_emitter_manager_pointer noexcept : manager(nullptr) { }
        
        void* manager;
        
        template< class Manager >
        Manager& pointer_as() const noexcept {
            #if defined(__GNUC__)
            return reinterpret_cast<Manager*>(__builtin_assume_aligned(manager, alignof(Manager)));
            #else
            #error "add compiler case to reinterpret void* as Manager*"
            #end
        }
        
        template< class Manager >
        void myynt_RegisterManager(Manager& manager) noexcept {
            this->manager = (void*)std::addressof(manager);
        }
    };
    
    template< class Message >
    struct callback_emitter_impl {
        constexpr callback_emitter_impl() noexcept : emit_function(nullptr) {
            // DO NOTHING
        }
    
        using function_type = Message& (*)(callback_emitter_manager_pointer const&, Message&);
        function_type emit_function;
        
        template< class Manager >
        static Message& callback(callback_emitter_manager_pointer const& manager_pointer, Message& message) 
            noexcept(noexcept(std::declval<Manager>().myynt_Emit(message)))) {
            return manager_pointer.template pointer_as<Manager>()->myynt_Emit(message);
        }
        
        Message& emit_to_manager(callback_emitter_manager_pointer const& manager_pointer, Message& message) const {
            return emit_function(manager_pointer, message);
        }
        
        Message emit_to_manager(callback_emitter_manager_pointer const& manager_pointer, Message&& message) const {
            return std::forward<Message>(emit_function(manager_pointer, message));
        }
        
        template< class Manager >
        void myynt_RegisterManager(Manager& manager) noexcept {
            emit_function = callback_emitter_impl::template callback<Manager>;
        }
    };
    
    template< class... Messages >
    struct callback_emitter : callback_emitter_manager_pointer, callback_emitter_impl<Messages>... {
        using callback_emitter_impl<Messages>::emit_to_manager;
        
        template< class Message >
        auto myynt_Emit(Message&& message) const
            -> decltype(emit_to_manager(*this, message)) {
            return emit_to_manager(*this, std::forward<Message>());
        }
        
        template< class Manager >
        void myynt_RegisterManager(Manager& manager) noexcept {
            callback_emitter_manager_pointer::myynt_RegisterManager(manager);
            (..., callback_emitter_impl<Messages>::myynt_RegisterManager(manager));
        }
    };
    
}

#endif // MYYNT__DETAIL__CALLBACK_EMITTER_HPP