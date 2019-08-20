FROM debian-grpc AS builder
WORKDIR /work
ADD CMakeLists.txt wave_client.cc wave_client.hh wave_test.cc /work/
RUN mkdir build \
 && cd build \
 && cmake -Wno-dev \
	          -G Ninja \
	          -DCMAKE_BUILD_TYPE=Release \
	          -DCMAKE_INSTALL_PREFIX:PATH=/opt/grpc_demo \
	          -DTHIRDPARTY_GOOGLETEST:PATH=/opt/googletest \
	          /work
RUN cd build && ninja wave_test

FROM debian:9-slim
ADD wait-for-it.sh /usr/.
COPY --from=builder /work/build/wave_test /usr
RUN chmod 777 /usr/wait-for-it.sh /usr/wave_test
ENTRYPOINT ["/usr/wave_test"]
