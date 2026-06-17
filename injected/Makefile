SRCS=$(wildcard *.cpp)
TARGET=injected.dll
MINHOOK_DIR=../minhook
MINHOOK_INCLUDE=$(MINHOOK_DIR)/include
MINHOOK_LIB=$(MINHOOK_DIR)/libMinHook.a
MIN_HOOK_MAKE=make -C $(MINHOOK_DIR) -f build/MinGW/Makefile

.PHONY: all clean
all: $(TARGET)

$(MINHOOK_LIB):
	$(MIN_HOOK_MAKE)

$(TARGET): $(SRCS) $(MINHOOK_LIB)
	g++ -static -shared $^ -o $@ -I $(MINHOOK_INCLUDE)

clean:
	rm -f $(TARGET) *.o
