#
# Makefile for pyraw.
#

.PHONY: clean-pyraw

PYRAW_BINARY = pyraw/pyraw
PYRAW_SRC = pyraw/pyraw.c

ZENHOME=/opt/zenoss
CC = gcc
CFLAGS = -I$(ZENHOME)/include/python2.7
LDLIBS = -lpython2.7 -ldl -lpthread -lutil -lm
LDFLAGS = -L$(ZENHOME)/lib -Xlinker -export-dynamic -Wl,-rpath,$(ZENHOME)/lib

clean-pyraw:
	rm -rf $(PYRAW_BINARY)

build-pyraw: $(PYRAW_BINARY)

$(PYRAW_BINARY): $(PYRAW_SRC)
	@echo "Compiling $(PYRAW_BINARY)"
	$(DOCKER_RUN) "cd /mnt && $(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(PYRAW_SRC) -o $@"
