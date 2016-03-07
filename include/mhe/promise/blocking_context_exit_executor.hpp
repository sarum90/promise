#pragma once

#include "mhe/promise/util.hpp"

MHE_PROMISE_START_NAMESPACE

class BlockingContextExitExecutor {
  public:
    BlockingContextExitExecutor() {}

    ~BlockingContextExitExecutor() {
      run_all();
    }

    void schedule(util::unique_function<void()> function) {
      _run_queue.push_back(std::move(function));
    }


  private:
    std::deque<util::unique_function<void()>> _run_queue;

    void run_all() {
      while(!_run_queue.empty()) {
        _run_queue.front()();
        _run_queue.pop_front();
      }
    }
};

MHE_PROMISE_END_NAMESPACE
