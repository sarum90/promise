#pragma once

#include "mhe/promise/util.hpp"
#include "mhe/promise/callback.hpp"
#include <cassert>
#include <iostream>

MHE_PROMISE_START_NAMESPACE

template <typename Exectuor, typename Result>
class unique_thenable;

template <typename Result, typename Executor>
struct resolvable_thenable{
  resolvable_thenable(
    util::unique_function<void(Result&&)>&& r,
    unique_thenable<Executor, Result>&& t
  ): resolve(std::move(r)), thenable(std::move(t)){}

  util::unique_function<void(Result&&)> resolve;

  unique_thenable<Executor, Result> thenable;
};

template <typename ResolveType, typename Executor>
auto create_unique_thenable(Executor * e) {
  std::shared_ptr<unique_callback<Executor, ResolveType>>
    r(new unique_callback<Executor, ResolveType>(e));
  return resolvable_thenable<ResolveType, Executor>(
    [cb=r](auto&& x){
      cb->set_value(x);
    },
    unique_thenable<Executor, ResolveType>(e, r)
  );
}

template <typename T, typename V>
struct function_types {
  using return_type = decltype(std::declval<T>()(std::declval<V>()));
};

template <typename R>
struct do_then {
  template <typename T, typename S, typename E, typename C>
  static auto execute(T&& f, S* s, E&& e, C&& c){
    auto r = create_unique_thenable<
      typename function_types<T, typename S::Result>::return_type,
      typename S::Executor
    >(e);

    c->set_callback(
      [func=std::forward<T>(f),
       resolve=std::move(r.resolve)](typename S::Result&& x) mutable
      {
        resolve(func(std::forward<typename S::Result>(x)));
      }
    );

    return std::move(r.thenable);
  }
};

template <>
struct do_then<void> {
  template <typename T, typename S, typename E, typename C>
  static auto execute(T&& f, S* s, E&& e, C&& c){
    c->set_callback(std::forward<T>(f));
  }
};


template <typename E, typename R>
class unique_thenable {

  // TODO(sarum90): Make this threadsafe.
  
  public:
    using Executor = E;
    using Result = R;

    unique_thenable(
        Executor* executor,
        std::shared_ptr<unique_callback<Executor, Result>> cb):
      _executor(executor),
      _callback(cb)
    {}

    unique_thenable(unique_thenable<Executor, Result> & other) = delete;

    unique_thenable(unique_thenable<Executor, Result> && other):
      _executor(other._executor),
      _callback(other._callback)
    { }
  
    template <typename T>
    auto then(T&& f)
    {
      return do_then<
          typename function_types<T, Result>::return_type
        >::execute(f, this, _executor, _callback);
    }

  private:
    Executor* _executor;
    std::shared_ptr<unique_callback<Executor, Result>> _callback;
};

MHE_PROMISE_END_NAMESPACE
