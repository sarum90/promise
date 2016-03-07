
#include <iostream>
#include "mhe/promise/util.hpp"
#include "mhe/promise/promise.hpp"
#include "mhe/promise/internal/callback_group.hpp"
#include "mhe/promise/blocking_context_exit_executor.hpp"

#include "mettle/header_only.hpp"

#include "will_be_called.hpp"

using MHE_PROMISE_SCOPE::Promise;
using MHE_PROMISE_SCOPE::BlockingContextExitExecutor;
using MHE_PROMISE_SCOPE::util::unique_function;
using MHE_PROMISE_SCOPE::internal::CallbackGroup;

// TODO(sarum90): Before these proliferate, consider writing a wrapper around
// expect. I'm not crazy about the fact that it just raises an exception.
using mettle::suite;
using mettle::expect;
using mettle::equal_to;
using mettle::skip;


suite<> basic("Basic tests for Promises", [](auto &_) {

  // Simple test to verify the basic resolved promise use case.
  _.test("test_happy_case", [](){
    WillBeCalled c;
    {
      BlockingContextExitExecutor b;
      auto p = Promise<int>(&b, [](auto resolve, auto reject) {
          resolve(5);
      });

      p.Then([&c](auto i){
        c.Call();
        expect(i, equal_to(5));
      });
    }
    expect(c, was_called());
  });


  // Simple test to verify the basic rejected promise use case.
  _.test("test_sad_case", {skip}, [](){
    WillBeCalled c;
    {
      BlockingContextExitExecutor b;
      auto p = Promise<int>(&b, [](auto resolve, auto reject) {
          reject(5);
      });

      p.Catch([&c](auto i){
        c.Call();
        expect(i, equal_to(5));
      });
    }
    expect(c, was_called());
  });

  _.test("test_callback_group", [](){
    WillBeCalled c1;
    WillBeCalled c2;
    WillBeCalled c3;
    CallbackGroup<int> cg;
    cg.AddCallback([&](int i){c1.Call();});
    cg.AddCallback([&](int i){
      expect(i, equal_to(2));
      c2.Call();
    });
    cg.Call(2);
    cg.AddCallback([&](int i){ c3.Call(); });
    expect(c1, was_called());
    expect(c2, was_called());
    expect(c3, was_not_called());
  });

  //Doesn't work, maybe should error nicer?
  _.test("test_callback_group_movable_arg", {skip}, [](){
    bool called = false;
    {
      typedef std::unique_ptr<bool *> arg_type;
      arg_type boolarg = std::make_unique<bool *>(&called);
      std::cout << boolarg.get() << std::endl;
      CallbackGroup<arg_type> cg;
      cg.AddCallback([&](arg_type i){ std::cout << i.get() << std::endl; }); // dummy
      cg.AddCallback([&](arg_type i){ std::cout << i.get() << std::endl; **i = true; });
      cg.Call(std::move(boolarg));
    }
    expect(called, equal_to(true));
  });

  _.test("test_callback_group_movable_function", [](){
    bool called = false;
    {
      auto bool_closure = std::make_unique<bool *>(&called);
      auto closure = [bool_closure=std::move(bool_closure)](int i){
        **bool_closure = true;
      };
      CallbackGroup<int> cg;
      cg.AddCallback(std::move(closure));
      cg.Call(2);
    }
    expect(called, equal_to(true));
  });

});
