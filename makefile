CC=gcc
DBG=gdb
BIN=yamenu
PKGCONFIG = $(shell which pkg-config)

INCLUDEDIR=./src/include
SRCDIR=./src
ODIR=./obj
ODIR_TEST=$(ODIR)/test
BINDIR=./bin
TEST_DIR = ./tests

DEBUG=-DDEBUG
LIBS=$(shell $(PKGCONFIG) --libs gtk+-3.0)
CFLAGS=-Wall -g $(shell $(PKGCONFIG) --cflags gtk+-3.0) $(DEBUG)
LIBS_TEST=
CFLAGS_TEST=-Wall -g
OTHER_FLAGS=-rdynamic
MAIN = main
TEST_MAIN = test
INSTALLDIR = /usr/local/bin

TESTABLE_MODULES = utility data path logger sysio
MODULES = $(TESTABLE_MODULES) commandline gui

DEPS=$(patsubst %,$(INCLUDEDIR)/%.h,$(MODULES))
OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
OBJ+=$(patsubst %,$(ODIR)/%.o,$(MAIN))
TEST_OBJ=$(patsubst %,$(ODIR_TEST)/%.o,$(TESTABLE_MODULES))
TEST_OBJ+=$(patsubst %,$(ODIR_TEST)/%.o,$(TEST_MAIN))

# main

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) $(OTHER_FLAGS) -c -o $@ $< $(CFLAGS)

yamenu: $(OBJ)
	$(CC) $(OTHER_FLAGS) -o $(BINDIR)/$@ $^ $(LIBS)


# test

$(ODIR_TEST)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o $@ $< $(CFLAGS_TEST)

build_test: $(TEST_OBJ)
	$(CC) -o $(BINDIR)/${TEST_MAIN} $^ $(LIBS_TEST) -l cmocka

test: build_test
	$(BINDIR)/$(TEST_MAIN)

leaktest: build_test
	valgrind -s $(BINDIR)/$(TEST_MAIN)


# other useful things

.PHONY: clean
clean:
	@echo Cleaning stuff. This make file officially is doing better than you irl.
	rm -f $(ODIR)/*.o
	rm -f $(BINDIR)/*
	rm -f $(ODIR_TEST)/*.o

.PHONY: setup
init:
	mkdir -p $(ODIR)
	mkdir -p $(BINDIR)
	mkdir -p $(ODIR_TEST)

.PHONY: install
install:
	mkdir -p ${INSTALLDIR}/view
	cp view/window_main.glade ${INSTALLDIR}/view/
	cp run_yamenu.sh ${INSTALLDIR}/
	cp ${BINDIR}/${BIN} ${INSTALLDIR}/${BIN}
