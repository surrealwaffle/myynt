//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MYYNT__MANAGER_HPP
#define MYYNT__MANAGER_HPP

#include <myynt/premanager.hpp>

namespace myynt {
    
    template< class... Modules >
    class manager : public premanager<manager<Modules...>, Modules...> {
    public:
        using premanager<manager<Modules...>, Modules...>::premanager;
        using premanager<manager<Modules...>, Modules...>::operator=;
        using premanager<manager<Modules...>, Modules...>::myynt_Process;
        
        /** \brief Sends \a message as an *lvalue reference* to the managed submodules.
         *
         * If this function is passed an lvalue, then this function returns an lvalue reference.
         * Otherwise, this function returns \a message by move.
         *
         * \return `std::forward<Message>(message)`
         */
        template< class Message >
        constexpr Message myynt_Emit(Message&& message) {
            return myynt_Process(std::forward<Message>(message));
        }
    };
    
    template< class... Modules >
    manager(Modules&&...) -> manager<typename std::decay<Modules>::type...>;
    
}

#endif // MYYNT__MANAGER_HPP