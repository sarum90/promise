
#include "mhe/promise/unique_promise.hpp"
#include "mettle/header_only.hpp"
#include "mhe/promise/blocking_context_exit_executor.hpp"

#include "cxx_function.hpp"

#include "will_be_called.hpp"

#include <iostream>

using std::endl;
using std::cout;

using MHE_PROMISE_SCOPE::BlockingContextExitExecutor;
using MHE_PROMISE_SCOPE::CreateUniquePromise;

// TODO(sarum90): Before these proliferate, consider writing a wrapper around
// expect. I'm not crazy about the fact that it just raises an exception.
// 
// Also expect uses some source_location magic header that isn't available in
// clang yet.
using mettle::suite;
using mettle::expect;
using mettle::equal_to;
using mettle::filter;


struct TokenWatcher {
  int constructed = 0;
  int copy_constructed = 0;
  int move_constructed = 0;
  int destructed = 0;
};

template <int I=0>
struct Token {
  Token(TokenWatcher* tw, int value=0): token_watcher(tw), value(value){
    token_watcher->constructed++;
  }

  Token(const Token& other):
      token_watcher(other.token_watcher),
      value(other.value) {
    // Copying does not give you a token.
    token_watcher->copy_constructed++;
  }

  Token(Token&& other):
      token_watcher(other.token_watcher),
      value(other.value),
      has_token(other.has_token) {
    other.has_token = false;
    token_watcher->move_constructed++;
  }

  ~Token(){
    token_watcher->destructed++;
  };

  TokenWatcher * token_watcher;
  int value;
  bool has_token = false;
};

auto has_value = [](int v) {
  return filter(
      [](auto && x) {return x.value;},
      equal_to(v)
  );
};


suite<> unique_promise("Tests for UniquePromise", [](auto &_){

    _.test("Verify our tokens work as expected.", []() {
      TokenWatcher tw;
      {
        Token<> t1(&tw, 3);
        t1.has_token = true;
        Token<> t2(t1);
        Token<> t3(std::move(t1));
        expect(t1.has_token, equal_to(false));
        expect(t1, has_value(3));
        expect(t2.has_token, equal_to(false));
        expect(t2, has_value(3));
        expect(t3.has_token, equal_to(true));
        expect(t3, has_value(3));
      }
      expect(tw.copy_constructed, equal_to(1));
      expect(tw.constructed, equal_to(1));
      expect(tw.move_constructed, equal_to(1));
      expect(tw.destructed, equal_to(3));
    });

    _.test("Simple Case.", []() {
      WillBeCalled c;
      TokenWatcher tw;
      {
        BlockingContextExitExecutor b;
        auto unique_promise_to_test =
            CreateUniquePromise<Token<1>, Token<2>>(&b);
        unique_promise_to_test.thenable.then([&c](auto&& x) {
            c.Call();
            expect(x, has_value(12));
            expect(x.has_token, equal_to(true));
          });
        Token<2> result(&tw, 12);
        result.has_token = true;
        unique_promise_to_test.Resolve(std::move(result));
        expect(c, was_not_called());
      }
      expect(tw.copy_constructed, equal_to(0));
      expect(c, was_called());
    });

    _.test("Mapped Case.", []() {
      WillBeCalled c;
      WillBeCalled c2;
      TokenWatcher tw;
      TokenWatcher tw2;

      {
        BlockingContextExitExecutor b;
        auto unique_promise_to_test =
            CreateUniquePromise<Token<1>, Token<2>>(&b);
        auto y = unique_promise_to_test.thenable.then([&c, &tw2](auto&& x) {
            c.Call();
            expect(x, has_value(12));
            expect(x.has_token, equal_to(true));
            auto retval = Token<5>(&tw2, 55);
            retval.has_token = true;
            return std::move(retval);
          });
        // BREAK
        y.then([&c2](auto&& x) -> void {
            c2.Call();
            expect(x.has_token, equal_to(true));
            expect(x, has_value(55));
          });
        Token<2> result(&tw, 12);
        result.has_token = true;
        unique_promise_to_test.Resolve(std::move(result));
        expect(c, was_not_called());
        expect(c2, was_not_called());
      }
      expect(tw.copy_constructed, equal_to(0));
      expect(c, was_called());
      expect(c2, was_called());
    });

});
