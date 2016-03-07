#pragma once

#include "mhe/promise/util.hpp"

MHE_PROMISE_START_NAMESPACE

template<typename S, typename F, typename E>
struct UniqueThenable{

  void then(util::unique_function<void (S&&)> s) {
  }

  E* executor;
};

template<typename S, typename F, typename E>
struct ConstructedUniquePromise{
  void Resolve(S&& s) {

  }
  util::unique_function<void(F&&)> reject;
  UniqueThenable<S, F, E> thenable;
};

template <typename S, typename F, typename E>
ConstructedUniquePromise<S, F, E> CreateUniquePromise(E* executor) {
    ConstructedUniquePromise<S, F, E> retval;
    return retval;
}

MHE_PROMISE_END_NAMESPACE
