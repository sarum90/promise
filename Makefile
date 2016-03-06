
default: test

test:
	./test.sh

install_deps:
	echo "Installing Dependencies"

clean:
	rm -rf build

.PHONY: test
