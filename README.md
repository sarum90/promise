# Promise

Promises/Deferred/Futures for C++

# How to develope:

## Natively on your computer

1) Install clang++-3.7
2) run `make test`

## Using docker

1) ???
2) run `make test`

# Design Thoughts:

Let T denote a type that is either movable or copyable.

Let Result<R, F> represent a value that has either type R or F.

A promise is constructed with:
  - a set of Ts (S) that it can be resolved to.
  - a set of Ts (F) that it can be rejected to.

The promise needs to provide 3 interfaces.
  - a "callable" that can be called with any of S.
  - a "callable" that can be called with any of F.
  - an ChainLink<S, F> that has the following methods:
    - Then - takes a callable that can recieve any of S and returns R or Result<R, Fr>.
      - Returns ChainLink<R, F+Fr>
    - Catch - takes a callable that can recieve any of F and returns R or Result<R, Fr>
      - Returns ChainLink<S+R, Fr>
    - ThenCatch - takes two callables as above that must return the same R and Fr
      - Returns ChainLink<R, Fr>
