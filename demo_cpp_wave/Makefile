.PHONY: all build cmake debian-grpc stop test

# make dc_test -> performs grpc test with docker-compose, it will
#                 create docker base image
#                 build executables in the build directory,
#                 create docker deployment images,
#                 setup docker compose in detached mode,
#                 wait for client to end up,
#                 check for client return code
#
# make dc_perf -> performs grpc test with docker-compose and ghz
#
# make dc_clean -> deletes build directory and deletes docker images

all: dc_test

debian-grpc: debian-grpc/Dockerfile
	docker build -t debian-grpc debian-grpc

cmake: debian-grpc
	mkdir -p build_debian
	docker run --rm \
	    -v $(shell pwd):/shared \
	    -v $(shell pwd)/../third_party:/third_party \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian \
	    debian-grpc \
	    cmake -Wno-dev \
	          -G Ninja \
	          -DCMAKE_BUILD_TYPE=Relase \
	          -DCMAKE_INSTALL_PREFIX:PATH=/opt/grpc_demo \
	          /shared

build: cmake
	docker run --rm \
	    -v $(shell pwd):/shared \
	    -v $(shell pwd)/../third_party:/third_party \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian \
	    debian-grpc \
	    ninja

docker_test:build
	cp build_debian/wave_test docker_wave_test/.
	cd docker_wave_test && docker build . -t docker_wave_test && rm wave_test && cd ..

dc_test:
	@docker-compose -f compose_demo.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

dc_gtest:
	@docker-compose -f compose_test.yml up -t 0 --exit-code-from test --abort-on-container-exit --build

dc_perf:
	@docker-compose -f compose_ghz.yml up -t 0 --exit-code-from client_ghz --abort-on-container-exit --build

