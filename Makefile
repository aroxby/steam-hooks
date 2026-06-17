MINHOOK_MAKEFILE=build/MinGW/Makefile
MINHOOK_MAKE=make -C minhook -f $(MINHOOK_MAKEFILE)

.PHONY: all clean example injected injector minhook mini-steam
all: example injected injector minhook mini-steam

example: mini-steam
	make -C $@

injected: minhook
	make -C $@

injector mini-steam:
	make -C $@

$(MINHOOK_MAKEFILE):
	git submodule update --init

minhook: $(MINHOOK_MAKEFILE)
	$(MINHOOK_MAKE)

clean:
	make -C example clean
	make -C injected clean
	make -C injector clean
	$(MINHOOK_MAKE) clean
	make -C mini-steam clean
