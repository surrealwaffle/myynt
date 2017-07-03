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
#include <utility>      // std::forward
#include <type_traits>  // std::decay

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
        
        template< class... U >
        constexpr manager(U&&... modules) : modules_(std::forward<U>(ms)...) { }
        
        /** \brief Sends \a message down to the submodules under the manager, by lvalue reference.
         *
         * \return `std::forward<Message>(message)`
         */
        template< class Message >
        constexpr typename std::decay<Message>::type
        myynt_Process(Message&& message);
        
        /** \brief Sends \a message down to the submodules under the manager, by lvalue reference.
         *
         * \return `std::forward<Message>(message)`
         */
        template< class Message >
        constexpr typename std::decay<Message>::type
        myynt_Emit(Message&& message);
        
    private:
        std::tuple<Modules...> modules_;
    };
    
    #if __cpp_deduction_guides >= 201606
    template< class... Modules >
    manager(Modules&&...) -> manager<typename std::decay<Modules>::type...>;
    #endif
}

#endif // MYYNT__MANAGER_HPP