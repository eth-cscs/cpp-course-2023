FROM gitpod/workspace-full

ENV CLANG_VERSION 17

USER root

RUN echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${CLANG_VERSION} main" >> /etc/apt/sources.list && \
    echo "deb-src http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${CLANG_VERSION} main" >> /etc/apt/sources.list && \
    wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc && \
    apt-get update -qq && \
    DEBIAN_FRONTEND=noninteractive apt-get install -qq -y clang-${CLANG_VERSION} libomp-${CLANG_VERSION}-dev clangd-${CLANG_VERSION} && \
    rm -rf /var/lib/apt/lists/*

USER gitpod

RUN brew install marp-cli
