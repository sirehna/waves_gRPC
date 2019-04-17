.PHONY: all build cmake stop test
.PHONY: dc_test dc_clean
.PHONY: all_clean

# make test    -> performs grpc test with docker
# make dc_test -> performs grpc test with docker-compose, it will
#                 create docker base image
#                 build executables in the build directory,
#                 create docker deployment images,
#                 setup docker compose in detached mode,
#                 wait for client to end up,
#                 check for client return code
#
# make all_clean deletes build directory and deletes docker images

all: dc_test

all_clean: clean dc_clean
all_stop: stop dc_stop

cmake:
	docker build -t debian_grpc .
	mkdir -p build_debian
	docker run --rm \
	    -v $(shell pwd):/shared -v $(shell pwd)/../third_party:/third_party \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian \
	    debian_grpc \
	    cmake -Wno-dev \
	          -G Ninja \
	          -DCMAKE_BUILD_TYPE=Relase \
	          -DCMAKE_INSTALL_PREFIX:PATH=/opt/grpc_demo \
	          /shared

build: cmake
	docker run --rm \
	    -v $(shell pwd):/shared -v $(shell pwd)/../third_party:/third_party \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian debian_grpc ninja

docker_server:build
	cp build_debian/greeter_sync_server docker_sync_server/.
	cd docker_sync_server && docker build . -t docker_sync_server && rm greeter_sync_server && cd ..

docker_client:build
	cp build_debian/greeter_sync_client docker_sync_client/.
	cd docker_sync_client && docker build . -t docker_sync_client && rm greeter_sync_client && cd ..

docker_images:docker_server docker_client
	echo "Deploying server and client"

dc_test:docker_images
	cd compose_demo && docker-compose up --force-recreate -d
	cd compose_demo && docker-compose logs -t
	cd compose_demo && sleep 0.5 && \
	    while [ -z "`docker-compose ps | grep client | grep Exit`" ]; do echo "Client terminated"; done
	cd compose_demo && \
	    result=`docker-compose ps | grep client | grep "Exit 0"` || true && \
	    echo "$${result}" && \
	    if [ -n "$${result}" ]; \
	       then echo "Success"; \
	       else echo "Failure" && docker-compose logs client; \
	       fi

dc_stop_demo:
	cd compose_demo && docker-compose stop && docker-compose down && cd ..

dc_stop:dc_stop_demo
	echo "Stopping docker compose"

dc_clean:dc_stop
	docker rmi -f docker_sync_server docker_sync_client

test: build run_sync_client run_async_client stop
	@echo "Building apps, building network, running servers, and running clients"

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

network:
	if docker network list | grep grpc_net > /dev/null ; then echo ""; else docker network create grpc_net; fi;

run_sync_server:network
	@if docker ps | grep grpc_sync_server > /dev/null ; \
	    then \
	    echo ""; \
	    else \
	    echo "Launching sync server" && \
	        docker run -d \
	        --net=grpc_net \
	        --name grpc_sync_server \
	        -p 50051:50051 \
	        --rm \
	        -v $(shell pwd):/shared \
	        -u $(shell id -u):$(shell id -g) \
	        -w /shared/build_debian \
	        debian_grpc \
	        ./greeter_sync_server; \
	    fi;

run_sync_client:run_sync_server
	docker run \
	    --net=grpc_net \
	    --name grpc_sync_client \
	    -p 50052:50052 \
	    --rm \
	    -v $(shell pwd):/shared \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian debian_grpc \
	    ./greeter_sync_client \
	    --ip `docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_sync_server` \
	    --port 50051

run_sync_client_debug:run_sync_server
	docker run \
	    --net=grpc_net \
	    --name grpc_sync_client \
	    -p 50052:50052 \
	    --rm \
	    -v $(shell pwd):/shared \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian debian_grpc \
	    /bin/sh -c "export GRPC_VERBOSITY=DEBUG && \
	                ./greeter_sync_client \
	                    --ip `docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_sync_server` \
	                    --port 50051"
	    #/bin/sh -c "export GRPC_TRACE=all && export GRPC_VERBOSITY=DEBUG && ./greeter_sync_client"

stop:
	@if docker ps | grep grpc_sync_server > /dev/null ; then echo "Stopping grpc_sync_server" && docker rm -f grpc_sync_server; fi;
	@if docker ps | grep grpc_sync_client > /dev/null ; then echo "Stopping grpc_sync_client" && docker rm -f grpc_sync_client; fi;
	@if docker ps | grep grpc_async_client > /dev/null ; then echo "Stopping grpc_async_client" && docker rm -f grpc_async_client; fi;
	@if docker ps | grep grpc_async_server > /dev/null ; then echo "Stopping grpc_async_server" && docker rm -f grpc_async_server; fi;
	@if docker network list | grep grpc_net > /dev/null ; then echo "Deleting grpc_net" && docker network rm grpc_net; fi;

inspect:
	@if docker ps | grep grpc_sync_server > /dev/null ; \
	    then \
	    echo "Inspect grpc_sync_server" && docker inspect grpc_sync_server -f "{{json .NetworkSettings.Networks }}"; \
	    fi;

get_server_ip_from_servers:
	@if docker ps | grep grpc_sync_server > /dev/null ; \
	    then \
	    echo "Inspect grpc_sync_server" && \
	    docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_sync_server; \
	    fi;
	@if docker ps | grep grpc_async_server > /dev/null ; \
	    then \
	    echo "Inspect grpc_async_server" && \
	    docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_async_server; \
	    fi;

run_async_server:network build
	@if docker ps | grep grpc_async_server > /dev/null ; \
	    then \
	    echo ""; \
	    else \
	    echo "Launching async server" && \
	    docker run -d --net=grpc_net \
	            --name grpc_async_server \
	            -p 50053:50053 \
	            --rm \
	            -v $(shell pwd):/shared \
	            -u $(shell id -u):$(shell id -g) \
	            -w /shared/build_debian \
	            debian_grpc \
	            ./greeter_async_server; \
	    fi;

run_async_client:run_async_server
	docker run --net=grpc_net \
	        --name grpc_async_client \
	        -p 50052:50052 \
	        --rm \
	        -v $(shell pwd):/shared \
	        -u $(shell id -u):$(shell id -g) \
	        -w /shared/build_debian \
	        debian_grpc \
	        ./greeter_async_client \
	        --ip `docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_async_server` \
	        --port 50053


run_async_client_debug:run_async_server
	docker run --net=grpc_net \
	        --name grpc_async_client \
	        -p 50052:50052 \
	        --rm \
	        -v $(shell pwd):/shared \
	        -u $(shell id -u):$(shell id -g) \
	        -w /shared/build_debian \
	        debian_grpc \
	        /bin/sh -c "export GRPC_VERBOSITY=DEBUG && \
	                    ./greeter_async_client \
	                        --ip `docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' grpc_async_server` \
	                        --port 50053"
	        #/bin/sh -c "export GRPC_TRACE=all && export GRPC_VERBOSITY=DEBUG && ./greeter_async_client"


gtest:build
	docker run \
	        --rm \
	        -v $(shell pwd):/shared \
	        -u $(shell id -u):$(shell id -g) \
	        -w /shared/build_debian \
	        debian_grpc \
	        ./my_test --gtest_color=yes
# --gtest_list_tests

test_server:build
	docker run \
	        --rm \
	        -v $(shell pwd):/shared \
	        -u $(shell id -u):$(shell id -g) \
	        -w /shared/build_debian \
	        debian_grpc \
	        ./test_server --gtest_color=yes
