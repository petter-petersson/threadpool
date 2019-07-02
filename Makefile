-include config.mk
override CFLAGS+=-std=c99 -D_GNU_SOURCE
LIBS= -lpthread -lbtree

DEBUG_FLAGS=-g -DDEBUG
BUILD_DIR=release
LDFLAGS = $(REL_LDFLAGS)

.PHONY: clean test lib debug_lib debug mkdirs

mkdirs:
	mkdir -p debug
	mkdir -p release 

debug: clean
debug: CFLAGS += $(DEBUG_FLAGS)
debug: LDFLAGS = $(DBG_LDFLAGS)
debug: test

debug_lib: clean
debug_lib: CFLAGS += $(DEBUG_FLAGS)
debug_lib: LDFLAGS = $(DBG_LDFLAGS)
debug_lib: mkdirs
debug_lib: BUILD_DIR=debug
debug_lib: lib

lib: clean
lib: mkdirs
lib: libthreadpool.a

libthreadpool.a: threadpool.o
	$(AR) rc $(BUILD_DIR)/$@ $^

%.o: %.c
	$(CC) -c  $(CFLAGS) $< -o $@

test: threadpool_test
	./threadpool_test

threadpool_test: test.o threadpool_test.o threadpool.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

clean:
	rm -rf *.o *.a *_test

