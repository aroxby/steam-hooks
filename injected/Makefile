SRCS=$(wildcard *.cpp)
TARGET=injected.dll

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRCS)
	g++ -static -shared $^ -o $@ -lminhook

clean:
	rm -f $(TARGET) *.o
