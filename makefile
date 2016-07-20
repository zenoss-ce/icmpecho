# Define the image name, version and tag name for the docker build image
IMAGENAME = build-tools
VERSION = 0.0.3
TAG = zenoss/$(IMAGENAME):$(VERSION)

UID := $(shell id -u)
GID := $(shell id -g)

DOCKER_RUN := docker run --rm \
		-v $(PWD):/mnt \
		--user $(UID):$(GID) \
		$(TAG) \
		/bin/bash -c

include pyraw.mk

build-bdist:
	@echo "Building a binary distribution of icmpecho"
	$(DOCKER_RUN) "cd /mnt && python setup.py bdist_wheel"

build-sdist:
	@echo "Building a source distribution of icmpecho"
	$(DOCKER_RUN) "cd /mnt && python setup.py sdist"

build-pyraw:

# Default to building a binary distribution
build: build-bdist build-pyraw

clean: clean-pyraw
	rm -rf *.pyc MANIFEST dist build icmpecho.egg-info
