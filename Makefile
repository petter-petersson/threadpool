-include config.mk
override CFLAGS+=-std=c99 -D_GNU_SOURCE
LIBS= -lpthread -lbtree

DEBUG_FLAGS=-g -DDEBUG
BUILD_DIR=release
LDFLAGS = $(REL_LDFLAGS)

.PHONY: clean test _test lib debug_lib debug mkdirs

mkdirs:
	mkdir -p debug
	mkdir -p release 

debug_lib: CFLAGS += $(DEBUG_FLAGS)
debug_lib: LDFLAGS = $(DBG_LDFLAGS)
debug_lib: mkdirs
debug_lib: BUILD_DIR=debug
debug_lib: lib

lib: mkdirs
lib: libthreadpool.a

libthreadpool.a: threadpool.o
	$(AR) rc $(BUILD_DIR)/$@ $^

%.o: %.c
	$(CC) -c  $(CFLAGS) $< -o $@


test: clean
test: CFLAGS += $(DEBUG_FLAGS)
test: LDFLAGS = $(DBG_LDFLAGS)
test: debug_lib
test: BUILD_DIR=debug
test: _test

_test: threadpool_test
	./threadpool_test

threadpool_test: test.o threadpool_test.o
	$(CC) $(CFLAGS) $(LDFLAGS) -L$(BUILD_DIR) -lthreadpool $(LIBS) $^ -o $@

clean:
	rm -rf *.o *.a *_test debug/*.a release/*.a

