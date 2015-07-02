AshleyCPP - C++11 ECS Framework
===============================

AshleyCPP is a lightweight, cross-platform and easy to use Entity framework for C++11 based on the Java entity framework
[Ashley](https://github.com/libgdx/ashley/), targeted towards game development. It contains all the classes you need to 
get started making a game using a pure, ECS-oriented style.

It prioritises speed and simplicity and mirrors the public API of the original where possible to make it easy to learn,
quick to run and simple to debug, doubly so for users of the Java original.

The original is inspired by the [Ash](http://www.ashframework.org/) and
[Artemis](http://gamadu.com/artemis/) frameworks.

AshleyCPP is a personal project and while there are no guarantees that it will compile and work, the master branch
on GitHub should compile and function properly and should be usable for making projects right now.

### Building

AshleyCPP uses the CMake build system and comes with three build targets; the debug library, "AshleyCPP-d", the release library "AshleyCPP" and the imaginatively named executable "AshleyCPPTest". The test program has a hard dependency on Google Test.

To build the library from source, install a relatively recent version of CMake, navigate to a copy of the source and
do the following:

    mkdir build
    cd build
    cmake ..

and proceed as normal for your environment. If you don't care about the tests, you can run `cmake -DEXCLUDE_TESTS=TRUE ..` and the dependency on GTest will be removed, just giving you the libraries.

### Usage Notes and API Changes
While AshleyCPP strives to match the exported public API of the Java original, differences in the languages mean that
some differences exist. Such changes are listed in detail in APICHANGES.md, but a quickstart is given below.

Note: In situations where you'd use `.class` to get the type of a Java class, in C++ you can `#include <typeinfo>` and:

    typeid(ComponentClass); // Equivalent to ComponentClass.class in Java.  
    
    // e.g. get a Family* that matches Entity having both ComponentA and ComponentB
    ashley::Family::getFor({typeid(ComponentA), typeid(ComponentB)});
     
- General
  - The whole library is enclosed in the `ashley` namespace. Use `using namespace ashley;` to save typing if you want.
  - You can `#include` individual headers (organised similarly to the Java packages) or `#include "Ashley/AshleyCore.hpp"`
  - Java generic functions that take vararg lists of `Class<? extends Component>` are replaced by
    `std::initializer_list<std::type_index>`. Some functions also provide template overloads which may be easier and
    faster to use; check the documentation for specific occurrences.
  - The library enforces ownership using std::unique_ptr; the engine owns all entities and systems that are passed into
    it and unless they're removed, all entities and systems die with the engine. Likewise, each Entity strictly owns
    its attached components. This minimises the risk of leaking memory. 
  
For more specific changes, see the [APICHANGES.md](https://github.com/SgtCoDFish/AshleyCPP/blob/master/APICHANGES.md) file.

### Implementation Status
Ticked classes have both their implementation and tests complete.
- Core
  - [x] Component
  - [x] ComponentMapper
  - [x] ComponentType
  - [x] Engine
  - [x] Entity
  - [x] EntityListener
  - [x] EntitySystem
  - [x] Family
- Signals
  - [x] Signal
  - [x] Listener
- Systems
  - [x] IteratingSystem
  - [x] IntervalSystem
  - [x] IntervalIteratingSystem
  - [ ] SortedIteratingSystem
- Util
  - [x] ObjectPool
  - [x] Poolable
  
*NB:*
- Bag and ImmutableArray will not be implemented, there are better native C++ choices without me reinventing the wheel.
- Currently implements as far as Ashley commit [#a276fe45](https://github.com/libgdx/ashley/commit/a276fe45c81d450f305ce1b5b0bd0fe837207a70).

Features to be implemented:

- Refactor entity ids, using a C++11 constexpr compile time hash and a templated Component class to prevent using a virtual method and vtable. Will lead to smaller components and better cache coherency.

### License

AshleyCPP has the same license as the original, as shown here:

> Ashley is licensed under the [Apache 2 License](https://github.com/libgdx/ashley/blob/master/LICENSE), meaning you
> can use it free of charge, without strings attached in commercial and non-commercial projects. We love to
> get (non-mandatory) credit in case you release a game or app using Ashley!

A mention if you find AshleyCPP useful would be very much appreciated!
