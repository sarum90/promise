
#include "mhe/promise/callback.hpp"
#include "mettle/header_only.hpp"
#include "mhe/promise/blocking_context_exit_executor.hpp"

#include "token.hpp"
#include "will_be_called.hpp"

#include <iostream>

using std::endl;
using std::cout;

using MHE_PROMISE_SCOPE::BlockingContextExitExecutor;
using MHE_PROMISE_SCOPE::unique_callback;

// TODO(sarum90): Before these proliferate, consider writing a wrapper around
// expect. I'm not crazy about the fact that it just raises an exception.
// 
// Also expect uses some source_location magic header that isn't available in
// clang yet.
using mettle::suite;
using mettle::expect;
using mettle::equal_to;
using mettle::filter;


suite<> unique_promise("Tests for unique_callback", [](auto &_){

    _.test("unique_callback before value.", []() {
      WillBeCalled c;
      TokenWatcher tw;
      {
        BlockingContextExitExecutor b;
        unique_callback<BlockingContextExitExecutor, Token<1>> callback(&b);
        callback.set_callback(
          [&c](Token<1>&& t) {
            c.Call();
            expect(t, has_token);
            expect(t, has_value(5));
          }
        );
        Token<1> t(&tw, 5);
        t.give_token();
        callback.set_value(std::move(t));

        expect(c, was_not_called());
      }
      expect(tw.copy_constructed, equal_to(0));
      expect(c, was_called());
    });

    _.test("Value before callback.", []() {
      WillBeCalled c;
      TokenWatcher tw;
      {
        BlockingContextExitExecutor b;
        unique_callback<BlockingContextExitExecutor, Token<1>> callback(&b);
        Token<1> t(&tw, 5);
        t.give_token();
        callback.set_value(std::move(t));

        callback.set_callback(
          [&c](Token<1>&& t) {
            c.Call();
            expect(t, has_token);
            expect(t, has_value(5));
          }
        );

        expect(c, was_not_called());
      }
      expect(tw.copy_constructed, equal_to(0));
      expect(c, was_called());
    });

    _.test("Token destructed on unique_callback destruction.", []() {
      TokenWatcher tw;
      {
        BlockingContextExitExecutor b;
        unique_callback<BlockingContextExitExecutor, Token<1>> callback(&b);
        Token<1> t(&tw, 5);
        t.give_token();
        callback.set_value(std::move(t));
      }
      expect(tw.copy_constructed, equal_to(0));
      expect(tw.constructed(), equal_to(tw.destructed));
      expect(tw.destructed_tokens, equal_to(1));
    });

    _.test("Works with plain types.", []() {
      WillBeCalled c;
      {
        BlockingContextExitExecutor b;
        unique_callback<BlockingContextExitExecutor, int> callback(&b);
        callback.set_value(12);

        callback.set_callback(
          [&c](int i) {
            c.Call();
            expect(i, equal_to(12));
          }
        );

        expect(c, was_not_called());
      }
      expect(c, was_called());
    });

    _.test("Called with lvalues.", []() {
      WillBeCalled c;
      int I = 5;
      {
        BlockingContextExitExecutor b;
        unique_callback<BlockingContextExitExecutor, int> callback(&b);
        callback.set_value(I);

        callback.set_callback(
          [&c](int i) {
            c.Call();
            expect(i, equal_to(5));
          }
        );
        expect(c, was_not_called());
      }
      expect(c, was_called());
    });

    _.test("Lambda with references", []() {
      WillBeCalled c;
      int I = 5;
      {
        BlockingContextExitExecutor b;
        unique_callback<BlockingContextExitExecutor, int> callback(&b);
        callback.set_value(I);

        callback.set_callback(
          [&c, &I](const int& i) {
            c.Call();
            expect(i, equal_to(5));
          }
        );
        expect(c, was_not_called());
      }
      expect(c, was_called());
    });

    _.test("Lambda with references cleans up.", []() {
      WillBeCalled c;
      TokenWatcher tw;
      {
        BlockingContextExitExecutor b;
        unique_callback<BlockingContextExitExecutor, Token<1>> callback(&b);
        Token<1> t(&tw, 5);
        t.give_token();
        callback.set_value(std::move(t));

        callback.set_callback(
          [&c](const Token<1>& t) {
            c.Call();
            expect(t, has_token);
            expect(t, has_value(5));
          }
        );

        expect(c, was_not_called());
      }
      expect(tw.copy_constructed, equal_to(0));
      expect(tw.constructed(), equal_to(tw.destructed));
      expect(c, was_called());
    });

});
