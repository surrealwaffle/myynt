//          Copyright Thomas Goehner 2017
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/** \file premanager.hpp
 * \brief Implements the module premanager (\ref myynt::premanager "premanager"), used as a base for various kinds of managers.
 */

#ifndef MYYNT__PREMANAGER_HPP
#define MYYNT__PREMANAGER_HPP

#include <tuple>        // std::tuple
#include <utility>      // std::forward, std::index_sequence, std::make_index_sequence
#include <type_traits>  // std::enable_if, std::is_constructible, std::is_

#include <yymp/typelist_fwd.hpp>    // yymp::typelist, yymp::size
#include <yymp/filter.hpp>          // yymp::filter_duplicates, yymp::indices_where, yymp::indices_within
#include <yymp/bind.hpp>            // yymp::bind
#include <yymp/expand.hpp>          // yymp::expand_into
#include <yymp/group.hpp>           // yymp::group_by
#include <yymp/transform.hpp>       // yymp::transform
#include <yymp/setops.hpp>          // yymp::difference
#include <yymp/conversions.hpp>

#include <myynt/traits.hpp>     // myynt::is_message_processable
#include <myynt/emitters.hpp>   // myynt::myynt_RegisterManagerWithEmitter
#include <myynt/complete.hpp>   // myynt::make_complete

namespace myynt {
    
    namespace impl {
        template< class Manager, class... Modules >
        struct premanager;
    }
    
    /** \brief Provides manager implementations the `myynt_Process` method, which emits a message down into its submodules. 
     *
     * This design makes use of CRTP in \a Manager.
     * The implementation of the premanager functionality is left to \ref myynt::impl::premanager.
     */
    template< class Manager, class... Modules >
    using premanager = impl::premanager<Manager, typename make_complete<Manager, Modules>::type...>;

    namespace impl {
        template< class... Modules >
        struct premanager_modules_container {
            premanager_modules_container() = default;
            premanager_modules_container(premanager_modules_container const&) = default;
            premanager_modules_container(premanager_modules_container&&) = default;
            
            premanager_modules_container& operator=(premanager_modules_container const&) = default;
            premanager_modules_container& operator=(premanager_modules_container&&) = default;
            
            template<
                class... UTypes, 
                class = typename std::enable_if<
                    sizeof...(UTypes) >= 1 &&
                    sizeof...(UTypes) == sizeof...(Modules) &&
                    (std::is_constructible<Modules, UTypes&&>::value && ...)    
                >::type
            > constexpr premanager_modules_container(UTypes&&... args) : modules(std::forward<UTypes>(args)...) { }
            
            std::tuple<Modules...> modules;
        };
        
        template< class Premanager >
        struct premanager_registrar {
            constexpr premanager_registrar() noexcept {
                static_cast<Premanager*>(this)->myynt_RegisterAsManager();
            }
            
            constexpr premanager_registrar(premanager_registrar const&) noexcept {
                static_cast<Premanager*>(this)->myynt_RegisterAsManager();
            }
            
            constexpr premanager_registrar(premanager_registrar&&) noexcept {
                static_cast<Premanager*>(this)->myynt_RegisterAsManager();
            }
            
            constexpr premanager_registrar& operator=(premanager_registrar const&) noexcept {
                static_cast<Premanager*>(this)->myynt_RegisterAsManager();
                return *this;
            }
            
            constexpr premanager_registrar& operator=(premanager_registrar&&) noexcept {
                static_cast<Premanager*>(this)->myynt_RegisterAsManager();
                return *this;
            }
        };
        
        /** \brief Implements premanager functionality.
         *
         * No reinterpretation of \a Modules is performed.
         */
        template< class Manager, class... Modules >
        class premanager : premanager_modules_container<Modules...>, premanager_registrar<premanager<Manager, Modules...>> {
            friend class premanager_registrar<premanager>;
            
            using module_types = typename yymp::typelist<Modules...>; ///< A `yymp::typelist` containing the pack \a Modules.
            
            using distinct_module_types = typename yymp::filter_duplicates<module_types>::type; ///< A `yymp::typelist` of the distinct types in \a Modules.
            
            using modules_container_type = typename yymp::expand_into<std::tuple, module_types>::type; ///< The container that holds the modules.
        public:
            premanager()                    = default; ///< Available if each module is default constructible.
            premanager(premanager const&)   = default; ///< Available if each module is copy constructible.
            premanager(premanager&&)        = default; ///< Available if each module is move constructible.
            
            premanager& operator=(premanager const&)    = default; ///< Available if each module is copy assignable.
            premanager& operator=(premanager&&)         = default; ///< Available only if each module is move assignable.
            
