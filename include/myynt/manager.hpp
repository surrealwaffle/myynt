//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file manager.hpp
 * \brief Implements a basic module manager (\ref myynt::manager "manager").
 */

#ifndef MYYNT__MANAGER_HPP
#define MYYNT__MANAGER_HPP

#include <tuple>        // std::tuple
#include <utility>      // std::forward, std::index_sequence, std::make_index_sequence
#include <type_traits>  // std::enable_if, std::is_constructible, std::is_

#include <yymp/typelist_fwd.hpp>    // yymp::typelist, yymp::size
#include <yymp/filter.hpp>          // yymp::indices_where
#include <yymp/bind.hpp>            // yymp::bind

#include <myynt/traits.hpp>     // myynt::is_message_processable
#include <myynt/emitters.hpp>   // myynt::myynt_RegisterManagerWithEmitter

namespace myynt {
    
    /** \brief A module manager that emits down to submodules. */
    template< class... Modules >
    class manager;

    template< class... Modules >
    class manager {
        using module_types = ::yymp::typelist<Modules...>;
        
        template< class Message >
        using get_processable_indices = typename ::yymp::indices_where<
            ::yymp::bind<is_message_processable<Message, ::yymp::var>>::template generic,
            module_types
        >::type;
        
        /** \brief The modules themselves. */
        std::tuple<Modules...> modules_;
    public:
        manager()               = delete;
        manager(manager const&) = delete; ///< \todo make optionally available
        manager(manager&&)      = delete; ///< \todo make optionally available
        
        manager& operator=(manager const&)  = delete; ///< \todo make optionally available
        manager& operator=(manager&&)       = delete; ///< \todo make optionally available
        
        /** \brief Conditionally defined default constructor.
         *
         * This constructor is enabled if and only if all \a Modules are default constructible.
         */
        template<
            class = typename std::enable_if< std::is_default_constructible<decltype(modules_)>::value >::type
        > explicit constexpr manager() noexcept(std::is_nothrow_default_constructible<decltype(modules_)>::value)
            : modules_() {
            myynt_RegisterAsManager();
        }
        
        /** \brief Constructs the manager's modules from \a modules. 
         *
         * This constructor is enabled if and only if all of the following are true:
         *  * `sizeof...(UTypes) >= 1`
         *  * `sizeof...(UTypes) == sizeof...(Modules)`
         *  * `std::is_constructible<Modules, U&&>` is \c true for each type `U` in the pack `UTypes`
         */
        template<
            class... UTypes, 
            class = typename std::enable_if<
                sizeof...(UTypes) >= 1 &&
                sizeof...(UTypes) == sizeof...(Modules) &&
                (std::is_constructible<Modules, UTypes&&>::value && ...)    
            >::type
        > explicit constexpr manager(UTypes&&... modules) : modules_(std::forward<UTypes>(modules)...) {
            myynt_RegisterAsManager();
        }
        
        /** \brief Sends \a message down to the submodules under the manager, by lvalue reference.
         *
         * If this function is passed an lvalue, then this function returns an lvalue reference.
         * Otherwise, this function this function attempts to return \a message by move.
         *
         * \return `std::forward<Message>(message)`
         */
        template< class Message >
        constexpr Message
        myynt_Process(Message&& message)
        { return myynt_SendToModules(std::forward<Message>(message), get_processable_indices<Message&>()); }
        
        /** \brief Sends \a message down to the submodules under the manager, by lvalue reference.
         *
         * If this function is passed an lvalue, then this function returns an lvalue reference.
         * Otherwise, this function this function attempts to return \a message by move.
         *
         * \return `std::forward<Message>(message)`
         */
        template< class Message >
        constexpr Message
        myynt_Emit(Message&& message)
        { return myynt_Process(std::forward<Message>(message)); }
        
    private:
        /** \brief Sends \a message down to the submodules selected by the supplied index sequence under the manager, by lvalue reference.
         *
         * If this function is passed an lvalue, then this function returns an lvalue reference.
         * Otherwise, this function this function attempts to return \a message by move.
         *
         * \return `std::forward<Message>(message)`
         */
        template< class Message, std::size_t... I >
        constexpr Message
        myynt_SendToModules(Message&& message, std::index_sequence<I...>);
        
        /** \brief Registers this manager with its submodules' emitters. */
        template< std::size_t... I >
        constexpr void
        myynt_RegisterAsManager(std::index_sequence<I...>) noexcept;
        
        /** \brief Registers this manager with its submodules' emitters. */
        constexpr void
        myynt_RegisterAsManager() noexcept {
            myynt_RegisterAsManager(std::make_index_sequence<yymp::size<module_types>::value>());
        }
        
    };
    
    #if __cpp_deduction_guides >= 201606
    template< class... Modules >
    explicit manager(Modules...) -> manager<Modules...>;
    #endif
        
    template< class... Modules >
    template< class Message, std::size_t... I >
    constexpr Message
    manager<Modules...>::myynt_SendToModules(Message&& message, std::index_sequence<I...>) {
        (..., std::get<I>(modules_).myynt_Process(message));
        return std::forward<Message>(message);
    }
    
    template< class... Modules >
    template< std::size_t... I >
    constexpr void
    manager<Modules...>::myynt_RegisterAsManager(std::index_sequence<I...>) noexcept {
        (..., (myynt_RegisterManagerWithEmitter(*this, std::get<I>(modules_)), void()));
    }
}

#endif // MYYNT__MANAGER_HPP