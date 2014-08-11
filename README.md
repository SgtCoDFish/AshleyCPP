AshleyCPP
=========

AshleyCPP is an (unimaginatively named) C++ port of the Java entity framework [Ashley](https://github.com/libgdx/ashley/).

It prioritises speed, simplicity and ease of use, and mirrors the public API of the original where possible.

The original is inspired by the [Ash](http://www.ashframework.org/) and
[Artemis](http://gamadu.com/artemis/) frameworks.

A test suite is included, designed for the googletest platform (works on Eclipse).

Note that for now, AshleyCPP is a for-fun project and is not even guaranteed to compile. It may be stabilised when more is complete.

### License

AshleyCPP has the same license as the original, as shown here:

> Ashley is licensed under the [Apache 2 License](https://github.com/libgdx/ashley/blob/master/LICENSE), meaning you
> can use it free of charge, without strings attached in commercial and non-commercial projects. We love to
> get (non-mandatory) credit in case you release a game or app using Ashley!

A mention if you find AshleyCPP useful would be very much appreciated!

### Implementation Status
Ticked classes have both their implementation and tests complete.
- Core
  - [ ] Component
  - [ ] ComponentMapper
  - [ ] ComponentType
  - [ ] Engine
  - [ ] Entity
  - [ ] EntityListener
  - [ ] EntitySystem
  - [ ] Family
  - [ ] PooledEngine
- Signals
  - [ ] Signal
  - [ ] Listener
- Systems
  - [ ] IteratingSystem
  
NB: Bag and ImmutableArray will not be implemented, there are better native C++ choices (std::vector and const iterators respectively).