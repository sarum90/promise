#pragma once

#include "mhe/promise/util.hpp"

MHE_PROMISE_START_NAMESPACE

namespace internal {

template <class T>
class CallbackGroup {
  public:
    typedef util::unique_function<void(T&&)> Callback;

    void Call(T&& argument) {
      for (auto& c : _callbacks) {
        c(std::forward<T>(argument));
      }
    }

    void AddCallback(Callback&& c) {
      _callbacks.push_back(std::move(c));
    }

  private:
    std::deque<Callback> _callbacks;
};

} // namespace internal

MHE_PROMISE_END_NAMESPACE