            /** \brief Constructs the premanager's modules_ from \a modules. 
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
            > explicit constexpr premanager(UTypes&&... modules) 
                : premanager_modules_container<Modules...>(std::forward<UTypes>(modules)...) {
                // myynt_RegisterAsManager(); // premanager_registrar registers automatically
            }
            
            /** \brief Sends \a message down to the submodules under the premanager, by lvalue reference.
             *
             * If this function is passed an lvalue, then this function returns an lvalue reference.
             * Otherwise, this function this function attempts to return \a message by move.
             *
             * \return `std::forward<Message>(message)`
             */
            template< class Message >
            constexpr Message
            myynt_Process(Message&& message);
            
        private:
            /** \brief Container for the modules. */
            using premanager_modules_container<Modules...>::modules;
            //modules_container_type modules_;
            
            /** \brief Sends \a message down to the submodules selected by the supplied index sequence under the premanager, by lvalue reference.
             *
             * If this function is passed an lvalue, then this function returns an lvalue reference.
             * Otherwise, this function returns \a message by move.
             *
             * \return `std::forward<Message>(message)`
             */
            template< class Message, std::size_t... I >
            constexpr Message
            myynt_SendToModules(Message&& message, std::index_sequence<I...>);
            
            /** \brief Registers the inheriting manager with the submodule emitters. */
            template< std::size_t... I >
            constexpr void
            myynt_RegisterAsManager(std::index_sequence<I...>) noexcept;
            
            /** \brief Registers the inheriting manager with the submodule emitters. */
            constexpr void
            myynt_RegisterAsManager() noexcept {
                myynt_RegisterAsManager(std::make_index_sequence<sizeof...(Modules)>());
            }
            
        };
        
        template< class Manager, class... Modules >
        template< class Message >
        constexpr Message
        premanager<Manager, Modules...>::myynt_Process(Message&& message) {
            using message_value_type = typename std::remove_cv<typename std::remove_reference<Message>::type>::type;
            
            using modules_processing = typename yymp::filter<
                yymp::bind<is_message_processable<message_value_type&, yymp::var>>::template generic,
                distinct_module_types
            >::type;
            
            // unordered
            using modules_by_category = typename yymp::group_by<
                yymp::bind<module_process_category<yymp::var, message_value_type>>::template generic,
                modules_processing
            >::type;
            
            // ordered
            using ordered_modules_by_category = typename yymp::transform<
                yymp::bind<reorder_process_group<message_value_type, yymp::var>>::template generic,
                modules_by_category
            >::type; // have fun looking at the implementation of this x.x
            /* as best i can describe it:
             * for each group, group each type under the representative tag (tags::tags_of_category filters duplicates for you)
             * then transform each tag into the group types in this grouping of subgroups (i.e. use the group as a map)
             * then expand the resulting typelist through join, as it is a typelist of typelists
             * then wrap it back as a typelist under the category
             */
            
            using first_modules         = typename yymp::get_group<tags::first, ordered_modules_by_category>::type;
            using last_modules          = typename yymp::get_group<tags::last, ordered_modules_by_category>::type;
            using intermediate_modules  = typename yymp::difference<
                modules_processing,
                typename yymp::join<first_modules, last_modules>::type
            >::type;
            
            using first_indices         = typename yymp::indices_within<module_types, first_modules>::type;
            using last_indices          = typename yymp::indices_within<module_types, last_modules>::type;
            using intermediate_indices  = typename yymp::indices_within<module_types, intermediate_modules>::type;
            
            using indices = typename yymp::typelist_to_integer_sequence<
                typename yymp::expand<
                    yymp::join,
                    yymp::typelist<
                        typename yymp::integer_sequence_to_typelist<first_indices>::type,
                        typename yymp::integer_sequence_to_typelist<intermediate_indices>::type,
                        typename yymp::integer_sequence_to_typelist<last_indices>::type
                    >
                >::type,
                std::size_t
            >::type;
            
            return myynt_SendToModules(std::forward<Message>(message), indices{});
        }
            
        template< class Manager, class... Modules >
        template< class Message, std::size_t... I >
        constexpr Message
        premanager<Manager, Modules...>::myynt_SendToModules(Message&& message, std::index_sequence<I...>) {
            (..., std::get<I>(modules).myynt_Process(message));
            return std::forward<Message>(message);
        }
        
        template< class Manager, class... Modules >
        template< std::size_t... I >
        constexpr void
        premanager<Manager, Modules...>::myynt_RegisterAsManager(std::index_sequence<I...>) noexcept {
            Manager& manager = *static_cast<Manager*>(this);
            (..., (myynt_RegisterManagerWithEmitter(manager, std::get<I>(modules)), void()));
        }
    }
}

#endif // MYYNT__PREMANAGER_HPP