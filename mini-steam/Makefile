SRCS=$(wildcard *.cpp)
TARGET=steam_api.dll
IMPORT_LIB=steam_api.a

.PHONY: all clean
all: $(TARGET) $(IMPORT_LIB)

$(TARGET) $(IMPORT_LIB): $(SRCS)
	g++ -DSTEAM_API_EXPORTS -shared $^ -o $@ -Wl,--out-implib,$(IMPORT_LIB)

clean:
	rm -f $(TARGET) $(IMPORT_LIB) *.o
