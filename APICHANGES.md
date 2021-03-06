This file contains all the API changes between this C++ port and the original Java version of Ashley.

### Changes ###

- Component
  - `virtual std::type_index identify() const;`  
    Added to help with type identification; can be called by Component-derived types using a base class `Component *`
    to identify the derived class polymorphically.
    
- ComponentType  
  - `operator==` and `operator!=`  
  Two ComponentTypes are equal if they have the same index, i.e. they represent the same type.
  
- ComponentMapper
  - `getMapper()`   
  Renamed from `getFor` to make more sense since this templated version needs no non-template arguments.
  
- Engine
  - `getEntitiesFor`  
  The C++ version is mutable but should not be changed; you get a pointer to a vector of `Entity*`. Modifiying the
  vector may cause errors and is not supported.
  - `SystemComparator` -> `bool systemPriorityComparator()`
  Private static comparator class in Java becomes a public static comparison function in C++.
  
- Entity
  - `add` and `remove`  
  Instead of accepting a component directly, can take the templated Component type and perfect forward into the
  appropriate (copy-/move-)constructor which saves typing and increases efficiency.
  Also provides an overload taking components as rvalue references to unique_ptrs which are moved from.
  - `getComponents()`  
  The C++ version is mutable but shouldn't be changed; you get a pointer to a vector of `Component *`. Modifiying
  the vector may cause errors and is not supported.
  - `operator==` and `operator!=`  
  Two entities compare equal if they have the same index number.
  - Relational comparison operators `<, <=, >, >=`  
  Defined based on index number.
  - `toggleComponentOperationHandler()`   
  Allows power users to temporarily interfere with how components are added or removed from Entities by changing the 
  attached component operation handler. Should be used with extreme caution.
  
- EntitySystem
  - `virtual std::type_index identify() const;`  
    Same reasons as with `Component::identify()`.
  - `=, !=, <, <=, >, >=` Comparison operators  
  Defined based on priority; lower priority means first to execute, so `a < b == true` means `a` will execute first.
  
- Family
  - Constructor  
    There is a visible constructor to make Family usable with smart pointer classes. The first argument is a private dummy type
    and you shouldn't try to use it; use a version of Family::getFor instead.
  - `hashCode()` -> `std::hash`  
  Family.hpp contains a specialisation of `std::hash` to allow Family instances to be used as keys in
  `std::unordered_map`s. It uses the same hash as in Java.
  - `operator==` and `operator!=`
  Defined based on index; two families are equal if they represent the same selection of components.
  
- ObjectPool<T> and Poolable
  - Similar to LibGDX's Pool class and Pool.Poolable interface. See the docs for more details about these classes.
  - Both found in `#include "Ashley/util/ObjectPools.hpp"`