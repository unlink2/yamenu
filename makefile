CC=gcc
DBG=gdb
BIN=yamenu

INCLUDEDIR=./src/include
SRCDIR=./src
ODIR=./obj
BINDIR=./bin
TEST_DIR = ./tests

LIBS=
CFLAGS=-Wall -g
CFLAGS_RELEASE=-Wall -O1
MAIN = main
TEST_MAIN = test
INSTALLDIR = /usr/local/bin

MODULES = utility data commandline path

DEPS=$(patsubst %,$(INCLUDEDIR)/%.h,$(MODULES))
OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
OBJ+=$(patsubst %,$(ODIR)/%.o,$(MAIN))
TEST_OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
TEST_OBJ+=$(patsubst %,$(ODIR)/%.o,$(TEST_MAIN))

# main

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o $@ $< $(CFLAGS)

yamenu: $(OBJ)
	$(CC) -o $(BINDIR)/$@ $^ $(LIBS)


# test

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o $@ $< $(CFLAGS)

build_test: $(TEST_OBJ)
	$(CC) -o $(BINDIR)/${TEST_MAIN} $^ $(LIBS) -l cmocka

test: build_test
	$(BINDIR)/$(TEST_MAIN)

leaktest: build_test
	valgrind -s $(BINDIR)/$(TEST_MAIN)

# build test asm files
buildtests: $(TEST_CODE)
	../scripts/buildtests.sh



# other useful things

.PHONY: clean
clean:
	@echo Cleaning stuff. This make file officially is doing better than you irl.
	rm -f $(ODIR)/*.o
	rm -f $(BINDIR)/*
	rm -f $(ODIR)/release/*.o

.PHONY: setup
init:
	mkdir -p $(ODIR)
	mkdir -p $(BINDIR)
	mkdir -p $(ODIR)/release

.PHONY: install
install:
	cp ${BINDIR}/${BIN} ${INSTALLDIR}/${BIN}