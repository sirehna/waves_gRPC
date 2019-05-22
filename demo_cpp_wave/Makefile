.PHONY: all cpp-perf-test debian-grpc ghz-perf-test test

all: gtest

debian-grpc: debian-grpc/Dockerfile
	docker build -t debian-grpc debian-grpc

cpp-perf-test: debian-grpc compose-cpp-perf-test.yml
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-cpp-perf-test.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

gtest: debian-grpc compose-gtest.yml
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-gtest.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

ghz-perf-test: debian-grpc compose-ghz-perf-test.yml
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-ghz-perf-test.yml up -t 0 --exit-code-from client --abort-on-container-exit --build
