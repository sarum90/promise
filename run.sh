#!/bin/bash

docker run --rm -v `pwd`:/wd -w /wd sarum90/cppdev "$@"
