.PHONY: all clean example injected injector
all: example injected injector

example injected injector:
	make -C $@

clean:
	make -C example clean
	make -C injected clean
	make -C injector clean
