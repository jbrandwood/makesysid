# makesysid
A method of hashing strings into integer values for game engine usage.

This is one solution to the problem of "How to Identify Game Objects by Name".

REFERENCES ...

[Mick West's "Practical Hash IDs"](http://cowboyprogramming.com/2007/01/04/practical-hash-ids/)

[Maciej Sinilo's "Hashing made (even more) useful"](http://msinilo.pl/blog/?p=82)


## The Design

The design was inspired by working with Neversoft's game engine on a project,
and being awed by the power of the hash-ID concept, but finding a few
weaknesses in the specific implementation (as it existed in that version of
Neversoft's engine).

Two crucial improvements over using a pure CRC-32 to generate the ID are ...

1. Identifiers should be case-insensitive because they are often created or
   edited by non-programmers (i.e. designers or artists), and it is far too
   easy to end up with unintended duplicates if the identifiers are
   case-sensitive.

2. Identifiers should have some reserved values that can be given special
   meanings by the game engine.

In this design, the sysID values 0..255 are reserved for special game objects
(such as the player), and sysID values 256..65535 are reserved for *temporary*
or *unnamed* object usage.

This allows in-game object instances to have unique sysIDs, where the sysID
values can be efficiently allocated and freed using a simple 1-bit flag map.


## The Code

The functions are written in C, and the usage of the sysIDs within a game's
source code is facilitated by the provision of a couple of macros.

The `ConstID()` macro can be used anywhere, but it is best used only in
*switch()* statements.

The `MakeID()` macro is provided for normal use, and while debugging it
will check and confirm that the hash value matches the string.

That is useful for confirming that the string hasn't been accidentally edited,
and also provides a hook that can be used to create an in-game reverse lookup
table to map sysID values back to their string form for in-game diagnostics
and error messages.

In the circumstances that the project's development hardware has more memory
than the final target hardware, keeping such an in-game database can be easier
than modifying the manufacturer's debugger to perform the reverse lookup.


## The CLI Utility

A simple command-line program is also provided to generate a sysID value from
a string.

The value (actually a macro) is copied to the Windows clipboard so that it can
be pasted into the programmer's source code.

A decently-powerful text editor can automate the whole process, making it even
easier to use.
