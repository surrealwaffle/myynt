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

- [ ] Create `manager`, for which `myynt_Emit` calls `myynt_Process`, which distributes a message down to all its submodules as described above
- [ ] Change `manager` to work for multiple submodules of the same type
- [ ] Change `manager` to only call `myynt_Process` on messages for submodules that *handle* those messages (check if an overload exists)
- [ ] Change `manager` to check for and run a runtime filter (`myynt_Filter`) on messages for submodules that handle those messages

# Emitters

# Tags
