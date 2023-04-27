# First stage for building the software:
FROM ubuntu:22.04 as builder

ENV DEBIAN_FRONTEND noninteractive

RUN apt update -y && \
    apt upgrade -y 

# Install the development libraries for OpenCV
RUN apt install -y --no-install-recommends \
        ca-certificates \
        cmake \
        build-essential \
        libopencv-dev

# Include this source tree and compile the sources
ADD . /opt/sources
WORKDIR /opt/sources
RUN mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/tmp .. && \
    make && make install


# Second stage for packaging the software into a software bundle:
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt update -y && \
    apt upgrade -y

RUN apt install -y --no-install-recommends \
        libopencv-core4.5d \
        libopencv-highgui4.5d \
        libopencv-imgproc4.5d 

WORKDIR /usr/bin
COPY --from=builder /tmp/bin/steer-clear .
# This is the entrypoint when starting the Docker container; hence, this Docker image is automatically starting our software on its creation
ENTRYPOINT ["/usr/bin/steer-clear"]
