FROM alpine:latest as builder

RUN apk add cmake make g++ upx

ADD ./src /opt/sources
WORKDIR  /opt/sources

RUN mkdir build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release .. && \
    make && make test  

WORKDIR /opt/sources/build

ENTRYPOINT [ "./hello" ]
