CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -fPIC
CXX=g++
CXXFLAGS=-std=c++03 -Wall -Wextra -pedantic -fPIC
LDFLAGS=-shared

ifdef DEBUG
CFLAGS+=-g
CXXFLAGS+=-g
endif
ifndef DEBUG
CFLAGS+=-O3
CXXFLAGS+=-O3
endif

VSQL?=vsql
MEMTEST?=valgrind --error-exitcode=1 --tool=memcheck --leak-check=full

SDK?=/opt/vertica/sdk
VERTICA_SDK_INCLUDE=$(SDK)/include
INCLUDE=include/


.PHONY: build test memtest examples clean


# Auxiliary tasks.

build: lib/JsonLib.so

test: build/test/json_selector build/test/json_slice
	./build/test/json_selector
	./build/test/json_slice

memtest: build/test/json_selector build/test/json_slice
	$(MEMTEST) ./build/test/json_selector
	$(MEMTEST) ./build/test/json_slice

examples:
	$(VSQL) -f examples/simple.sql
	$(VSQL) -f examples/dots.sql
	$(VSQL) -f examples/table.sql
	$(VSQL) -f examples/strings.sql

install: ddl/install.sql lib/JsonLib.so
	$(VSQL) -f $<

uninstall: ddl/uninstall.sql
	$(VSQL) -f $<

clean:
	rm -rf build/ lib/


# Build the library.

lib/JsonLib.so: build/Vertica.o build/JsonQuery.o build/JsonUnnest.o build/json/selector.o build/json/slice.o
	mkdir -p `dirname $@`
	$(CXX) $(LDFLAGS) -o $@ $^

build/Vertica.o: $(VERTICA_SDK_INCLUDE)/Vertica.cpp
	mkdir -p `dirname $@`
	$(CXX) -I $(VERTICA_SDK_INCLUDE) $(CXXFLAGS) -c -o $@ $<

build/JsonQuery.o: src/JsonQuery.cpp include/json/slice.h include/json/selector.h
	mkdir -p `dirname $@`
	$(CXX) -I $(VERTICA_SDK_INCLUDE) -I $(INCLUDE) $(CXXFLAGS) -c -o $@ $<

build/JsonUnnest.o: src/JsonUnnest.cpp include/json/slice.h
	mkdir -p `dirname $@`
	$(CXX) -I $(VERTICA_SDK_INCLUDE) -I $(INCLUDE) $(CXXFLAGS) -c -o $@ $<

build/json/selector.o: src/json/selector.c include/json/selector.h
	mkdir -p `dirname $@`
	$(CC) -I $(INCLUDE) $(CFLAGS) -c -o $@ $<

build/json/slice.o: src/json/slice.c include/json/slice.h
	mkdir -p `dirname $@`
	$(CC) -I $(INCLUDE) $(CFLAGS) -c -o $@ $<


# Build test runners.

build/test/json_selector: test/json_selector.c build/json/selector.o
	mkdir -p `dirname $@`
	$(CC) -I $(INCLUDE) $(CFLAGS) -o $@ $^

build/test/json_slice: test/json_slice.c build/json/selector.o build/json/slice.o
	mkdir -p `dirname $@`
	$(CC) -I $(INCLUDE) $(CFLAGS) -o $@ $^
