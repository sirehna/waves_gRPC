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

debian-grpc: debian_grpc/Dockerfile
	docker build -t debian_grpc debian_grpc

cmake: debian-grpc
	mkdir -p build_debian
	docker run --rm \
	    -v $(shell pwd):/shared \
	    -v $(shell pwd)/../third_party:/third_party \
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
	    -v $(shell pwd):/shared \
	    -v $(shell pwd)/../third_party:/third_party \
	    -u $(shell id -u):$(shell id -g) \
	    -w /shared/build_debian \
	    debian_grpc \
	    ninja

docker_server:build
	cp build_debian/wave_server docker_wave_server/.
	cd docker_wave_server && docker build . -t docker_wave_server && rm wave_server && cd ..

docker_client:build
	cp build_debian/wave_client docker_wave_client/.
	cd docker_wave_client && docker build . -t docker_wave_client && rm wave_client && cd ..

docker_test:build
	cp build_debian/wave_test docker_wave_test/.
	cd docker_wave_test && docker build . -t docker_wave_test && rm wave_test && cd ..

docker_ghz:
	cd docker_wave_ghz && docker build . -t docker_wave_ghz && cd ..

docker_images:docker_server docker_client
	echo "Deploying server and client"

dc_test:docker_images
	cd compose_demo && docker-compose up --force-recreate -d
	# cd compose_demo && sleep 0.5 && \
	#     while [ -z "`docker-compose ps | grep client | grep Exit`" ]; do echo "Client terminated"; done
	cd compose_demo && sleep 55 &&\
	    result=`docker-compose ps | grep client | grep "Exit 0"` || true && \
	    echo "$${result}" && \
	    if [ -n "$${result}" ]; \
	       then echo "Success" && docker-compose logs -t; \
	       else echo "Failure" && docker-compose logs -t; \
	       fi

dc_gtest:docker_server docker_test
	cd compose_test && docker-compose up --force-recreate -d
	cd compose_test && docker-compose logs -t
	cd compose_test && \
	    result=`docker-compose ps | grep test | grep "Exit 0"` || true && \
	    echo "$${result}" && \
	    if [ -n "$${result}" ]; \
	       then echo "Success"; \
	       else echo "Failure" && docker-compose logs test; \
	       fi

dc_perf:docker_server docker_ghz
	cp wave.proto compose_ghz/.
	cd compose_ghz && docker-compose up -d
	cd compose_ghz && sleep 0.5 && while [ -z "`docker-compose ps | grep client_ghz | grep Exit`" ]; do echo "Client terminated";done
	cd compose_ghz && \
	    result=`docker-compose ps | grep client | grep "Exit 0"` || true && \
	    echo "$${result}" && \
	    if [ -n "$${result}" ]; \
	        then echo "Success"; \
	        else echo "Failure"; \
	        fi \
	    && docker-compose logs client_ghz server
	rm compose_ghz/wave.proto

dc_stop_demo:
	cd compose_demo && docker-compose stop && docker-compose down && cd ..

dc_stop_test:
	cd compose_test && docker-compose stop && docker-compose down && cd ..

dc_stop_perf:
	cd compose_ghz && docker-compose stop && docker-compose down && cd ..

dc_stop:dc_stop_demo dc_stop_test dc_stop_perf
	echo "Stopping docker compose"

dc_clean:dc_stop
	docker rmi -f docker_wave_client docker_wave_ghz docker_wave_server docker_wave_test
