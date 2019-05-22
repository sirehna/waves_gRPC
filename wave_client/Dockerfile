FROM debian-grpc AS builder
WORKDIR /work
ADD args.hxx CMakeLists.txt wave_client.cc wave_client.hh wave_client_main.cc /work/
RUN mkdir build \
 && cd build \
 && cmake -Wno-dev \
	          -G Ninja \
	          -DCMAKE_BUILD_TYPE=Relase \
	          -DCMAKE_INSTALL_PREFIX:PATH=/opt/grpc_demo \
	          /work
RUN cd build && ninja

FROM debian:9-slim
RUN apt-get update && \
    apt-get install --yes --no-install-recommends \
        time
ADD wait-for-it.sh /usr/.
RUN chmod 777 /usr/wait-for-it.sh
COPY --from=builder /work/build/wave_client /usr
RUN chmod 777 /usr/wave_client
CMD /usr/wave_client
