SRCS=$(wildcard *.cpp)
TARGET=example

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRCS)
	g++ $^ -o $@

clean:
	rm -f $(TARGET) *.o
