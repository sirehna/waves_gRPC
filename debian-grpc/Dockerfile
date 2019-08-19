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
        ca-certificates \
        wget \
    && \
    apt-get -t stretch-backports install --yes --no-install-recommends \
        libgrpc++-dev \
        libgrpc++1 \
        libgrpc-dev \
        libgrpc6 \
        libprotobuf-dev \
        libprotoc-dev \
        protobuf-compiler-grpc

# googletest : BSD 3-Clause "New" or "Revised" License
RUN wget https://github.com/google/googletest/archive/release-1.8.1.tar.gz -O googletest.tar.gz && \
    mkdir -p /opt/googletest && \
    tar -xf googletest.tar.gz --strip 1 -C /opt/googletest && \
    rm -rf googletest.tar.gz

ADD wave.proto /
