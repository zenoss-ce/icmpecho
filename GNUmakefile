.PHONY: all dist rpm

PROJECT = icmpecho
SOURCES = ~/rpmbuild/SOURCES
VERSION = $(shell grep Version $(PROJECT).spec | cut -f 2)
SOURCE_TAR = $(SOURCES)/$(PROJECT)-$(VERSION).tar.gz

all: dist

dist: $(SOURCE_TAR)

rpm: dist
	rpmbuild -ba $(PROJECT).spec

$(SOURCE_TAR):
	mkdir -p $(SOURCES)
	tar czf $(SOURCE_TAR) $(PROJECT)
