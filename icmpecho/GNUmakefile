PROJECT = pyraw
LDLIBS  = -lpython2.7 -ldl
CC     ?= gcc

CFLAGS += -I/usr/include/python2.7
LDLIBS += -lpthread -lutil -lm
LDFLAGS += -Xlinker -export-dynamic

PYTHON = python

DESTDIR= install

all: build

build: $(PROJECT)
	$(PYTHON) setup.py build

install:
	$(PYTHON) setup.py install --root $(DESTDIR) --home .
	mkdir -p $(DESTDIR)/bin
	cp -p pyraw $(DESTDIR)/bin

uninstall:
	rm -rf $(DESTDIR)

$(PROJECT): $(PROJECT).o
	$(LINK.c) $< $(LDLIBS) $(OUTPUT_OPTION)

clean:
	$(PYTHON) setup.py clean
	rm -rf *.o pyraw
