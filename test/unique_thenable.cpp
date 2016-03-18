#include "mhe/promise/unique_thenable.hpp"
#include "mettle/header_only.hpp"
#include "mhe/promise/blocking_context_exit_executor.hpp"

#include "token.hpp"
#include "will_be_called.hpp"

#include <iostream>

using std::endl;
using std::cout;

using MHE_PROMISE_SCOPE::BlockingContextExitExecutor;
using MHE_PROMISE_SCOPE::create_unique_thenable;
using MHE_PROMISE_SCOPE::unique_thenable;

// TODO(sarum90): Before these proliferate, consider writing a wrapper around
// expect. I'm not crazy about the fact that it just raises an exception.
// 
// Also expect uses some source_location magic header that isn't available in
// clang yet.
using mettle::suite;
using mettle::expect;
using mettle::equal_to;
using mettle::filter;


suite<> unique_promise("Tests for unique_thenable", [](auto &_){

    _.test("unique_thenable Chains.", []() {
      WillBeCalled c;
      WillBeCalled c2;
      {
        BlockingContextExitExecutor b;

        auto r = create_unique_thenable<int>(&b);

        r.thenable.then([&c](int a) {
          c.Call();
          return a * 2;
        }).then([&c2](int b) {
          c2.Call();
          expect(b, equal_to(222));
          return b;
        });

        r.resolve(111);

        expect(c, was_not_called());
        expect(c2, was_not_called());
      }
      expect(c, was_called());
      expect(c2, was_called());
    });

    _.test("unique_thenable chains ending with void.", []() {
      WillBeCalled c;
      WillBeCalled c2;
      {
        BlockingContextExitExecutor b;

        auto r = create_unique_thenable<int>(&b);

        r.thenable.then([&c](int a) {
          c.Call();
          return a * 2;
        }).then([&c2](int b) {
          c2.Call();
          expect(b, equal_to(222));
        });

        r.resolve(111);

        expect(c, was_not_called());
        expect(c2, was_not_called());
      }
      expect(c, was_called());
      expect(c2, was_called());
    });

});
