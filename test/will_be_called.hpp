#pragma once

#include "mettle/matchers.hpp"

// =============================
// 
// Tiny utility object and matcher for verifying that some code was executed as
// part of the test.
//
class WillBeCalled {
  public:
    WillBeCalled(){}

    ~WillBeCalled(){
    }

    void Call() {
      _called = true;
    }

    bool WasCalled() const {
      return _called;
    }

  private:
    bool _called = false;
};

auto was_called = []() {
  return mettle::filter(
      [](auto && x) {return x.WasCalled();},
      mettle::equal_to(true),
      "Callback was called: "
  );
};

auto was_not_called = []() {
  return mettle::filter(
      [](auto && x) {return x.WasCalled();},
      mettle::equal_to(false),
      "Callback was called: "
  );
};

// =============================
