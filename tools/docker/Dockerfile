
FROM ubuntu:jammy

ENV DEBIAN_FRONTEND noninteractive

LABEL maintainer="morantr@gmail.com"

ARG PACKAGES_FOR_BUILD="\
    cmake \
    gcc \
    git \
    ninja-build \
    python3 \
    "
ARG PACKAGES_FOR_DEBUG="\
    gdb \
    vim \
    "

ARG PACKAGES="\
    $PACKAGES_FOR_BUILD \
    $PACKAGES_FOR_DEBUG\
    "

RUN apt-get update && apt-get -y upgrade
RUN apt-get install -y $PACKAGES software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt update && apt install gcc-11 g++-11 cmake gdb gdbserver -y
    
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 999 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 999 \
 && update-alternatives --install /usr/bin/cc  cc  /usr/bin/gcc-11 999 \
 && update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-11 999

# Clean up APT when done.
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*


# for gdbserver
EXPOSE 2000

ENV DEBIAN_FRONTEND teletype
