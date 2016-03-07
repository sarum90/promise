
-include local.mak

default: test

test: test_main test_unique_promise

test_main:
	$(RUNNER) ./test.sh main

test_unique_promise:
	$(RUNNER) ./test.sh unique_promise

install_deps:
	echo "Installing Dependencies"
	git submodule init
	git submodule update

clean:
	rm -rf build

.PHONY: default test test_main test_unique_promise
