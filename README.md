# Promise

Promises/Deferred/Futures for C++

# How to develop:

## Natively on your computer

1. Install clang++-3.7 (Ensure it can be run as `clang++-3.7`)
2. run `make test`

Installing clang++-3.7 is a bit non-trivial. On an Ubuntu machine, if you are
lucky, you might be able to just run `image/load.sh`. This is provided without
support, and is more just an approximate documentation of how I set up my build
environment.

See `image/Dockerfile` for another way to set up the environment on Ubuntu
16.04.

## Using docker

1. run `cp local.mak.docker local.mak`
2. run `make test`

This sets an environment variable in the Makefile that will use `./run.sh` to
run all actual build commands. `./run.sh` is a small bash script that runs build
commands in a docker container. The docker container has a full clang 3.7 build
environment. The docker container is built from `image/makeimage.sh` and
`image/Dockerfile`.

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
    - Then - takes a callable that can receive any of S and returns R or Result<R, Fr>.
      - Returns ChainLink<R, F+Fr>
    - Catch - takes a callable that can receive any of F and returns R or Result<R, Fr>
      - Returns ChainLink<S+R, Fr>
    - ThenCatch - takes two callables as above that must return the same R and Fr
      - Returns ChainLink<R, Fr>
