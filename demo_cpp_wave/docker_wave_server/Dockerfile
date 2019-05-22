FROM debian-grpc AS builder
WORKDIR /work
ADD CMakeLists.txt wave_server.cc args.hxx /work/

RUN mkdir build \
 && cd build \
 && cmake -Wno-dev \
	          -G Ninja \
	          -DCMAKE_BUILD_TYPE=Relase \
	          -DCMAKE_INSTALL_PREFIX:PATH=/opt/grpc_demo \
	          /work
RUN cd build && ninja

FROM debian:9-slim
COPY --from=builder /work/build/wave_server /usr
ENTRYPOINT ["/usr/wave_server"]
