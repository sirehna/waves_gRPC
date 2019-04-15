.PHONY: all build cmake stop test

all: build

test: build run_wave_client stop
	@echo "Building app, building network, running server, and running client"

clean: stop
	rm -rf build_debian binMSVC

cwd := $(shell pwd)
archive:
	echo $(cwd) && \
	echo $(shell basename $(cwd)) && \
	cd .. && \
	tar -zcf GRPC_demo.tgz \
	    --exclude='$(shell basename $(cwd))/build*' \
	    --exclude='$(shell basename $(cwd))/binMSVC' \
	    --exclude='$(shell basename $(cwd))/.vscode' \
	    --exclude='$(shell basename $(cwd))/report*' \
	    $(shell basename $(cwd)) && \
	cd $(cwd)

build: cmake
	docker run --rm -v $(shell pwd):/shared -u $(shell id -u):$(shell id -g) -w /shared/build_debian debian_grpc ninja

cmake:
	docker build -t debian_grpc .
	mkdir -p build_debian
	docker run --rm -v $(shell pwd):/shared -u $(shell id -u):$(shell id -g) -w /shared/build_debian debian_grpc \
	    cmake -Wno-dev \
	          -G Ninja \
	          -DCMAKE_BUILD_TYPE=Relase \
	          -DCMAKE_INSTALL_PREFIX:PATH=/opt/grpc_demo \
	          /shared

network:
	if docker network list | grep grpc_net > /dev/null ; then echo ""; else docker network create grpc_net; fi;

run_wave_server:network
	@if docker ps | grep grpc_wave_server > /dev/null ; \
	    then \
	    echo ""; \
	    else \
	    echo "Launching wave server" && \
	        docker run -d \
	        --net=grpc_net \
	        --name grpc_wave_server \
	        -p 50051:50051 \
	        --rm \
	        -v $(shell pwd):/shared \
	        -u $(shell id -u):$(shell id -g) \
	        -w /shared/build_debian \
	        debian_grpc \
	        ./wave_server; \
	    fi;

run_wave_client:run_wave_server
	docker run \
	    --net=grpc_net \
	    --name grpc_wave_client \
	    -p 50052:50052 \
	    --rm \
	    -v $(shell pwd):/shared \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian debian_grpc \
	    ./wave_client \
	    --ip `docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_wave_server` \
	    --port 50051

run_wave_client_debug:run_wave_server
	docker run \
	    --net=grpc_net \
	    --name grpc_wave_client \
	    -p 50052:50052 \
	    --rm \
	    -v $(shell pwd):/shared \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian debian_grpc \
	    /bin/sh -c "export GRPC_VERBOSITY=DEBUG && \
	                ./wave_client \
	                    --ip `docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_wave_server` \
	                    --port 50051"
	    #/bin/sh -c "export GRPC_TRACE=all && export GRPC_VERBOSITY=DEBUG && ./greeter_wave_client"

stop:
	@if docker ps | grep grpc_wave_server > /dev/null ; then echo "Stopping grpc_wave_server" && docker rm -f grpc_wave_server; fi;
	@if docker ps | grep grpc_wave_client > /dev/null ; then echo "Stopping grpc_wave_client" && docker rm -f grpc_wave_client; fi;
	@if docker network list | grep grpc_net > /dev/null ; then echo "Deleting grpc_net" && docker network rm grpc_net; fi;

inspect:
	@if docker ps | grep grpc_wave_server > /dev/null ; \
	    then \
	    echo "Inspect grpc_wave_server" && docker inspect grpc_wave_server -f "{{json .NetworkSettings.Networks }}"; \
	    fi;

get_server_ip_from_server:
	@if docker ps | grep grpc_wave_server > /dev/null ; \
	    then \
	    echo "Inspect grpc_wave_server" && \
	    docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_wave_server; \
	    fi;
