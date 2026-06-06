SRCS=$(wildcard *.cpp)
TARGET=injected.dll

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRCS)
	g++ -shared $^ -o $@ -lMinHook

clean:
	rm -f $(TARGET) *.o
