FROM centos/devtoolset-7-toolchain-centos7:latest

## install git&wget
USER root
RUN yum -y install git && yum -y install wget

WORKDIR /
## install cmake
RUN mkdir -p /tmp/cmake && \
    pushd /tmp/cmake && \
    wget 'https://cmake.org/files/v3.20/cmake-3.20.0-linux-x86_64.sh' && \
    bash cmake-3.20.0-linux-x86_64.sh --prefix=/usr/local --exclude-subdir && \
    popd && \
    rm -rf /tmp/cmake

ENV PATH="/usr/local/bin:${PATH}"
ENV LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib64/
## Build pinpoint_common
COPY ./common pinpoint_common/
RUN pushd / && cd pinpoint_common && mkdir build && cd build && cmake .. && make && make install && popd && rm -rf pinpoint_common
ENV PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:$PKG_CONFIG_PATH

## install golang 1.13.10
RUN wget -P /tmp 'https://golang.org/dl/go1.13.10.linux-amd64.tar.gz' && \
    rm -rf /usr/local/go && tar -C /usr/local -xzf /tmp/go1.13.10.linux-amd64.tar.gz && \
    rm /tmp/go1.13.10.linux-amd64.tar.gz
ENV PATH="${PATH}:/usr/local/go/bin"
RUN pkg-config --cflags pinpoint_common

## install collector-agent

COPY ./collector-agent /tmp/collector-agent

RUN cd /tmp/collector-agent && go build  -o  CollectorAgent server.go && cp CollectorAgent /usr/local/bin/ && rm -rf /tmp/collector-agent