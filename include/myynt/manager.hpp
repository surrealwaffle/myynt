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
#include <type_traits>  // std::enable_if, 

namespace myynt {
    
    /** \brief A module manager that emits down to submodules. */
    template< class... Modules >
    class manager;

	template< class... Modules >
    class manager {
    public:	
        manager()               = default;
        manager(manager const&) = default;
        manager(manager&&)      = default;
        
        manager& operator=(manager const&)  = default;
        manager& operator=(manager&&)       = default;
        
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
		> explicit constexpr manager(UTypes&&... modules) : modules_(std::forward<UTypes>(utypes)...) { }
        
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
        { return this->myynt_ProcessModulesByIndex(std::forward<Message>(message), std::make_index_sequence<sizeof...(Modules)>()); }
        
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
        { return this->myynt_Process(std::forward<Message>(message)); }
        
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
        myynt_ProcessModulesByIndex(Message&& message, std::index_sequence<I...>);
        
        /** \brief The modules themselves. */
        std::tuple<Modules...> modules_;
    };
    
    #if __cpp_deduction_guides >= 201606
    template< class... Modules >
    explicit manager(Modules...) -> manager<Modules...>;
    #endif
    
	template< class... Modules >
    template< class Message, std::size_t... I >
    constexpr Message
    manager<Modules...>::myynt_ProcessModulesByIndex(Message&& message, std::index_sequence<I...>) {
        (..., std::get<I>(modules_).myynt_Process(message));
        return std::forward<Message>(message);
    }
}

#endif // MYYNT__MANAGER_HPP