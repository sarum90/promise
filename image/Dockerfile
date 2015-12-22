

FROM ubuntu:14.04

MAINTAINER Marcus Ewert <sarum90@gmail.com>

RUN apt-get update && apt-get install -y wget

RUN wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key|apt-key add -

# RUN echo "deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty main\ndeb-src http://llvm.org/apt/trusty/ llvm-toolchain-trusty main" >> /etc/apt/sources.list
RUN echo "deb http://llvm.org/apt/trusty/ llvm-toolchain-trusty main\ndeb-src http://llvm.org/apt/trusty/ llvm-toolchain-trusty main\n# 3.6\ndeb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.6 main\ndeb-src http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.6 main\n# 3.7\ndeb http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.7 main\ndeb-src http://llvm.org/apt/trusty/ llvm-toolchain-trusty-3.7 main" >> /etc/apt/sources.list



#RUN apt-get update && apt-get install -y clang-3.8 clang-3.8-doc libclang-common-3.8-dev libclang-3.8-dev libclang1-3.8 libclang1-3.8-dbg libllvm-3.8-ocaml-dev libllvm3.8 libllvm3.8-dbg lldb-3.8 llvm-3.8 llvm-3.8-dev llvm-3.8-doc llvm-3.8-examples llvm-3.8-runtime clang-modernize-3.8 clang-format-3.8 python-clang-3.8 lldb-3.8-dev liblldb-3.8-dbg

RUN apt-get update && apt-get install -y clang-3.7 clang-3.7-doc libclang-common-3.7-dev libclang-3.7-dev libclang1-3.7 libclang1-3.7-dbg libllvm-3.7-ocaml-dev libllvm3.7 libllvm3.7-dbg lldb-3.7 llvm-3.7 llvm-3.7-dev llvm-3.7-doc llvm-3.7-examples llvm-3.7-runtime clang-modernize-3.7 clang-format-3.7 python-clang-3.7 lldb-3.7-dev liblldb-3.7-dbg

