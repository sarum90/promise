
-include local.mak

default: test

test:
	$(RUNNER) ./test.sh

install_deps:
	echo "Installing Dependencies"
	git submodule init
	git submodule update

clean:
	rm -rf build

.PHONY: test
