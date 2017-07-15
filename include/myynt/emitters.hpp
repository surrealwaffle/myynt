//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file emitters.hpp
 * \brief Mixins providing carry-up message passing.
 */

#ifndef MYYNT__EMITTERS_HPP
#define MYYNT__EMITTERS_HPP

#include <yymp/typelist_fwd.hpp>
#include <yymp/filter.hpp>

#include <myynt/traits.hpp>

namespace myynt {
    
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
    };
        
    template< class... Messages >
    class emitter<callback, Messages...> {
        
    };
}

#endif // MYYNT__EMITTERS_HPP