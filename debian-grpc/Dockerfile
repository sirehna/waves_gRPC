FROM debian:9
RUN echo "deb http://deb.debian.org/debian stretch-backports main" >>  /etc/apt/sources.list && \
    apt-get update && \
    apt-get install --yes --no-install-recommends \
        make \
        cmake \
        build-essential \
        g++ \
        gcc \
        libbz2-dev \
        libc-ares-dev \
        libssl-dev \
        ninja-build \
    && \
    apt-get -t stretch-backports install --yes --no-install-recommends \
        libgrpc++-dev \
        libgrpc++1 \
        libgrpc-dev \
        libgrpc6 \
        libprotobuf-dev \
        libprotoc-dev \
        protobuf-compiler-grpc
ADD wave.proto /
