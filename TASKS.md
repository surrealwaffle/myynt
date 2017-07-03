# Summary
The goal of this project is to provide a means of intercommunication between modules of code through messages.

- [ ] Modules
- [ ] Emitters
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

- [ ] Create class `manager`, for which `myynt_Emit` calls `myynt_Process`, which distributes a message down to all its submodules as described above
- [ ] Change `manager` to work for multiple submodules of the same type
- [ ] Change `manager` to only call `myynt_Process` on messages for submodules that *handle* those messages (check if an overload exists)

# Emitters
Emitters are mix-in classes that provide to user-created modules the `myynt_Emit` method.
There are two types of emitters, both class templates:
- `callback_emitter`s store a pointer to the parent manager as well as function pointers to their `myynt_Emit` methods for specific message types
- `emitter`, whose template parameter is the manager type and stores a pointer to the manager

- [ ] Implement `emitter`
- [ ] Implement `callback_emitter`
- [ ] Change `manager` to check for emitter bases and directly register with the emitter (avoid requiring the user to reintroduce methods with a `using` declaration)
- [ ] \(Awful Headache) Design the emitters to register with the manager, even if a submodule inherits from an emitter multiple times

# Tags
When a parent module passes a message down to its submodules, it does so in the order the submodules are supplied. However, it would be useful to permute this order. 

A tag is a generic description for the service that a module performs.
A message type may optionally define two nested types `first` and `last` that provide a list of tags of modules that must process this message first and last.

- [ ] Change the managers to emit down to modules that have a tag in the message's `first` nested type, if it exists, first
- [ ] Change the managers to emit down to modules that have a tag in the message's `last` nested type, if it exists, last

# Extensions
More features may be added here as I come across them.