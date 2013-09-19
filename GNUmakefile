.PHONY: all sdist dist rpm

PROJECT = icmpecho
SOURCES = ~/rpmbuild/SOURCES
VERSION = 1.0.0
PREFIX = /usr
SOURCE_TAR = $(SOURCES)/$(PROJECT)-$(VERSION).tar.gz
DIST_TAR = $(PROJECT)-$(VERSION)-`uname -m`.tar.gz
DESTDIR=$(shell pwd)/install

all: dist

sdist: $(SOURCE_TAR)

dist: $(DIST_TAR)

$(PROJECT).spec: in.spec
	sed in.spec -e 's/@@VERSION@@/$(VERSION)/' | sed -e 's/@@PREFIX@@/$(subst /,\/,$(PREFIX))/' >| $(PROJECT).spec

rpm: sdist $(PROJECT).spec
	rpmbuild -ba $(PROJECT).spec

$(SOURCES):
	mkdir -p $(SOURCES)

$(SOURCE_TAR): $(SOURCES)
	tar czf $(SOURCE_TAR) $(PROJECT)

$(DESTDIR):
	mkdir -p $(DESTDIR)

$(DIST_TAR): $(DESTDIR)
	cd $(PROJECT) && make uninstall clean build install DESTDIR=$(DESTDIR)
	tar czf $(DIST_TAR) -C install .

clean:
	rm -rf $(DESTDIR)
	rm -f $(DIST_TAR)
	rm -f $(SOURCE_TAR)
	rm -f $(PROJECT).spec
