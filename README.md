AshleyCPP
=========

AshleyCPP is an (unimaginatively named) C++11 port of the Java entity framework [Ashley](https://github.com/libgdx/ashley/).

It prioritises speed, simplicity and ease of use, and mirrors the public API of the original where possible.

The original is inspired by the [Ash](http://www.ashframework.org/) and
[Artemis](http://gamadu.com/artemis/) frameworks.

A test suite is included, designed for the googletest platform (works on Eclipse).

Note that for now, AshleyCPP is a for-fun project and is not even guaranteed to compile. It may be stabilised when more is complete.
That said, it probably does compile and will probably work fine!


### Usage Notes and API Changes
While AshleyCPP strives to match the exported public API of the Java original, differences in the languages mean that some changes must be made. Such changes are listed below.

Note: In situations where you'd use `.class` to get the type of a Java class, in C++ you can use this instead:

    // at the top with the rest of the includes
    #include <typeinfo>
    
    typeid(ComponentClass); // Equivalent to ComponentClass.class in Java.  
    
    // e.g. create a Family that matches Entity instances with ComponentA and ComponentB
    ashley::Family::getFor({typeid(ComponentA), typeid(ComponentB)});
     
- General
  - The whole library is enclosed in the `ashley` namespace. Use `using namespace ashley;` to save typing if you want.
  - You can `#include` individual headers (organised in a similar way to the Java packages) or use `#include "Ashley/AshleyCore.hpp"`
  - Java generic functions that take vararg lists of `Class<? extends Component>` are replaced by
    `std::initializer_list<std::type_index>`. Some functions also provide variadic template overloads which may be
    easier to use; check the documentation for specific occurrences.
  - Many places where references to class type in Java were pass to a method are replaced by passing an `std::shared_ptr`.
- Component
  - `virtual std::type_index identify() const;`  
    Added to help with type identification; can be called by Component-derived types using a base class `Component *`
    to identify the derived class polymorphically.
- ComponentType
  - `getFor`, `getBitsFor` and `getIndexFor`  
  All have `std::type_index` and templated versions.
  - `operator==` and `operator!=`  
  Two ComponentTypes are equal if they have the same index, i.e. they represent the same type.
- ComponentMapper
  - `getRaw(std::shared_ptr<Entity>)`   
  Returns a const raw pointer for convenience; saves having to call `get()` on the `std::shared_ptr` return of `get()` for passing to a function.
  - `getMapper()`   
  Renamed from `getFor` to make more sense since this templated version needs no non-template arguments.
- Engine
  - Immutable `getEntitiesFor`  
  Not implemented, might be in the future.
  - `SystemComparator` -> `bool systemPriorityComparator()`
  Private static comparator class in Java becomes a public static comparison function in C++.
- Entity
  - `add` and `remove`  
  Instead of accepting a component directly, takes the templated Component type and perfect forwards into the appropriate (copy-/move-)constructor.
  - `getComponents()`  
  No longer immutable, might change in the future.
  - `getComponentWeak`  
  Same as getComponent but returns an `std::weak_ptr` instead.
  - `operator==` and `operator!=`  
  Two entities compare equal if they have the same index number.
  - Relational comparison operators `<, <=, >, >=`  
  Defined based on index number.
- EntitySystem
  - `virtual std::type_index identify() const;`  
    Same reasons as with `Component::identify()`.
  - `=, !=, <, <=, >, >=` Comparison operators  
  Defined based on priority; lower priority means first to execute, so `a < b == true` means a will execute first.
- Family
  - Constructor  
    There is a visible constructor to make Family usable with std::shared_ptr. The first argument is a private dummy type
    and you shouldn't try to use it; use a version of Family::getFor instead.
  - `hashCode()` -> `std::hash`  
  Family.hpp contains a specialisation of `std::hash` to allow Family instances to be used as keys in `std::unordered_map`s. It uses the same hash as in Java.
  - `operator==` and `operator!=`
  Defined based on index; two families are equal if they represent the same selection of components.

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
  
*NB:*
- Bag and ImmutableArray will not be implemented, there are better native C++ choices without me reinventing the wheel.
- PooledEngine may be done in future, but is left out for now.

### License

AshleyCPP has the same license as the original, as shown here:

> Ashley is licensed under the [Apache 2 License](https://github.com/libgdx/ashley/blob/master/LICENSE), meaning you
> can use it free of charge, without strings attached in commercial and non-commercial projects. We love to
> get (non-mandatory) credit in case you release a game or app using Ashley!

A mention if you find AshleyCPP useful would be very much appreciated!