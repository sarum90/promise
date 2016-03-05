#!/bin/bash

set -u
set -e

apt-get update && apt-get install -y wget

wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|apt-key add -

echo -e "deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty main\ndeb-src http://llvm.org/apt/trusty/ llvm-toolchain-trusty main\n# 3.6\ndeb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.6 main\ndeb-src http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.6 main\n# 3.7\ndeb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.7 main\ndeb-src http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.7 main" >> /etc/apt/sources.list

apt-get update && apt-get install -y clang-3.7 clang-3.7-doc libclang-common-3.7-dev libclang-3.7-dev libclang1-3.7 libclang1-3.7-dbg libllvm-3.7-ocaml-dev libllvm3.7 libllvm3.7-dbg lldb-3.7 llvm-3.7 llvm-3.7-dev llvm-3.7-doc llvm-3.7-examples llvm-3.7-runtime clang-modernize-3.7 clang-format-3.7 python-clang-3.7 lldb-3.7-dev liblldb-3.7-dbg

