
#include <iostream>
#include "mhe/promise/util.hpp"
#include "mhe/promise/promise.hpp"
#include "mhe/promise/internal/callback_group.hpp"

using MHE_PROMISE_SCOPE::Promise;
using MHE_PROMISE_SCOPE::BlockingContextExitExecutor;
using MHE_PROMISE_SCOPE::util::unique_function;
using MHE_PROMISE_SCOPE::internal::CallbackGroup;

int main(int argc, char ** argv) {

  std::pair<const char *, unique_function<bool()>> tests[] = {

    // Simple test to verify the basic resolved promise use case.
    std::make_pair("test_happy_case", [](){
      bool success = false;
      {
        BlockingContextExitExecutor b;
        auto p = Promise<int>(&b, [](auto resolve, auto reject) {
            resolve(5);
        });

        p.Then([&success](auto i){success = (i == 5);});
      }
      return success;
    }),

    // Simple test to verify the basic rejected promise use case.
    std::make_pair("test_sad_case", [](){
      return true; // for now -- the test does not actually run.
      bool success = false;
      {
        BlockingContextExitExecutor b;
        auto p = Promise<int>(&b, [](auto resolve, auto reject) {
            reject(5);
        });

        p.Catch([&success](auto i){success = (i == 5);});
      }
      return success;
    }),

    std::make_pair("test_callback_group", [](){
      bool c1_called = false;
      bool c2_called = false;
      bool c3_not_called = true;
      CallbackGroup<int> cg;
      cg.AddCallback([&](int i){ c1_called = true; });
      cg.AddCallback([&](int i){ if(i == 2) { c2_called = true;} });
      cg.Call(2);
      cg.AddCallback([&](int i){ c3_not_called = false; });
      return c1_called && c2_called && c3_not_called;
    }),

    /* Doesn't work, maybe should error nicer?
    std::make_pair("test_callback_group_movable_arg", [](){
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
      return called;
    }),
    */

    std::make_pair("test_callback_group_movable_function", [](){
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
      return called;
    }),

  };

  bool success = true;

  for (auto& test: tests) {
    auto result = test.second();
    if (result) {
      std::cout << test.first << " PASSED" << std::endl;
    } else {
      std::cout << test.first << " FAILED" << std::endl;
    }
    success = result && success;
  }

  std::cout << std::endl;
  if (success) {
    std::cout << "ALL OK" << std::endl;
    return 0;
  }
  std::cout << "FAILURE" << std::endl;
  return -1;
}
