AshleyCPP
=========

AshleyCPP is a lightweight, cross-platform and easy to use Entity framework for C++11 based on the Java entity framework
[Ashley](https://github.com/libgdx/ashley/). It contains the tools you need to create an application using data-oriented
design and also includes a generic object-pooling system to minimise dynamic allocation during runtime.

It prioritises speed and simplicity and mirrors the public API of the original where possible to make it easy to learn,
quick to run and simple to debug.

The original is inspired by the [Ash](http://www.ashframework.org/) and
[Artemis](http://gamadu.com/artemis/) frameworks.

AshleyCPP is a personal project and while there are no guarantees that it will compile and work, the master branch
on GitHub should compile and function properly and can be (and has been) used to make projects.

### Building

AshleyCPP uses the CMake build system and comes with three build targets; debug, release and test.

To build the library from source, install a relatively recent version of CMake, navigate to a copy of the source and
do the following:

    mkdir build
    cd build
    cmake -G "<your platform here>" ..

and then proceed to build in your preferred environment. For example using makefiles on Unix:

    cmake -G "Unix Makefiles"
    make # builds the release (libAshleyCPP) and debug (libAshleyCPP-d) libraries
    make AshleyCPPTest # for building the test suite executable


### Usage Notes and API Changes
While AshleyCPP strives to match the exported public API of the Java original, differences in the languages mean that some changes must be made. Such changes are listed below.

Note: In situations where you'd use `.class` to get the type of a Java class, in C++ you can `#include <typeinfo>` and:

    typeid(ComponentClass); // Equivalent to ComponentClass.class in Java.  
    
    // e.g. create a Family that matches Entity instances with ComponentA and ComponentB
    ashley::Family::getFor({typeid(ComponentA), typeid(ComponentB)});
     
- General
  - The whole library is enclosed in the `ashley` namespace. Use `using namespace ashley;` to save typing if you want.
  - You can `#include` individual headers (organised in a similar way to the Java packages) or use `#include "Ashley/AshleyCore.hpp"`
  - Java generic functions that take vararg lists of `Class<? extends Component>` are replaced by
    `std::initializer_list<std::type_index>`. Some functions also provide template overloads which may be
    easier and faster to use; check the documentation for specific occurrences.
  - Many places where references to class type in Java were passed to a method are replaced by passing an `std::shared_ptr`.
  
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
- Util
  - [x] ObjectPool
  - [x] Poolable
  
Roadmap from commits to main version since last major release:
- [x] Check "Family filtering issue" - [Java Commit](https://github.com/libgdx/ashley/commit/325223d82935138fc5a28505660c2b23a05cfc6b)
- [x] Add `EntitySystem::setProcessing` and update Engine tests
- [x] Add `Engine::getSystems` and test
- [x] Check issue `removing Entities mid iteration` and add tests - [Java Commit](https://github.com/libgdx/ashley/commit/a2a63f4e42e09e3221331b2333e675b3a4ab6fe3)
- [x] Add IntervalIteratingSystem and IntervalSystem - [Java Commit](https://github.com/libgdx/ashley/commit/47bf907b15ad8ed4297a10eb6b6b311e1542dcb8)

  
*NB:*
- Bag and ImmutableArray will not be implemented, there are better native C++ choices without me reinventing the wheel.
- PooledEngine may be done in future, but is left out for now.

### License

AshleyCPP has the same license as the original, as shown here:

> Ashley is licensed under the [Apache 2 License](https://github.com/libgdx/ashley/blob/master/LICENSE), meaning you
> can use it free of charge, without strings attached in commercial and non-commercial projects. We love to
> get (non-mandatory) credit in case you release a game or app using Ashley!

A mention if you find AshleyCPP useful would be very much appreciated!