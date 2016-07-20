#
# Makefile for icmpecho
#
VERSION = 1.0.1-dev

# Define the image name, version and tag name for the docker build image
BUILD_IMAGE = build-tools
BUILD_VERSION = 0.0.3
TAG = zenoss/$(BUILD_IMAGE):$(BUILD_VERSION)

UID := $(shell id -u)
GID := $(shell id -g)

DOCKER_RUN := docker run --rm \
		-v $(PWD):/mnt \
		--user $(UID):$(GID) \
		$(TAG) \
		/bin/bash -c

include pyraw.mk

build-bdist: setup.py
	@echo "Building a binary distribution of icmpecho"
	$(DOCKER_RUN) "cd /mnt && python setup.py bdist_wheel"

build-sdist: setup.py
	@echo "Building a source distribution of icmpecho"
	$(DOCKER_RUN) "cd /mnt && python setup.py sdist"

setup.py:
	@sed -e "s/%VERSION%/$(VERSION)/g" < setup.py.in > setup.py

# Default to building a binary distribution of the wheel
build: build-bdist build-pyraw

clean: clean-pyraw
	rm -rf setup.py *.pyc MANIFEST dist build icmpecho.egg-info

#
# Bundle up the wheel and the pyraw binary into a tar artifact
#
install:
	mv pyraw/pyraw dist
	cd dist && tar cvfz icmpecho-$(VERSION).tar.gz ./*
