SRCS=$(wildcard *.cpp)
TARGET=injector

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRCS)
	g++ $^ -o $@

clean:
	rm -f $(TARGET) *.o
