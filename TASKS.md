# Summary
The goal of this project is to provide a means of intercommunication between modules of code through messages.

- [x] Modules
- [x] Emitters
- [ ] Tags

# Modules
The exact requirements for a module are very lax (even an instance of the empty class can be used as a module).
However, ideally, modules perform at least one of the following tasks:

- Process messages (via a method `myynt_Process`) as a service
- Emit messages to its parent module, which may then hand it up the hierarchy or back down to sibling modules under the same parent to be processed (via `myynt_Process`)

When a module passes the message up the hierarchy, it calls the `myynt_Emit` on the parent.
When a user or a module hand a message down to submodules, `myynt_Process` must be called.
Module that aggregate submodules must define `myynt_Process` to send the message down to its submodules.
There are some nuances with passing the message further up the module hierarchy, so the current list of tasks looks like this:

- [x] Create class `manager`, for which `myynt_Emit` calls `myynt_Process`, which distributes a message down to all its submodules as described above
- [x] Change `manager` to work for multiple submodules of the same type
- [x] Change `manager` to only call `myynt_Process` on messages for submodules that *handle* those messages (check if an overload exists)

# Emitters
Emitters are mixins that provide to user-created modules the `myynt_Emit` method.
There are two emitter types:
- 'emitter', which emits messages by a typed pointer to the manager above it (templated on the manager type)
- 'callback_emitter', which emits messages through callback functions (templated on the message types)

To avoid awful headaches, it is assumed that a module does not inherit multiple times from emitters.

- [x] Implement `emitter`
- [x] Implement `callback_emitter` (DONE, uses `emitter<callback, MessageTypes...>`)
- [x] Change `manager` to register directly with the emitter base

# Tags
When a parent module passes a message down to its submodules, it does so in the order the submodules are supplied. However, it would be useful to permute this order. 

A tag is a generic description for the service that a module performs.
A message type may optionally define two nested types `first` and `last` that provide a list of tags of modules that must process this message first and last.

- [x] Change the managers to emit down to modules that have a tag in the message's `first` nested type, if it exists, first
- [x] Change the managers to emit down to modules that have a tag in the message's `last` nested type, if it exists, last

# Extensions

- [ ] Move manager.hpp to premanager.hpp, rename class accordingly and remove `myynt_Emit` method
- [ ] Reintroduce `manager`, inheriting from `premanager`
- [ ] Create `complete`, which takes a template module and supplies it the manager type during manager instantiation
- [ ] Create `package`, which a manager expands its type parameters into its modules
- [ ] Create `isolated_package`, which acts similarly to `package`, except that messages passed through must be escaped in order to pass on to the manager