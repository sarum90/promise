#pragma once

#include "mhe/promise/util.hpp"
#include <cassert>
#include <iostream>
#include "variant.hpp"
#include "cxx_function.hpp"

using std::cout;
using std::endl;

MHE_PROMISE_START_NAMESPACE

template <typename E, typename ... S>
struct UniquePromiseResultHolder {
  using Holder = typename mapbox::util::variant<S...>;

  typedef cxx_function::unique_function<void(S&&)...> F;

  UniquePromiseResultHolder(E* e): executor(e){}

  void resolve(Holder&& r) {
    assert(!resolved);
    resolved = true;
    value.reset(new Holder(std::move(r)));
    if (func_set) {
      schedule();
    }
  }

  void add_callback(F &&f) {
    assert(!func_set);
    func_set = true;
    func = std::move(f);
    if (resolved) {
      schedule();
    }
  }


  private:
    void schedule() {
      executor->schedule([func=std::move(func),
                          value=std::move(value)] () mutable {
          mapbox::util::apply_visitor(
            [f=std::move(func)](auto& x) mutable {
              f(std::move(x));
            },
            *value
          );
      });
    }

    std::unique_ptr<Holder> value;
    F func;
    bool func_set = false;
    bool resolved = false;
    E* executor;
};

template <typename T, typename ... Args>
struct my_result_of {
  using type = decltype(
          mapbox::util::apply_visitor(
              std::declval<T>(),
              mapbox::util::variant<Args...>()
            )
      );
};

template<typename E, typename ... S>
struct UniqueThenable;

template<typename E, typename ... S>
struct ConstructedUniquePromise;

template <typename ... S, typename E>
ConstructedUniquePromise<E, S...> CreateUniquePromise(E* executor);


template<typename E, typename Result>
struct my_then {
  using result_type = UniqueThenable<E, Result>;

  template <typename H, typename C>
  static result_type then(E* executor, H* holder, C&& f) {
    auto p = CreateUniquePromise<Result>(executor);
    holder->add_callback(std::move([f=std::move(f), p=p](auto&& x) mutable {
      auto&& res = f(std::move(x));
      p.Resolve(std::move(res));
    }));
    return std::move(p.thenable);
  }
};

template<typename E>
struct my_then<E, void> {
  using result_type = void;

  template <typename H, typename C>
  static void then(E* executor, H* holder, C&& f) {
    holder->add_callback(std::move(f));
  }

};

template<typename E, typename ... S>
struct UniqueThenable {

  UniqueThenable(std::shared_ptr<UniquePromiseResultHolder<E, S...>> r, E* e):
    executor(e), _result(r){}

  template <typename C>
  auto then(C&& f){
    return athen([f=std::move(f)](auto&& x) mutable {return f(x);});
  }

  template <typename C>
  typename my_then<E, typename my_result_of<C, S...>::type>::result_type
  athen(C&& f){

    return my_then<E, typename my_result_of<C, S...>::type>::then(
          executor, _result.get(), std::move(f));
  }

  E* executor;
  std::shared_ptr<UniquePromiseResultHolder<E, S...>> _result;
};

template<typename E, typename ... S>
struct ConstructedUniquePromise{
  ConstructedUniquePromise(E * executor):
    _result(new UniquePromiseResultHolder<E, S...>(executor)),
    thenable(_result, executor) {}

  template <typename C>
  void Resolve(C&& c) {
    _result->resolve(std::move(c));
  }

  private:
    std::shared_ptr<UniquePromiseResultHolder<E, S...>> _result;

  public:
  UniqueThenable<E, S...> thenable;
  
};

template <typename ... S, typename E>
ConstructedUniquePromise<E, S...> CreateUniquePromise(E* executor) {
    ConstructedUniquePromise<E, S...> retval(executor);
    return retval;
}

MHE_PROMISE_END_NAMESPACE
