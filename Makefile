.PHONY: all clean example injected injector mini-steam
all: example injected injector mini-steam

example injected: mini-steam
	make -C $@

injector mini-steam:
	make -C $@

clean:
	make -C example clean
	make -C injected clean
	make -C injector clean
	make -C mini-steam clean
