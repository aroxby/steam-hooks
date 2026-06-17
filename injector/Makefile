SRCS=$(wildcard *.cpp)
TARGET=injector

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRCS)
	g++ -static $^ -o $@

clean:
	rm -f $(TARGET) *.o
