SRCS=$(wildcard *.cpp)
TARGET=injected.dll

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRCS)
	g++ -shared $^ -o $@

clean:
	rm -f $(TARGET) *.o
