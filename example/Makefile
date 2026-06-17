SRCS=$(wildcard *.cpp)
TARGET=example
MINI_STEAM_DIR=../mini-steam
MINI_STEAM_LIB=steam_api.dll
MINI_STEAM_LIB_SRC=$(MINI_STEAM_DIR)/$(MINI_STEAM_LIB)

.PHONY: all clean
all: $(TARGET)

$(MINI_STEAM_LIB_SRC):
	make -C $(MINI_STEAM_DIR)

$(MINI_STEAM_LIB): $(MINI_STEAM_LIB_SRC)
	cp $< $@

$(TARGET): $(SRCS) $(MINI_STEAM_LIB)
	g++ $^ -o $@

clean:
	rm -f $(TARGET) *.o $(MINI_STEAM_LIB)
