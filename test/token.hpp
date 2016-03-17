#pragma once

#include "mettle/header_only.hpp"

using mettle::expect;
using mettle::equal_to;
using mettle::filter;


struct TokenWatcher {
  int basic_constructed = 0;
  int copy_constructed = 0;
  int move_constructed = 0;
  int destructed = 0;
  int destructed_tokens = 0;

  int constructed() {
    return basic_constructed + copy_constructed + move_constructed;
  }

};

template <int I=0>
struct Token {
  Token(TokenWatcher* tw, int value=0): token_watcher(tw), value(value){
    token_watcher->basic_constructed++;
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
    if (has_token) {
      token_watcher->destructed_tokens++;
    }
  };

  void give_token() {
    has_token = true;
  }

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

auto has_token = filter(
    [](auto && x) {return x.has_token;},
    equal_to(true)
);
