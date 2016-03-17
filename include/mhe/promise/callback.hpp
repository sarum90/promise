#pragma once

#include "mhe/promise/util.hpp"
#include <cassert>
#include <iostream>

MHE_PROMISE_START_NAMESPACE

template <typename E, typename T>
class unique_callback {

  // TODO(sarum90): Make this threadsafe.
  
  public:

    explicit unique_callback(E* executor):
      _executor(executor),
      _storage_ptr(nullptr, &destruct)
    {}

    unique_callback(unique_callback<E, T> & other) = delete;

    unique_callback(unique_callback<E, T> && other):
      _executor(other._executor),
      _function(std::move(other._function)),
      _storage_ptr(new (storage()) T(std::forward<T>(other.value())))
    {
    }
  
    void set_callback(util::unique_function<void(T&&)>&& f) {
      assert(!_function_set);
      _function_set = true;
      _function = std::move(f);
      if (storage_set()) {
        schedule();
      }
    }

    template <typename S>
    void set_value(S&& value) {
      assert(!storage_set());
      T* v = new (storage()) T(std::forward<S>(value));
      _storage_ptr = std::unique_ptr<T, decltype(&destruct)>(v, &destruct);
      if(_function_set) {
        schedule();
      }
    }

  private:
    void schedule() {
      assert(_function_set);
      assert(storage_set());
      _executor->schedule([func=std::move(_function),
                           value=std::forward<T>(value())] () mutable {
        func(std::forward<T>(value));
      });
    }

    void * storage() {
      return static_cast<void *>(&_storage);
    }

    T& value() {
      return *_storage_ptr;
    }

    static void destruct(T* t) {
      t->~T();
    }

    E* _executor;
    util::unique_function<void(T&&)> _function;
    char _storage[sizeof(T)];


    bool storage_set() {
      return _storage_ptr.get() != nullptr;
    }

    std::unique_ptr<T, decltype(&destruct)> _storage_ptr;
    bool _function_set = false;

};

MHE_PROMISE_END_NAMESPACE
