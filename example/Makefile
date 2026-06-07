SRCS=$(wildcard *.cpp)
TARGET=example
STEAM_LIB=steam_api.dll
STEAM_LIB_SRC=../mini-steam/$(STEAM_LIB)

.PHONY: all clean
all: $(TARGET)

$(STEAM_LIB): $(STEAM_LIB_SRC)
	cp $< $@

$(TARGET): $(SRCS) $(STEAM_LIB)
	g++ $^ -o $@

clean:
	rm -f $(TARGET) *.o $(STEAM_LIB)
