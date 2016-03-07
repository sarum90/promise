#pragma once

#include "mhe/promise/util.hpp"

#include <memory>
#include <deque>

MHE_PROMISE_START_NAMESPACE

template <class ResultType, class Executor>
class ResolveCallback;

namespace PromiseStates {
  enum State {
      PENDING,
      RESOLVED,
      REJECTED
  };
}

template <class ResultType, class Executor>
struct PromiseGlue {
  explicit PromiseGlue(Executor * e): executor(e){}

  void Resolve(ResultType x) {
    std::cout << "Resolve called:" << x << std::endl;
    if(state != PromiseStates::PENDING) {
      std::cerr << "RESOLVING NON-PENDING PROMISE" << std::endl;
      return;
    }
    state = PromiseStates::RESOLVED;
    result = x;
    while (!resolve_callbacks.empty()) {
      resolve_callbacks.front()(result);
      resolve_callbacks.pop_front();
    }
    resolve_callbacks.clear();
  }


  Executor * executor;
  std::deque<util::unique_function<void(ResultType)>> resolve_callbacks;
  std::deque<util::unique_function<void(ResultType)>> reject_callbacks;

  PromiseStates::State state = PromiseStates::PENDING;

  union {
    ResultType result;
  };
};

template <class ResultType, class Executor>
class PromiseImpl {
  public:
    PromiseImpl(Executor* e):
      _promise_glue(new PromiseGlue<ResultType, Executor>(e)),
      resolve(_promise_glue),
      reject(_promise_glue)
  {}

    template <class ThenLambda>
    auto Then(ThenLambda tl) {
      if(_promise_glue->state == PromiseStates::PENDING) {
        _promise_glue->resolve_callbacks.push_back(tl);
      } else if (_promise_glue->state == PromiseStates::RESOLVED) {
        _promise_glue->executor->schedule(
            [promise_glue=_promise_glue, tl=std::move(tl)](){
              tl(promise_glue->result);
            }
        );
      }
    }

    template <class CatchLambda>
    auto Catch(CatchLambda tl) {
      std::cout << "Catching" << std::endl;
    }

  private:
    std::shared_ptr<PromiseGlue<ResultType, Executor>> _promise_glue;
  public:
    ResolveCallback<ResultType, Executor> resolve;
    ResolveCallback<ResultType, Executor> reject;
};

template <class ResultType, class Executor, class Initializer>
auto Promise(Executor * e, Initializer i) {
  auto p = PromiseImpl<ResultType, Executor>(e);
  auto resolve = p.resolve;
  auto reject = p.reject;
  e->schedule([i, resolve, reject](){i(resolve, reject);});
  return p;
}

template <class ResultType, class Executor>
class ResolveCallback {
  public:
    ResolveCallback(
        std::shared_ptr<PromiseGlue<ResultType, Executor>> promise_glue):
      _promise_glue(promise_glue){}

    void operator()(ResultType x) {
      _promise_glue->Resolve(x);
    }
  private:
    std::shared_ptr<PromiseGlue<ResultType, Executor>> _promise_glue;
};

template <class ResultType, class Executor>
class RejectCallback {
  public:
    RejectCallback(
        std::shared_ptr<PromiseGlue<ResultType, Executor>> promise_glue):
      _promise_glue(promise_glue){}

    void operator()(ResultType x) {
      _promise_glue->Reject(x);
    }
  private:
    std::shared_ptr<PromiseGlue<ResultType, Executor>> _promise_glue;
};


MHE_PROMISE_END_NAMESPACE
