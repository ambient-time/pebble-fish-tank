# Copyright 2026 Luke Steuber. MIT License.
PEBBLE ?= pebble
PYTHON ?= python3
HOST_CC ?= clang
.PHONY: build test
build: test
	$(PEBBLE) build --sdk 4.33.1
test:
	mkdir -p build
	$(HOST_CC) -std=c11 -fsanitize=address,undefined -Wall -Wextra -Werror test/math.c src/c/tank.c src/c/tide.c -lm -o build/math-test
	./build/math-test
	node --test test/tide.test.cjs

package:
	$(PYTHON) tools/release.py
