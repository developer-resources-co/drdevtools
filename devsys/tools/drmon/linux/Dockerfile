# Throwaway toolchain image for building drmon on Linux.
# Nothing is installed on the host; the build runs inside this container with the
# repo mounted. See docs/plans/2026-06-10-port-drmon-linux.md.
# Base: 26.04 (the version Foundry Linux / Kubuntu target) — it ships libcppdap-dev
# 1.58.0a as a first-class package, so the DAP front end builds out of the box.
FROM ubuntu:26.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        g++ \
        clang \
        cmake \
        ninja-build \
        bison \
        flex \
        make \
        file \
        gdb \
        git \
        tmux \
        xterm \
        python3 \
        libjsoncpp-dev \
        libcppdap-dev \
        libncurses-dev \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
