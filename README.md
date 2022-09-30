# Kernel-Thread-Subsystem

Implementation of a kernel subsystem for thread management
intended for old x86 CPUs. The subsystem provides the concept
of threads using time sharing, as well as support for creating, 
starting and forking threads, concept of semaphores, and interrupt driven events.

The kernel subsystem is realized in such a way that a user program
and the subsystem itself share the same address space. They are compiled
and linked into the same program. It was written in Eclipse IDE using
a specific Borland C/C++ compiler.

Example of a user program can be found in the test folder, in which
all the conventions needed to be followed in order for the
user program to be executed normally can be seen.