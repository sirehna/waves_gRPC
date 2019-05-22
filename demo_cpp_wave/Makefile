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

dc_test: debian-grpc
	@docker-compose -f compose_demo.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

dc_gtest: debian-grpc
	@docker-compose -f compose_test.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

dc_perf: debian-grpc
	@docker-compose -f compose_ghz.yml up -t 0 --exit-code-from client --abort-on-container-exit --build

