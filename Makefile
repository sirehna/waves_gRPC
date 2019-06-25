.PHONY: all cpp-perf-test debian-grpc ghz-perf-test pylama python report test

all: gtest python

debian-grpc: debian-grpc/Dockerfile
	docker build -t debian-grpc debian-grpc

cpp-perf-test: performance.md

performance.md: debian-grpc compose-cpp-perf-test.yml
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-cpp-perf-test.yml up -t 0 --exit-code-from client --abort-on-container-exit --build
	docker-compose -f compose-cpp-perf-test.yml ps | grep client | awk '{print $$1}' | xargs -n1 docker logs > performance.md

gtest: debian-grpc compose-gtest.yml
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-gtest.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

ghz-perf-test: debian-grpc compose-ghz-perf-test.yml
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-ghz-perf-test.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

pylama:
	cd pylama && make

python: compose-python.yml python_client/* python_server/* wave_types.proto wave_grpc.proto pylama
	@docker run -t --rm -v $(shell pwd):/work -w /work -u $(shell id -u):$(shell id -g) pylama /work/python_server
	@docker run -t --rm -v $(shell pwd):/work -w /work -u $(shell id -u):$(shell id -g) pylama /work/python_client
	make -C python_server
	make -C python_client
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-python.yml up -t 0 --exit-code-from client --abort-on-container-exit --build --force-recreate

report: performance.html

performance.html: performance.md
	docker build -t pandoc pandoc
	docker run -w /work -v `pwd`:/work -u $(shell id -u):$(shell id -g) pandoc -s --mathml --highlight-style pygments -o performance.html performance.md

