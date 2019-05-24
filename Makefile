.PHONY: all cpp-perf-test debian-grpc ghz-perf-test python report test

all: gtest

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

python: compose-python.yml python_client/* python_server/* waves.proto
	@cp waves.proto python_client
	@cp waves.proto python_server
	@CURRENT_UID=$(shell id -u):$(shell id -g) docker-compose -f compose-python.yml up -t 0 --exit-code-from client --abort-on-container-exit --build --force-recreate
	@rm python_client/waves.proto
	@rm python_server/waves.proto

report: performance.html

performance.html: performance.md
	docker build -t pandoc pandoc 
	docker run -w /work -v `pwd`:/work -u $(shell id -u):$(shell id -g) pandoc -s --mathml --highlight-style pygments -o performance.html performance.md

