RAKE=		rake
GEM=		gem

default: PHONY build

build: PHONY
	$(RAKE) compile

test t: PHONY
	$(RAKE) test

gem: PHONY
	$(RAKE) build

upload: PHONY
	$(RM) pkg/*.gem
	$(MAKE) gem
	$(GEM) push pkg/*.gem

install: PHONY
	$(RAKE) install


clean: PHONY
	$(RAKE) clean

distclean clobber: PHONY
	$(RAKE) clobber

PHONY:

