#
# Makefile for icmpecho
#
VERSION = 1.0.1

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

IN_DOCKER = 1

include pyraw.mk

build-bdist: setup.py
	@echo "Building a binary distribution of icmpecho $(IN_DOCKER)"
	if [ -n "$(IN_DOCKER)" ]; then \
		$(DOCKER_RUN) "cd /mnt && python setup.py bdist_wheel"; \
	else \
		python setup.py bdist_wheel; \
	fi

build-sdist: setup.py
	@echo "Building a source distribution of icmpecho"
	if [ -n "$(IN_DOCKER)" ]; then \
		$(DOCKER_RUN) "cd /mnt && python setup.py sdist"; \
	else \
		python setup.py sdist; \
	fi

# Default to building a binary distribution of the wheel
build: build-bdist build-pyraw

clean: clean-pyraw
	rm -rf *.pyc MANIFEST dist build icmpecho.egg-info

#
# Bundle up the wheel and the pyraw binary into a tar artifact
#
install:
	mv pyraw/pyraw dist
	cd dist && tar cvfz icmpecho-$(VERSION).tar.gz ./*
