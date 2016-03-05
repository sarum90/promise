#pragma once

#include "function.hpp"

#ifndef MHE_PROMISE_START_NAMESPACE
#define MHE_PROMISE_START_NAMESPACE namespace mhe { namespace promise {
#define MHE_PROMISE_END_NAMESPACE } }
#define MHE_PROMISE_SCOPE mhe::promise
#endif

MHE_PROMISE_START_NAMESPACE

namespace util {

template <class T>
using unique_function = fu2::unique_function<T>;

};


MHE_PROMISE_END_NAMESPACE
