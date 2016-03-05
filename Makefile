
default: test

test:
	./test.sh

clean:
	rm -rf build

.PHONY: test
